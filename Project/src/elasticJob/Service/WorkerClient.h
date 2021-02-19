#ifndef __WORKER_CLIENT_H__
#define __WORKER_CLIENT_H__

#include "../Base/ElasticJobTypeDefs.h"

#include <set>
#include <string>

namespace elasticjob
{
    class Worker;
    struct JobResult;
    struct JobInfo;

    struct WorkerStatistics
    {
        std::set<std::string> categories;
        int max_job = 100;   // 最大处理的任务数量，默认100
        int running_job = 0; // 当前处理的任务数量
    };

    class WorkerClient
    {
    public:
        enum State
        {
            Pending = 0,
            Normal = 1,
            Exception = 2,
        };

        class Delegate
        {
        public:
            virtual ~Delegate(){};

            virtual void OnWorkerStarted() = 0;
            virtual void OnWorkerWillStop(WorkerClient *client) = 0;
            virtual void OnWorkerStopped(WorkerClient *client) = 0;
            virtual void OnJobRunStatusChanged(WorkerClient *client, const JobResult &param) = 0;
        };

        virtual std::string GetWorkerID() const = 0;

        virtual bool AddJob(const JobRunParam &run_param) = 0;
        virtual bool GetStatistical(WorkerStatistics &st) = 0;
    };
} // namespace elasticjob

#endif
