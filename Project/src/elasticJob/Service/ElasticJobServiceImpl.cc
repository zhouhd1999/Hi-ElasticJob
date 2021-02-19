#include "ElasticJobServiceImpl.h"

#include "ElasticLeader.h"
#include "Proto/elasticjob_worker_service.pb.h"
#include "LocalWorkClient.h"
#include "ServiceErrors.h"
#include "Worker.h"
#include "WorkerClientProxy.h"

#include "../Base/Errors.h"

#include "../../base/StringUtils.h"
#include "../../base/RedisController.h"
#include "../../base/Timer.h"

#include "muduo/base/Logging.h"

#include <grpc++/channel.h>
#include <grpc++/client_context.h>
#include <grpc++/create_channel.h>
#include <grpc++/security/credentials.h>
#include <grpc++/server_builder.h>
#include <grpc/grpc.h>

using namespace base;

namespace elasticjob
{
    namespace
    {
        /**
             * @brief 获取集群前缀
             *
             * @param cluster_name 调度系统集群名称
             * @return std::string 调度系统使用的KEY前缀
             */
        std::string GetLeaderKey(const std::string &cluster_name)
        {
            return "convergence-" + cluster_name + "-elasticjob-leader";
        }

        std::string GetLeaderClusterLockerKey(const std::string &cluster_name)
        {
            return "convergence-" + cluster_name + "-elasticjob-leaderlock";
        }
    } // namespace

    namespace internal
    {
        // 对外接口，由client调用
        class ClientServiceAPIImpl : public proto::ElasticJobService::Service
        {
        public:
            ClientServiceAPIImpl(ElasticJobServiceImpl *service)
                : service_(service)
            {
            }

            // proto
            ::grpc::Status Ping(::grpc::ServerContext *context,
                                const elasticjob::proto::PingRequest *request,
                                elasticjob::proto::PongResponse *response) override;
            ::grpc::Status AddJob(::grpc::ServerContext *context,
                                  const elasticjob::proto::AddJobRequest *request,
                                  elasticjob::proto::AddJobReply *response) override;
            ::grpc::Status DeleteJob(::grpc::ServerContext *context,
                                     const elasticjob::proto::DeleteJobRequest *request,
                                     elasticjob::proto::DeleteJobReply *response) override;
            ::grpc::Status GetJobResult(::grpc::ServerContext *context,
                                        const elasticjob::proto::GetJobResultRequest *request,
                                        elasticjob::proto::GetJobResultReply *response) override;
        private:
            ElasticJobServiceImpl *service_;
        };

        // 若未获取到集群锁，客户端调用该Service失败
        ::grpc::Status ClientServiceAPIImpl::Ping(::grpc::ServerContext *context,
                                                  const elasticjob::proto::PingRequest *request,
                                                  elasticjob::proto::PongResponse *response)
        {
            if (!service_->has_fetch_locker_)
            {
                return grpc::Status(grpc::StatusCode::UNAVAILABLE, "not leader");
            }

            return grpc::Status::OK;
        }

        ::grpc::Status ClientServiceAPIImpl::AddJob(::grpc::ServerContext *context,
                                                    const elasticjob::proto::AddJobRequest *request,
                                                    elasticjob::proto::AddJobReply *response)
        {
            if (!service_->has_fetch_locker_)
            {
                response->mutable_err_code()->set_code(ClientErrors::kIAmNotLeader);
                return grpc::Status::OK;
            }

            if (!request->has_job_info() || request->job_info().category().empty())
            {
                response->mutable_err_code()->set_code(ClientErrors::kRequestParamError);
                return grpc::Status::OK;
            }

            JobInfo job_info;
            job_info.name = request->job_info().name();
            job_info.category = request->job_info().category();
            job_info.priority = request->job_info().priority();
            job_info.param = request->job_info().param();
            job_info.parallel_strategy = request->job_info().parallel_strategy();
            job_info.retry_strategy = request->job_info().retry_strategy();
            job_info.shedule_type = static_cast<JobInfo::SheduleType>(request->job_info().shedule_type());
            job_info.shedule_param = request->job_info().shedule_param();

            CReadLock rlocker(service_->rwlock_);
            auto ret = service_->leader_->AddJob(job_info);
            if (!ret.empty())
                response->mutable_err_code()->set_code(ret);
            else
                response->set_id(job_info.id);

            return grpc::Status::OK;
        }

        ::grpc::Status ClientServiceAPIImpl::DeleteJob(::grpc::ServerContext *context,
                                                       const elasticjob::proto::DeleteJobRequest *request,
                                                       elasticjob::proto::DeleteJobReply *response)
        {
            if (!service_->has_fetch_locker_)
            {
                response->mutable_err_code()->set_code(ClientErrors::kIAmNotLeader);
                return grpc::Status::OK;
            }

            if (request->id().empty() && request->name().empty())
            {
                response->mutable_err_code()->set_code(ClientErrors::kRequestParamError);
                return grpc::Status::OK;
            }

            CReadLock rlocked(service_->rwlock_);
            if (!request->id().empty())
                service_->leader_->RemoveJob(request->id());
            else if (!request->name().empty())
                service_->leader_->RemoveJobByName(request->name());
            return grpc::Status::OK;
        }

        ::grpc::Status ClientServiceAPIImpl::GetJobResult(::grpc::ServerContext *context,
                                                          const elasticjob::proto::GetJobResultRequest *request,
                                                          elasticjob::proto::GetJobResultReply *response)
        {
            if (!service_->has_fetch_locker_)
            {
                response->mutable_err_code()->set_code(ClientErrors::kIAmNotLeader);
                return grpc::Status::OK;
            }

            if (request->job_id().empty())
            {
                response->mutable_err_code()->set_code(ClientErrors::kRequestParamError);
                return grpc::Status::OK;
            }

            CReadLock locked(service_->rwlock_);
            JobResult result;
            auto ret = service_->leader_->GetResult(result, request->job_id());
            if (!ret.empty())
            {
                response->mutable_err_code()->set_code(ret);
                return grpc::Status::OK;
            }

            response->mutable_result()->set_id(result.id);
            response->mutable_result()->set_job_id(result.job_id);
            response->mutable_result()->set_result(result.result);
            response->mutable_result()->set_failed_count(result.failed_count);
            response->mutable_result()->set_start_time(result.start_time);
            response->mutable_result()->set_finished_time(result.finished_time);
            response->mutable_result()->set_status(static_cast<int>(result.status));
            response->mutable_result()->mutable_error_code()->set_code(result.err_code.ToString());
            response->mutable_result()->mutable_error_code()->set_message(result.err_code.GetDesc());

            return grpc::Status::OK;
        }
    } // namespace internal

    std::unique_ptr<ElasticJobService> ElasticJobService::Create(const Configuration &config)
    {
        return std::unique_ptr<ElasticJobService>(new ElasticJobServiceImpl(config));
    }

    ElasticJobServiceImpl::ElasticJobServiceImpl(const Configuration &config)
        : config_(config)
    {
        client_service_impl_.reset(new internal::ClientServiceAPIImpl(this));
    }

    bool ElasticJobServiceImpl::Start()
    {
        if (timer_)
            return true;

        // 服务启动时立刻创建一个worker并启动
        worker_.reset(new Worker(config_));
        worker_->Start();

        OnClusterLockChanged();

        // 启动一个定时器，定时获取集群锁，若获取到集群锁则为leader模式，未获取到为LocalClient模式
        timer_.reset(new base::Timer(std::bind(&ElasticJobServiceImpl::FetchClusterLock, this), "ej-cluster-locker"));
        timer_->start(2500, true);

        return true;
    }

    bool ElasticJobServiceImpl::Stop()
    {
        LOG_INFO << "Stop Elastic job service";

        timer_->stop();

        worker_->Stop();

        if (local_client_)
            local_client_->Stop();

        CWriteLock wlocked(rwlock_);
        if (leader_)
        {
            // 将锁释放后再stop
            std::string lock_name = GetLeaderClusterLockerKey(config_.cluster_name);
            auto ret = RedisController::Instance().RedisUnlock(lock_name);
            if (ret != RedisController::RCStatus::RCStatus_OK)
            {
                LOG_ERROR << "Release cluster lock failed when stop";
            }
            leader_->Stop();
        }
        return true;
    }

    // 获取集群锁
    bool ElasticJobServiceImpl::FetchClusterLock()
    {
        std::string lock_name = GetLeaderClusterLockerKey(config_.cluster_name);
        const int kExpiredMs = 10 * 1000;

        // if (has_fetch_locker_)
        // {
        //     RemoveLostConnectClients();
        // }

        if (has_fetch_locker_)
        {
            auto ret = RedisController::Instance().ExpireLock(lock_name, kExpiredMs);   //tag
            if (ret != RedisController::RCStatus::RCStatus_OK)
            {
                LOG_WARN << "lost cluster lock, try to relock";

                ret = RedisController::Instance().RedisLock(lock_name, kExpiredMs);
                if (ret != RedisController::RCStatus::RCStatus_OK)
                {
                    LOG_WARN << "lost cluster lock, really";

                    has_fetch_locker_ = false;
                    OnClusterLockChanged();

                    return true;
                }

                LOG_INFO << "Get locker again";
            }
        }
        else
        {
            auto ret = RedisController::Instance().RedisLock(lock_name, kExpiredMs);
            if (ret != RedisController::RCStatus::RCStatus_OK)
            {
                return true;
            }

            has_fetch_locker_ = true;
            OnClusterLockChanged();
        }

        return true;
    }

    void ElasticJobServiceImpl::OnClusterLockChanged()
    {
        if (has_fetch_locker_)
        {
            StopClient();
            StartLeader();
        }
        else
        {
            StopLeader();
            StartClient();
        }
    }

    bool ElasticJobServiceImpl::StartLeader()
    {
        LOG_INFO << "Start as leader";
        std::string leader_key = GetLeaderKey(config_.cluster_name);

        std::ostringstream os;
        os << config_.host_ip << ":" << config_.host_port;
        std::string host = os.str();

        LOG_INFO << "Leader is host is " << host;

        // 设置Redis 集群名称  ip:port
        RedisController::Instance().SetKeyValue(leader_key, host);

        {
            CWriteLock wlocked(rwlock_);
            leader_.reset(new ElasticLeader(config_));
            // 启动leader
            leader_->Start();
        }
        // 给leader绑定worker
        leader_->AddWorker(worker_.get());
        // 给worker设置代理
        worker_->SetDelegate(leader_.get());

        grpc::ServerBuilder builder;
        std::string listen_port = strfmt("0.0.0.0:%d", config_.host_port);
        LOG_INFO << "ElasticJob listen to " << listen_port;
        builder.AddListeningPort(listen_port, grpc::InsecureServerCredentials());
        builder.RegisterService(this);
        builder.RegisterService(client_service_impl_.get());

        server_ = builder.BuildAndStart();

        return true;
    }

    bool ElasticJobServiceImpl::StopLeader()
    {
        LOG_INFO << "Stop as Leader";

        CWriteLock wlocked(rwlock_);

        // 关闭服务
        if (server_)
        {
            server_->Shutdown();
        }

        // 移除Leader上的Worker
        if (leader_)
        {
            leader_->RemoveWorker(worker_.get());
            leader_->Stop();
        }

        return true;
    }

    // void ElasticJobServiceImpl::RemoveLostConnectClients()
    // {
    //     // 找到所有失去链接的worker
    //     std::vector<std::shared_ptr<WorkerClientProxy>> lost_connect_workers;
    //     {
    //         std::unique_lock<std::mutex> locked(worker_proxy_lock_);

    //         for (auto iter = worker_proxys_.begin(); iter != worker_proxys_.end();)
    //         {
    //             if (iter->second->IsLostConnected())
    //             {
    //                 lost_connect_workers.push_back(iter->second);
    //                 iter = worker_proxys_.erase(iter);
    //             }
    //             else
    //             {
    //                 iter++;
    //             }
    //         }
    //     }

    //     // post to a background thread
    //     if (!lost_connect_workers.empty())
    //     {
    //         for (auto worker : lost_connect_workers)
    //         {
    //             LOG_INFO << "worker(" << worker->GetWorkerID() << " has lost connection";
    //             {
    //                 CWriteLock wlocked(rwlock_);
    //                 leader_->RemoveWorker(worker.get());
    //             }
    //             worker->SetDelegate(nullptr);
    //             worker->Stop();
    //         }
    //     }
    // }

    std::shared_ptr<WorkerClientProxy> ElasticJobServiceImpl::FindProxyByID(const std::string &id)
    {
        std::unique_lock<std::mutex> locked(worker_proxy_lock_);
        auto iter = worker_proxys_.find(id);

        if (iter == worker_proxys_.end())
        {
            return std::shared_ptr<WorkerClientProxy>();
        }

        return iter->second;
    }

    ::grpc::Status ElasticJobServiceImpl::RegisterWorker(::grpc::ServerContext *context,
                                                         const ::elasticjob_service::RegisterWorkerReqeust *request,
                                                         ::elasticjob_service::RegisterWorkerReply *response)
    {
        if (!has_fetch_locker_)
        {
            response->mutable_err_code()->set_code(ServiceErrors::kIAmNotLeader);
            return grpc::Status::OK;
        }

        auto iter = context->client_metadata().find("worker_id");
        if (iter == context->client_metadata().end())
        {
            LOG_ERROR << "no worker id in client meta";
            response->mutable_err_code()->set_code("err");
            response->mutable_err_code()->set_message("no worker id in client meta");
            return grpc::Status::OK;
        }
        std::string worker_id = std::string(iter->second.data(), iter->second.size());
        WorkerAbility ability;
        if (!request->has_ability())
        {
            LOG_ERROR << "no ability in client meta";
            response->mutable_err_code()->set_code("err");
            response->mutable_err_code()->set_message("ability in client meta");
            return grpc::Status::OK;
        }

        ability.max_parallel_job = request->ability().max_parallel_job();
        for (const auto &c : request->ability().categories())
        {
            ability.categories.insert(c);
        }

        auto worker = FindProxyByID(worker_id);
        if (worker)
        {
            LOG_ERROR << "Worker(id) is already registered";
            return grpc::Status::OK;
        }

        auto new_worker = std::make_shared<WorkerClientProxy>(worker_id, ability);
        new_worker->Start();

        {
            std::unique_lock<std::mutex> locked(worker_proxy_lock_);
            worker_proxys_.insert(std::make_pair(worker_id, new_worker));
        }

        {
            CWriteLock wlocked(rwlock_);
            leader_->AddWorker(new_worker.get());
            new_worker->SetDelegate(leader_.get());
        }
        return grpc::Status::OK;
    }

    ::grpc::Status ElasticJobServiceImpl::DeregisterWorker(::grpc::ServerContext *context,
                                                           const ::elasticjob_service::DeregisterWorkerReqeust *request,
                                                           ::elasticjob_service::DeregisterWorkerReply *response)
    {
        if (!has_fetch_locker_)
        {
            response->mutable_err_code()->set_code(ServiceErrors::kIAmNotLeader);
            return grpc::Status::OK;
        }

        auto iter = context->client_metadata().find("worker_id");
        if (iter == context->client_metadata().end())
        {
            LOG_ERROR << "no worker id in client meta";
            response->mutable_err_code()->set_code(ServiceErrors::kRequestParamError);
            response->mutable_err_code()->set_message("no worker id in client meta");
            return grpc::Status::OK;
        }

        std::string worker_id = std::string(iter->second.data(), iter->second.size());

        std::shared_ptr<WorkerClientProxy> worker;
        {
            std::unique_lock<std::mutex> locked(worker_proxy_lock_);
            auto worker_iter = worker_proxys_.find(worker_id);

            if (worker_iter == worker_proxys_.end())
            {
                LOG_ERROR << "There is no worker proxy exist, id=" << worker_id;
                LOG_ERROR << "no worker id in client meta";
                response->mutable_err_code()->set_code(ServiceErrors::kWorkerIDIsNotExist);
                response->mutable_err_code()->set_message("no worker id in client meta");
                return grpc::Status::OK;
            }

            worker = worker_iter->second;
            worker_proxys_.erase(worker_iter);
        }

        // post to a background thread
        if (worker)
        {
            {
                CWriteLock wlocked(rwlock_);
                leader_->RemoveWorker(worker.get());
            }
            worker->SetDelegate(nullptr);
            worker->Stop();
        }

        return grpc::Status::OK;
    }

    ::grpc::Status ElasticJobServiceImpl::SendRunningJobs(::grpc::ServerContext *context,
                                                          const ::elasticjob_service::SendRunningJobsRequest *request,
                                                          ::elasticjob_service::SendRunningJobsReply *response)
    {
        if (!has_fetch_locker_)
        {
            response->mutable_err_code()->set_code(ServiceErrors::kIAmNotLeader);
            return grpc::Status::OK;
        }

        auto iter = context->client_metadata().find("worker_id");
        if (iter == context->client_metadata().end())
        {
            LOG_ERROR << "no worker id in client meta";
            response->mutable_err_code()->set_code(ServiceErrors::kRequestParamError);
            response->mutable_err_code()->set_message("no worker id in client meta");
            return grpc::Status::OK;
        }

        std::string worker_id = std::string(iter->second.data(), iter->second.size());
        auto worker = FindProxyByID(worker_id);
        if (!worker)
        {
            LOG_ERROR << "Worker(id) is already registered";
            return grpc::Status::OK;
        }

        return grpc::Status::CANCELLED;
    }

    ::grpc::Status ElasticJobServiceImpl::SendStats(::grpc::ServerContext *context,
                                                    const ::elasticjob_service::SendStatsReqeust *request, ::elasticjob_service::SendStatsReply *response)
    {
        if (!has_fetch_locker_)
        {
            return grpc::Status::OK;
        }

        auto iter = context->client_metadata().find("worker_id");
        if (iter == context->client_metadata().end())
        {
            LOG_ERROR << "no worker id in client meta";
            return grpc::Status::OK;
        }

        std::string worker_id = std::string(iter->second.data(), iter->second.size());
        auto worker = FindProxyByID(worker_id);
        if (!worker)
        {
            LOG_ERROR << "Worker(id) is not registered" << worker_id;
            return grpc::Status::OK;
        }

        WorkerStatistics stats;
        stats.running_job = request->runing_jobs();
        worker->UpdateStat(stats);

        return grpc::Status::OK;
    }

    ::grpc::Status ElasticJobServiceImpl::SendJobResult(::grpc::ServerContext *context,
                                                        const ::elasticjob_service::SendJobResultRequest *request,
                                                        ::elasticjob_service::SendJobResultReply *response)
    {
        if (!has_fetch_locker_)
        {
            response->mutable_err_code()->set_code(ServiceErrors::kIAmNotLeader);
            return grpc::Status::OK;
        }

        auto iter = context->client_metadata().find("worker_id");
        if (iter == context->client_metadata().end())
        {
            LOG_ERROR << "no worker id in client meta";
            response->mutable_err_code()->set_code(ServiceErrors::kRequestParamError);
            response->mutable_err_code()->set_message("no worker id in client meta");
            return grpc::Status::OK;
        }

        if (!request->has_job_results())
        {
            LOG_ERROR << "No job result params";
            response->mutable_err_code()->set_code(ServiceErrors::kRequestParamError);
            response->mutable_err_code()->set_message("no job result params");
            return grpc::Status::OK;
        }

        JobResult job_result;
        job_result.id = request->job_results().id();
        job_result.job_id = request->job_results().job_id();
        job_result.result = request->job_results().result();
        job_result.status = static_cast<JobResult::Status>(request->job_results().status());
        if (request->job_results().has_err_code() && !request->job_results().err_code().code().empty())
        {
            job_result.err_code = ErrorCode::FromString(request->job_results().err_code().code());
            job_result.err_code.SetDesc(request->job_results().err_code().message());
        }

        std::string worker_id = std::string(iter->second.data(), iter->second.size());
        auto worker = FindProxyByID(worker_id);
        if (!worker)
        {
            LOG_ERROR << "Worker(id) is not registered" << worker_id;
            return grpc::Status::OK;
        }
        job_result.worker_id = worker_id;

        worker->SendJobResult(job_result);
        return grpc::Status::OK;
    }

    ::grpc::Status ElasticJobServiceImpl::ExchangeJob(::grpc::ServerContext *context,
                                                      const ::elasticjob_service::ExchangeJobRequest *request,
                                                      ::grpc::ServerWriter<::elasticjob_service::ExchangeJobReply> *writer)
    {
        if (!has_fetch_locker_)
        {
            elasticjob_service::ExchangeJobReply reply;
            reply.mutable_err_code()->set_code(ServiceErrors::kIAmNotLeader);
            writer->Write(reply);
            return grpc::Status::OK;
        }

        auto iter = context->client_metadata().find("worker_id");
        if (iter == context->client_metadata().end())
        {
            LOG_ERROR << "no worker id in client meta";
            return grpc::Status::OK;
        }
        std::string worker_id = std::string(iter->second.data(), iter->second.size());

        auto worker = FindProxyByID(worker_id);
        if (!worker)
        {
            LOG_ERROR << "Worker(id) is not registered";
            return grpc::Status::OK;
        }

        worker->SetConnectState(WorkerClientProxy::ConnectionStatus::Connected);

        bool write_succ = true;
        while (!context->IsCancelled() && write_succ)
        {
            std::vector<JobRunParam> params;
            worker->GetDispathedJobs(params);

            if (params.empty())
            {
                // 将空的回复作为心跳包写入
                elasticjob_service::ExchangeJobReply reply;
                write_succ = writer->Write(reply);
                if (!write_succ)
                {
                    LOG_ERROR << "Write empty replay failed, to: " << worker->GetWorkerID();
                    break;
                }
                continue;
            }

            for (const auto &p : params)
            {
                elasticjob_service::ExchangeJobReply reply;
                elasticjob_service::JobRunningInfo *info = reply.mutable_running_info();
                info->set_id(p.id);
                info->set_job_id(p.job_id);
                info->set_category(p.category);
                info->set_param(p.param);
                write_succ = writer->Write(reply);
                if (!write_succ)
                {
                    LOG_ERROR << "Write exchange replay failed, to: " << worker->GetWorkerID();
                    break;
                }
            }
        }

        worker->SetConnectState(WorkerClientProxy::ConnectionStatus::Disconnected);

        return grpc::Status::OK;
    }

    ////////////////////////////////////////////////////////////////////////

    bool ElasticJobServiceImpl::StartClient()
    {
        LOG_INFO << "Start local client";

        std::string leader_key = GetLeaderKey(config_.cluster_name);

        // Worker是服务刚启动时创建的
        local_client_.reset(new LocalWorkerClient(worker_.get()));
        // 启动本地客户端
        return local_client_->Start(leader_key);
    }

    bool ElasticJobServiceImpl::StopClient()
    {
        LOG_INFO << "Stop local client";
        if (!local_client_)
            return true;

        local_client_->Stop();
        return true;
    }
} // namespace elasticjob
