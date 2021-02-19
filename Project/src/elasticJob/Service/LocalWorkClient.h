#ifndef __LOCAL_WORK_CLIENT_H__
#define __LOCAL_WORK_CLIENT_H__

#include <thread>
#include <mutex>
#include <condition_variable>
#include <grpc++/channel.h>

#include "../Base/ElasticJobTypeDefs.h"
#include "./Proto/elasticjob_worker_service.grpc.pb.h"
#include "WorkerClient.h"

namespace elasticjob
{

    struct WorkerAbility;
    class Worker;

    class LocalWorkerClient : public WorkerClient::Delegate
    {
    public:
        LocalWorkerClient(Worker *worker);
        ~LocalWorkerClient() override;

        bool Start(const std::string &leader_key);
        void Stop();

        void OnWorkerStarted() override;
        void OnWorkerWillStop(WorkerClient *client) override;
        void OnWorkerStopped(WorkerClient *client) override;
        void OnJobRunStatusChanged(WorkerClient *client, const JobResult &param) override;

    private:
        bool RegisterClient();
        bool DeregisterClient();
        bool SendStat();
        bool ListenDispatchJob();

    private:
        Worker *worker_;

        bool shutdown_ = false;
        std::mutex lock_;
        std::condition_variable cv_;
        using Locker = std::unique_lock<std::mutex>;

        std::thread stub_client_thread_;
        std::unique_ptr<grpc::ClientContext> grpc_context_;

        std::unique_ptr<elasticjob_service::ElasticJobWorker::Stub> client_;
    };
} // namespace elasticjob
#endif
