#ifndef __ELASTIC_LEADER_H__
#define __ELASTIC_LEADER_H__

#include <list>
#include <map>
#include <memory>
#include <thread>
#include <mutex>

#include "../Base/ElasticJobTypeDefs.h"
#include "./ParallelStrategy.h"
#include "./WorkerClient.h"

#include "boost/asio/executor_work_guard.hpp"
#include "boost/asio/io_context.hpp"
#include "boost/asio/system_timer.hpp"

namespace elasticjob
{
    struct Configuration;
    class Storage;
    class LocalWorkerClient;

    struct JobInfoDetail : public JobInfo
    {
        enum class Status
        {
            Waiting,
            Prepared,
            Pending,
            Running,
        };

        ptime next_shedule_time; // 记录下次任务触发的时间点
        ParallelItem parallel_item;
        Status status = Status::Waiting;

        JobInfoDetail(const JobInfo &job_info)
            : JobInfo(job_info)
        {
        }
    };

    class ElasticLeader : public WorkerClient::Delegate
    {
    public:
        using error_code = boost::system::error_code;
        using ptime = boost::posix_time::ptime;
        using WorkGurad = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;
        using Timer = boost::asio::system_timer;
        using JobMap = std::map<std::string, JobInfoDetail>;
        using JobIterator = JobMap::iterator;
        using JobList = std::list<JobMap::iterator>;
        using JobListValue = JobList::value_type;
        using JobResultMap = std::map<std::string, JobResult>;

        ElasticLeader(const Configuration &config);
        ~ElasticLeader();

        bool Start();
        bool Stop();

        void AddWorker(WorkerClient *worker);
        void RemoveWorker(WorkerClient *worker);

        std::string AddJob(JobInfo &job_info);
        void RemoveJob(const std::string &id);
        void RemoveJobByName(const std::string &name);
        std::string GetResult(JobResult &result, const std::string &id);

        void GetJobStatusByName(const std::string &name);
        void GetJobStatusByID(const std::string &id);

        // WorkClient::Delegate implementation
        void OnWorkerStarted() override;
        void OnWorkerWillStop(WorkerClient *client) override;
        void OnWorkerStopped(WorkerClient *client) override;
        void OnJobRunStatusChanged(WorkerClient *client, const JobResult &param) override;

    private:
        void AddJobToMap(const JobInfo &job);
        void RemoveJobFromMap(const JobInfo &job);

        bool AddJobToWait(JobMap::iterator iter);
        bool AddJobToPrepared(JobMap::iterator iter);
        void AddJobToRun(JobMap::iterator iter);
        void AddJobResult(JobIterator iter);

        void SwitchWaitingToPrepare(const std::string &job_id);
        void SwitchRunningToWait(const std::string &job_id);
        void SwitchRunningToPrepare(const std::string &job_id);
        void DeleteJobByID(const std::string &job_id);

        void OnTimeCheckWaitingTask(const error_code &ec);
        void NextCheckWaitingJob(const ptime &next);

        void OnTimeCheckPreparedJob(const error_code &ec);
        void NextCheckPreparedJob(const ptime &next);

        void OnTimeCheckJobRunStatus(const error_code &ec);
        void NextCheckJobRunStatus(const ptime &next);

        void OnTimeToReportStat(const error_code &ec);
        void NextToReportStat(const ptime &next);

        bool HasPendingJob() const;
        void DispatchPendingJob(const error_code &ec);
        void NextDispathPendingJob(const ptime &next);
        void CheckJobResult();

        void NextCheckJobRunHealth(const ptime &next);
        void CheckJobRunHealth(const error_code &ec);

        void NextCleanupStorage(const ptime &next);
        void CleanupStorage();

        void HandleJobAfterRunFinished(const std::string &job_id);

        void UpdateJobResultStatus(const JobResult &result);

        WorkerClient *FindAvailableWorkClient();

        void InitParallelStrategiesFromDB();

    private:
        Configuration config_;

        ParallelStrategy parallel_strategy_;

        std::unique_ptr<Storage> storage_;

        JobMap jobs_;
        JobList waiting_jobs_;  // 等待触发的任务队列
        JobList prepared_jobs_; // 已经触发的任务队列，未执行
        JobList runing_jobs_;   // 执行中
        std::map<std::string, JobResult> job_results_;  // 系统里运行，但未完成的任务

        boost::asio::io_context context_;
        Timer waiting_timer_;
        Timer check_prepare_job_timer_;
        Timer check_job_results_timer_;
        Timer check_dispath_pending_job_timer_;
        Timer check_job_run_health_timer_;
        Timer report_stat_timer_;
        Timer cleanup_db_timer_;
        std::thread work_thread_;
        std::unique_ptr<WorkGurad> work_guard_;

        std::mutex worker_lock_;
        std::map<std::string, WorkerClient *> workers_;
    };
} // namespace elasticjob

#endif
