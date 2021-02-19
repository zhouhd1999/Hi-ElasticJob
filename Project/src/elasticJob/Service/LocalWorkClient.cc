#include "LocalWorkClient.h"

#include <assert.h>

#include "muduo/base/Logging.h"
#include "../../base/RedisController.h"
#include "../../base/ThreadUtils.h"
#include "Worker.h"
#include "WorkerClient.h"

#include <grpc++/channel.h>
#include <grpc++/client_context.h>
#include <grpc++/create_channel.h>
#include <grpc++/security/credentials.h>
#include <grpc/grpc.h>

using namespace base;

namespace elasticjob
{
    namespace
    {
        void InitContext(grpc::ClientContext &context)
        {
            auto deadline = std::chrono::system_clock::now() + std::chrono::seconds(15);
            context.set_deadline(deadline);
        }
    } // namespace

    LocalWorkerClient::LocalWorkerClient(Worker *worker)
        : worker_(worker)
    {
        assert(worker_);
        worker->SetDelegate(this);
    }

    LocalWorkerClient::~LocalWorkerClient() { worker_->SetDelegate(nullptr); }

    bool LocalWorkerClient::Start(const std::string &leader_key)
    {
        if (stub_client_thread_.joinable())
            return false;

        shutdown_ = false;

        stub_client_thread_ = std::thread([this, leader_key]() {
            SetThreadName("ej-c-stub");
            std::string host;

            auto wait = [this]() {
                Locker locked(lock_);
                cv_.wait_for(locked, std::chrono::seconds(5));
                return shutdown_;
            };

            while (!shutdown_)
            {
                auto ret = RedisController::Instance().GetKeyValue(host, leader_key);
                if (ret != RedisController::RCStatus::RCStatus_OK || host.empty())
                {
                    LOG_ERROR << "Get leader info failed";
                    if (wait())
                        break;

                    continue;
                }

                LOG_INFO << "leader is " << host;

                auto channel = grpc::CreateChannel(host, grpc::InsecureChannelCredentials());
                client_ = elasticjob_service::ElasticJobWorker::NewStub(channel);

                if (!RegisterClient())
                {
                    LOG_ERROR << "Register client failed,";

                    if (wait())
                        break;

                    continue;
                }

                if (!ListenDispatchJob())
                {
                }

                if (shutdown_)
                    break;

                DeregisterClient();
            }
        });

        return true;
    }

    void LocalWorkerClient::Stop()
    {
        worker_->SetDelegate(nullptr);

        {
            Locker locked(lock_);
            shutdown_ = true;
            if (grpc_context_)
                grpc_context_->TryCancel();
        }

        if (stub_client_thread_.joinable())
        {
            stub_client_thread_.join();
        }
    }

    void LocalWorkerClient::OnWorkerStarted() {}
    void LocalWorkerClient::OnWorkerWillStop(WorkerClient *) {}
    void LocalWorkerClient::OnWorkerStopped(WorkerClient *) {}
    void LocalWorkerClient::OnJobRunStatusChanged(WorkerClient *, const JobResult &param)
    {
        if (!client_)
        {
            LOG_ERROR << "Client is nullptr";
            return;
        }
        elasticjob_service::SendJobResultRequest request;
        request.mutable_job_results()->set_id(param.id);
        request.mutable_job_results()->set_job_id(param.job_id);
        request.mutable_job_results()->set_status(int(param.status));
        request.mutable_job_results()->set_result(param.result);
        request.mutable_job_results()->mutable_err_code()->set_code(param.err_code.ToString());
        request.mutable_job_results()->mutable_err_code()->set_message(param.err_code.GetDesc());

        grpc::ClientContext client_context;
        InitContext(client_context);
        client_context.AddMetadata("worker_id", worker_->GetWorkerID());

        elasticjob_service::SendJobResultReply reply;
        auto ret = client_->SendJobResult(&client_context, request, &reply);
        if (!ret.ok())
        {
            LOG_ERROR << "Send to JobResult(" << param.job_id << ") to leader failed";
            return;
        }

        LOG_INFO << "Send to JobResult(" << param.job_id << ") to leader succ";

        SendStat();
    }

    bool LocalWorkerClient::RegisterClient()
    {
        elasticjob_service::RegisterWorkerReqeust request;
        for (const auto &c : worker_->ability().categories)
        {
            request.mutable_ability()->add_categories(c);
        }
        request.mutable_ability()->set_max_parallel_job(worker_->ability().max_parallel_job);

        elasticjob_service::RegisterWorkerReply reply;
        grpc::ClientContext client_context;
        InitContext(client_context);
        client_context.AddMetadata("worker_id", worker_->GetWorkerID());
        auto ret = client_->RegisterWorker(&client_context, request, &reply);
        if (!ret.ok())
        {
            LOG_ERROR << "Register client failed. code=" << ret.error_code()
                      << ", msg=" << ret.error_message();
            return false;
        }
        if (reply.has_err_code())
        {
            LOG_ERROR << "Register client failed, err=" << reply.err_code().code()
                      << ", msg=" << reply.err_code().message();
            return false;
        }
        return true;
    }

    bool LocalWorkerClient::DeregisterClient()
    {
        grpc::ClientContext client_context;
        InitContext(client_context);
        client_context.AddMetadata("worker_id", worker_->GetWorkerID());

        elasticjob_service::DeregisterWorkerReqeust req;
        elasticjob_service::DeregisterWorkerReply reply;

        auto ret = client_->DeregisterWorker(&client_context, req, &reply);
        if (!ret.ok())
        {
            LOG_ERROR << "Register client failed. code=" << ret.error_code()
                      << ", msg=" << ret.error_message();
            return false;
        }

        return true;
    }

    bool LocalWorkerClient::SendStat()
    {
        assert(client_);

        grpc::ClientContext client_context;
        InitContext(client_context);
        client_context.AddMetadata("worker_id", worker_->GetWorkerID());

        elasticjob_service::SendStatsReqeust req;
        elasticjob_service::SendStatsReply reply;

        WorkerStatistics stat;
        worker_->GetStatistical(stat);
        req.set_runing_jobs(stat.running_job);

        auto ret = client_->SendStats(&client_context, req, &reply);
        if (!ret.ok())
        {
            LOG_ERROR << "Send Stats failed. code=" << ret.error_code()
                      << ", msg=" << ret.error_message();
            return false;
        }
        return true;
    }

    bool LocalWorkerClient::ListenDispatchJob()
    {
        {
            Locker locked(lock_);
            // 判断是否已经在退出流程了，避免死等
            if (shutdown_)
                return false;

            grpc_context_.reset(new grpc::ClientContext());
        }

        grpc::ClientContext &client_context = *grpc_context_;
        client_context.AddMetadata("worker_id", worker_->GetWorkerID());

        ::elasticjob_service::ExchangeJobRequest request;

        std::unique_ptr<::grpc::ClientReader<::elasticjob_service::ExchangeJobReply>> reader =
            client_->ExchangeJob(&client_context, request);

        ::elasticjob_service::ExchangeJobReply reply;
        while (reader->Read(&reply))
        {
            if (reply.has_running_info())
            {
                JobRunParam run_param;
                run_param.category = reply.running_info().category();
                run_param.id = reply.running_info().id();
                run_param.job_id = reply.running_info().job_id();
                run_param.param = reply.running_info().param();
                worker_->AddJob(run_param);
            }
        }

        LOG_INFO << "Read finished";

        {
            Locker locked(lock_);
            grpc_context_.reset();
        }

        return true;
    }

} // namespace elasticjob
