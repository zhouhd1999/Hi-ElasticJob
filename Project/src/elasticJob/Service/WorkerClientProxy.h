#ifndef __WORKER_CLIENT_PROXY_H__
#define __WORKER_CLIENT_PROXY_H__

#include <thread>
#include <mutex>
#include <vector>
#include <condition_variable>

#include "../Base/ElasticJobTypeDefs.h"
#include "WorkerClient.h"

namespace elasticjob
{

    class Worker;

    class WorkerClientProxy : public WorkerClient
    {
    public:
        using Clock = std::chrono::steady_clock;

        WorkerClientProxy(const std::string &worker_id, WorkerAbility &ability);
        ~WorkerClientProxy();

        enum class ConnectionStatus
        {
            Connected,
            Disconnected,
        };

        void SetDelegate(WorkerClient::Delegate *delegate);

        bool Start();
        bool Stop();

        // WorkerClient implementation
        std::string GetWorkerID() const override;
        bool AddJob(const JobRunParam &param) override;
        bool GetStatistical(WorkerStatistics &st) override;

        bool GetDispathedJobs(std::vector<JobRunParam> &jobs);
        bool UpdateStat(WorkerStatistics &stat);

        bool SendJobResult(const JobResult &res);

        void SetConnectState(ConnectionStatus);
        bool IsLostConnected() const;

    private:
        std::string worker_id_;
        std::thread thread_;

        std::mutex lock_;
        WorkerClient::Delegate *delegate_;
        WorkerAbility ability_;
        WorkerStatistics stat_;

        ConnectionStatus status_;
        Clock::time_point lost_connection_time_; // 连接丢失时间

        std::condition_variable cv_;
        std::vector<JobRunParam> job_run_params_;
    };
} // namespace elasticjob
#endif
