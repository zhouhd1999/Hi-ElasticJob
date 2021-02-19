#ifndef __ELASTIC_JOB_SERVICE_IMPL_H__
#define __ELASTIC_JOB_SERVICE_IMPL_H__

#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "../Base/ElasticJobTypeDefs.h"
#include "../Base/elastic_job.grpc.pb.h"
#include "ElasticJobService.h"
#include "Proto/elasticjob_worker_service.grpc.pb.h"
#include "../../base/Timer.h"

#include <boost/asio/executor_work_guard.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/locks.hpp>

#include <grpc++/server.h>

class CTimer;

namespace elasticjob
{
    class ElasticLeader;
    class WorkerClientProxy;
    class Worker;
    class Job;
    class LocalWorkerClient;

    typedef boost::shared_mutex CRWMutex; 
    typedef boost::shared_lock<CRWMutex> CReadLock; 
    typedef boost::unique_lock <CRWMutex> CWriteLock; 

    namespace internal
    {
        class ClientServiceAPIImpl;
    }

    class ElasticJobServiceImpl : public ElasticJobService, public elasticjob_service::ElasticJobWorker::Service
    {
    public:
        ElasticJobServiceImpl(const Configuration &config);

        bool Start() override;
        bool Stop() override;

        // elasticjob_service::ElasticJobWorker::Service
        ::grpc::Status RegisterWorker(::grpc::ServerContext *context,
                                      const ::elasticjob_service::RegisterWorkerReqeust *request,
                                      ::elasticjob_service::RegisterWorkerReply *response) override;
        ::grpc::Status DeregisterWorker(::grpc::ServerContext *context,
                                        const ::elasticjob_service::DeregisterWorkerReqeust *request,
                                        ::elasticjob_service::DeregisterWorkerReply *response) override;
        ::grpc::Status SendStats(::grpc::ServerContext *context,
                                 const ::elasticjob_service::SendStatsReqeust *request,
                                 ::elasticjob_service::SendStatsReply *response) override;
        ::grpc::Status SendRunningJobs(::grpc::ServerContext *context,
                                       const ::elasticjob_service::SendRunningJobsRequest *request,
                                       ::elasticjob_service::SendRunningJobsReply *response) override;
        ::grpc::Status SendJobResult(::grpc::ServerContext *context,
                                     const ::elasticjob_service::SendJobResultRequest *request,
                                     ::elasticjob_service::SendJobResultReply *response) override;
        ::grpc::Status ExchangeJob(::grpc::ServerContext *context,
                                   const ::elasticjob_service::ExchangeJobRequest *request,
                                   ::grpc::ServerWriter<::elasticjob_service::ExchangeJobReply> *writer) override;

    private:
        bool FetchClusterLock();
        void OnClusterLockChanged();

        bool StartLeader();
        bool StopLeader();

        /// @brief 删除长丢失连接的客户端
        void RemoveLostConnectClients();

        bool StartClient();
        bool StopClient();

        std::shared_ptr<WorkerClientProxy> FindProxyByID(const std::string &id);

    private:
        friend class internal::ClientServiceAPIImpl;

        Configuration config_;
        std::unique_ptr<internal::ClientServiceAPIImpl> client_service_impl_;

        CRWMutex rwlock_;
        std::unique_ptr<ElasticLeader> leader_;
        std::unique_ptr<Worker> worker_;

        std::unique_ptr<base::Timer> timer_;
        bool has_fetch_locker_ = false;

        std::mutex worker_proxy_lock_;
        std::map<std::string, std::shared_ptr<WorkerClientProxy>> worker_proxys_;

        std::thread stub_client_thread_;
        std::unique_ptr<elasticjob_service::ElasticJobWorker::Stub> client_;

        std::unique_ptr<LocalWorkerClient> local_client_;

        std::unique_ptr<grpc::Server> server_;
    };

} // namespace elasticjob

#endif
