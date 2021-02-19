#include "./ElasticJobClientImpl.h"

#include "../Base/elastic_job.pb.h"
#include "../../base/ThreadUtils.h"
#include "../../base/RedisController.h"
#include "muduo/base/Logging.h"

#include <grpcpp/grpcpp.h>

using RedisController = base::RedisController;
using namespace elasticjob::proto;

namespace elasticjob
{
    namespace
    {
        std::string GetLeaderKey(const std::string &cluster_name)
        {
            return "convergence-" + cluster_name + "-elasticjob-leader";
        }

        void InitContext(grpc::ClientContext &context, std::chrono::milliseconds timeout = std::chrono::seconds(15))
        {
            auto deadline = std::chrono::system_clock::now() + timeout;
            context.set_deadline(deadline);
        }

    } // namespace

    std::unique_ptr<ElasticJobClient> ElasticJobClient::Create(const std::string &cluster_name)
    {
        return std::unique_ptr<ElasticJobClient>(new ElasticJobClientImpl(cluster_name));
    }

    ElasticJobClientImpl::ElasticJobClientImpl(const std::string &cluster_name) : cluster_name_(cluster_name) {}

    ElasticJobClientImpl::~ElasticJobClientImpl() {}

    std::unique_ptr<ElasticJobService::Stub> ElasticJobClientImpl::GetStub(bool reinit /*= false*/)
    {
        std::lock_guard<std::mutex> locked(std::mutex);

        bool need_create = false;
        if (reinit || !channel_)
            need_create = true;

        if (!need_create && channel_)
        {
            auto status = channel_->GetState(true);
            if (status == grpc_connectivity_state::GRPC_CHANNEL_SHUTDOWN)
                need_create = true;
            else if (status == grpc_connectivity_state::GRPC_CHANNEL_CONNECTING)
            {
                auto deadline = std::chrono::system_clock::now() + std::chrono::seconds(3);
                if (!channel_->WaitForConnected(deadline))
                    need_create = true;
            }
        }

        // Ping下服务，检测是否能够使用
        if (!need_create)
        {
            need_create = !Ping();
        }

        if (need_create)
        {
            std::string leader_key = GetLeaderKey(cluster_name_);
            std::string leader_host;
            auto ret = RedisController::Instance().GetKeyValue(leader_host, leader_key);
            if (ret != RedisController::RCStatus::RCStatus_OK)
            {
                LOG_ERROR << "Get leader host from redis failed, key=" << leader_key;
                return std::move(std::unique_ptr<ElasticJobService::Stub>());
            }

            channel_ = grpc::CreateChannel(leader_host, grpc::InsecureChannelCredentials());
        }

        return std::move(ElasticJobService::NewStub(channel_));
    }

    bool ElasticJobClientImpl::Ping()
    {
        if (!channel_)
            return false;

        grpc::ClientContext context;
        InitContext(context, std::chrono::seconds(1));
        PingRequest request;
        PongResponse response;

        auto stub = ElasticJobService::NewStub(channel_);
        auto status = stub->Ping(&context, request, &response);

        return status.ok();
    }

    bool ElasticJobClientImpl::AddJobInfo(JobInfo &job_info, std::string *err /*= nullptr*/)
    {
        int retry = 0;

        do
        {
            auto stub = GetStub(retry > 0);
            if (!stub)
                return false;

            AddJobRequest request;
            request.mutable_job_info()->set_name(job_info.name);
            request.mutable_job_info()->set_category(job_info.category);
            request.mutable_job_info()->set_priority(job_info.priority);
            request.mutable_job_info()->set_shedule_type(static_cast<int>(job_info.shedule_type));
            request.mutable_job_info()->set_shedule_param(job_info.shedule_param);
            request.mutable_job_info()->set_param(job_info.param);
            request.mutable_job_info()->set_parallel_strategy(job_info.parallel_strategy);
            request.mutable_job_info()->set_retry_strategy(job_info.retry_strategy);

            AddJobReply reply;
            grpc::ClientContext context;
            InitContext(context);
            auto status = stub->AddJob(&context, request, &reply);
            if (status.ok())
            {
                if (!reply.has_err_code())
                {
                    job_info.id = reply.id();
                    return true;
                }
                else
                {
                    if (err != nullptr)
                        *err = reply.err_code().code();

                    LOG_ERROR << "Add Job info failed: " << reply.err_code().code()
                              << "," << reply.err_code().message();
                    return false;
                }
            }
            else
            {
                LOG_ERROR << "connection failed: " << status.error_code() << ","
                          << status.error_message();
                retry++;
            }
        } while (retry < 2);

        return false;
    }

    bool ElasticJobClientImpl::DeleteJobInfo(const std::string &job_id, std::string *err /*= nullptr*/)
    {
        assert(!job_id.empty());
        if (job_id.empty())
            return false;

        return DeleteJobInfoHelper(job_id, std::string(), err);
    }

    bool ElasticJobClientImpl::DeleteJobInfoByName(const std::string &name, std::string *err /*= nullptr*/)
    {
        assert(!name.empty());
        if (name.empty())
            return false;

        return DeleteJobInfoHelper(std::string(), name, err);
    }

    bool ElasticJobClientImpl::GetJobResult(
        JobResult &job_result, const std::string &job_id, std::string *err /*= nullptr*/)
    {
        int retry = 0;
        do
        {
            auto stub = GetStub(retry > 0);
            if (!stub)
                return false;

            GetJobResultRequest request;
            request.set_job_id(job_id);

            GetJobResultReply reply;
            grpc::ClientContext context;
            InitContext(context);
            auto status = stub->GetJobResult(&context, request, &reply);
            if (status.ok())
            {
                if (!reply.has_err_code())
                {
                    if (reply.has_result())
                    {
                        job_result.id = reply.result().id();
                        job_result.job_id = reply.result().job_id();
                        job_result.status = static_cast<JobResult::Status>(reply.result().status());
                        job_result.result = reply.result().result();
                        if (reply.result().has_error_code())
                        {
                            // job_result.err_code = ErrorCode::FromString(reply.result().error_code().code());
                            // job_result.err_code.SetDesc(reply.result().error_code().message());
                        }
                        job_result.start_time = reply.result().start_time();
                        job_result.finished_time = reply.result().finished_time();
                        job_result.failed_count = reply.result().failed_count();
                    }
                    return true;
                }
                else
                {
                    if (err != nullptr)
                        *err = reply.err_code().code();

                    LOG_ERROR << "Delete Job info failed, job(id=" << job_id << ")"
                              << ", err=" << reply.err_code().code() << "," << reply.err_code().message();
                    return false;
                }
            }
            else
            {
                LOG_ERROR << "Delete Job info failed, job(id=" << job_id << ")"
                          << ", err=" << status.error_code() << "," << status.error_message();
                retry++;
            }
        } while (retry < 2);

        return false;
    }

    bool ElasticJobClientImpl::DeleteJobInfoHelper(
        const std::string &job_id, const std::string &name, std::string *err /*= nullptr*/)
    {
        int retry = 0;

        do
        {
            auto stub = GetStub(retry > 0);
            if (!stub)
                return false;

            DeleteJobRequest request;
            request.set_id(job_id);
            request.set_name(name);

            DeleteJobReply reply;

            grpc::ClientContext context;
            InitContext(context);
            auto status = stub->DeleteJob(&context, request, &reply);
            if (status.ok())
            {
                if (!reply.has_err_code())
                {
                    return true;
                }
                else
                {
                    if (err != nullptr)
                        *err = reply.err_code().code();

                    LOG_ERROR << "Delete Job info failed, job(id=" << job_id << ")"
                              << ", err=" << reply.err_code().code() << "," << reply.err_code().message();
                    return false;
                }
            }
            else
            {
                LOG_ERROR << "Delete Job info failed, job(id=" << job_id << ")"
                          << ", err=" << status.error_code() << "," << status.error_message();
                retry++;
            }
        } while (retry < 2);

        return false;
    }
} // namespace elasticjob
