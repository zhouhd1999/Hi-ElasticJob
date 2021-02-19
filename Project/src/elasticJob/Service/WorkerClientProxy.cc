#include "WorkerClientProxy.h"

namespace elasticjob
{
    WorkerClientProxy::WorkerClientProxy(const std::string &worker_id, WorkerAbility &ability)
        : worker_id_(worker_id),
          ability_(ability),
          status_(ConnectionStatus::Disconnected),
          lost_connection_time_(Clock::time_point::min())
    {
        stat_.categories = ability_.categories;
        stat_.max_job = ability_.max_parallel_job;
    }

    WorkerClientProxy::~WorkerClientProxy() {}

    bool WorkerClientProxy::Start()
    {
        lost_connection_time_ = Clock::time_point::min();
        return true;
    }

    bool WorkerClientProxy::Stop()
    {
        lost_connection_time_ = Clock::now();
        return true;
    }

    std::string WorkerClientProxy::GetWorkerID() const { return worker_id_; }

    void WorkerClientProxy::SetDelegate(WorkerClient::Delegate *delegate)
    {
        std::unique_lock<std::mutex> locked(lock_);
        delegate_ = delegate;
    }

    bool WorkerClientProxy::AddJob(const JobRunParam &param)
    {
        std::unique_lock<std::mutex> locked(lock_);
        job_run_params_.push_back(param);

        cv_.notify_all();
        return true;
    }

    bool WorkerClientProxy::GetStatistical(WorkerStatistics &st)
    {
        std::unique_lock<std::mutex> locked(lock_);
        st = stat_;
        return true;
    }

    bool WorkerClientProxy::GetDispathedJobs(std::vector<JobRunParam> &jobs)
    {
        std::unique_lock<std::mutex> locked(lock_);
        if (!job_run_params_.empty())
        {
            jobs.swap(job_run_params_);
            return true;
        }

        cv_.wait_for(locked, std::chrono::seconds(5));

        jobs.swap(job_run_params_);
        return true;
    }

    bool WorkerClientProxy::UpdateStat(WorkerStatistics &stat)
    {
        std::unique_lock<std::mutex> locked(lock_);
        stat_.running_job = stat.running_job;
        return true;
    }

    bool WorkerClientProxy::SendJobResult(const JobResult &res)
    {
        std::unique_lock<std::mutex> locked(lock_);
        if (delegate_)
            delegate_->OnJobRunStatusChanged(this, res);

        return true;
    }

    void WorkerClientProxy::SetConnectState(ConnectionStatus status)
    {
        status_ = status;
        if (status == ConnectionStatus::Connected)
        {
            lost_connection_time_ = Clock::time_point::min();
        }
        else if (status == ConnectionStatus::Disconnected)
        {
            lost_connection_time_ = Clock::now();
        }
    }

    bool WorkerClientProxy::IsLostConnected() const
    {
        if (lost_connection_time_ == Clock::time_point::min())
            return false;

        return (Clock::now() - lost_connection_time_) > std::chrono::seconds(10);
    }

} // namespace elasticjob