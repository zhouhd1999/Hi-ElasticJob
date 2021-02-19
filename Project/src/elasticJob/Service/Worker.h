#ifndef __WORKER_H__
#define __WORKER_H__

#include <map>
#include <string>
#include <mutex>

#include "../Base/ElasticJobTypeDefs.h"
#include "../../base/Timer.h"
#include "Job.h"
#include "WorkerClient.h"

namespace base
{
    class MessageLoopDyThreadPool;
};

class Timer;
namespace elasticjob
{
    class Worker;
    struct JobRunParam;

    class Worker : public WorkerClient, public Job::Delegate
    {
    public:
        using JobMap = std::map<std::string, std::shared_ptr<Job>>;

        Worker(const Configuration &config);
        ~Worker();

        bool Start();
        bool Stop();

        void SetDelegate(WorkerClient::Delegate *delegate);

        // WorkClient implementation
        std::string GetWorkerID() const;

        bool AddJob(const JobRunParam &param) override;
        bool GetStatistical(WorkerStatistics &st) override;

        // Job::Delegate implementation
        void OnJobStarted(Job *job) override;
        void OnJobFinished(Job *job) override;

        const WorkerAbility &ability() const { return config_.ability; }

    private:
        void SendReport();

    private:
        std::string worker_id_;
        Configuration config_;

        std::recursive_mutex lock_;
        WorkerClient::Delegate *delegate_ = nullptr;
        std::unique_ptr<base::MessageLoopDyThreadPool> work_pool_;

        JobMap jobs_;

        WorkerStatistics stat_;
        std::unique_ptr<base::Timer> timer_;
    };
} // namespace elasticjob
#endif
