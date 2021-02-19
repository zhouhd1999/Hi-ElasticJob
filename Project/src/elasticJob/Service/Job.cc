#include "Job.h"

#include "../../base/TimerCounter.h"
#include "JobRunner.h"

#include "muduo/base/Logging.h"

namespace elasticjob
{
    Job::Job(const JobRunParam &param, std::unique_ptr<JobRunner> runner, const std::string &worker_id)
        : runner_(std::move(runner)), param_(param)
    {
        runner_->job_ = this;

        result_.id = param_.id;
        result_.job_id = param_.job_id;
        result_.status = JobResult::Status::Pending;
        result_.worker_id = worker_id;
    }

    Job::~Job() {}

    void Job::Run()
    {
        result_.status = JobResult::Status::Doing;
        delegate()->OnJobStarted(this);

        base::TimeCounter counter;
        runner_->DoJob(result_.err_code, param_.param);

        LOG_ERROR << "Job" << param_.job_id << " using " << counter.GetCount() << "s";

        result_.status = result_.err_code.IsOk() ? JobResult::Status::Succ : JobResult::Status::Failed;

        delegate()->OnJobFinished(this);
    }

    void Job::SetDelegate(Delegate *delegate)
    {
        delegate_ = delegate;
    }

} // namespace elasticjob
