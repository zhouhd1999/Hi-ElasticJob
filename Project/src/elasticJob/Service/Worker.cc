#include "Worker.h"
#include "Job.h"
#include "JobRunner.h"

#include "muduo/base/Logging.h"

#include "../../base/Timer.h"
#include "../../base/GuidUtil.h"
#include "../../base/Task.h"
#include "../../base/MessageLoopByThreadPool.h"

#include <assert.h>

using Lock = std::unique_lock<std::recursive_mutex>;
using namespace base;

namespace elasticjob
{
    namespace
    {
        class JobTask : public Task
        {
        public:
            JobTask(const std::shared_ptr<Job> &job)
                : job_(job)
            {
            }
            ~JobTask() override {}

            void Exec() override { job_->Run(); }

        private:
            std::shared_ptr<Job> job_;
        };
    } // namespace

    Worker::Worker(const Configuration &config)
        : config_(config)
    {
        worker_id_ = config_.cluster_name + ":" + CreateGuid() + ":" + config.host_ip;

        stat_.categories = config_.ability.categories;
        stat_.max_job = config_.ability.max_parallel_job;
    }

    Worker::~Worker() { assert(!work_pool_); }

    bool Worker::Start()
    {
        assert(!work_pool_);
        if (work_pool_)
            return true;

        // 创建一个线程池
        work_pool_.reset(new base::MessageLoopDyThreadPool());
        bool ret = work_pool_->Start(config_.work_pool_min_thread, config_.work_pool_max_thread, 1, "ej-pool");
        if (!ret)
        {
            LOG_ERROR << "Start worker thread pool failed";
            return false;
        }

        // 启动定时器，定时获取worker中job的结果
        timer_.reset(new base::Timer(std::bind(&Worker::SendReport, this), "ej-worker-timer"));
        timer_->start(4000, true);

        return true;
    }

    bool Worker::Stop()
    {
        timer_->stop();

        if (!work_pool_)
            return true;

        // 通知leader这个worker即将结束，防止leader分配新任务过来
        if (delegate_)
            delegate_->OnWorkerStopped(this);

        // 需要等待全部任务结束
        work_pool_->Stop();
        work_pool_.reset();

        return true;
    }

    std::string Worker::GetWorkerID() const { return worker_id_; }

    void Worker::SetDelegate(WorkerClient::Delegate *delegate)
    {
        Lock locked(lock_);
        delegate_ = delegate;

        if (delegate_ != nullptr)
        {
            delegate_->OnWorkerStarted();
        }
    }

    bool Worker::AddJob(const JobRunParam &param)
    {
        assert(work_pool_);
        assert(config_.job_factory);

        Lock locked(lock_);
        auto iter = jobs_.find(param.id);
        if (iter != jobs_.end())
        {
            LOG_WARN << "JobRun(" << param.id << ") is already exist";

            // TODO(cgm): report a job's status;
            return true;
        }

        if (config_.job_factory == nullptr)
        {
            LOG_ERROR << "job_factory param is nullptr";
            return false;
        }

        auto job_runner = config_.job_factory->Create(param);

        if (!job_runner)
        {
            LOG_ERROR << "Create job for " << param.job_id << " failed";
            return false;
        }

        LOG_INFO << param.job_id << " start to running";

        auto job = std::make_shared<Job>(param, std::move(job_runner), worker_id_);
        job->SetDelegate(this);
        jobs_.insert(std::make_pair(param.id, job));
        stat_.running_job += 1;

        work_pool_->PostTask(new JobTask(job));
        return true;
    }

    bool Worker::GetStatistical(WorkerStatistics &st)
    {
        Lock locked(lock_);
        st = stat_;
        return true;
    }

    void Worker::OnJobStarted(Job *job)
    {
        LOG_INFO << "job(" << job->param().job_id << ") is started to run";
        Lock locked(lock_);
        if (delegate_)
        {
            delegate_->OnJobRunStatusChanged(this, job->result());
        }
    }

    void Worker::OnJobFinished(Job *job)
    {
        LOG_INFO << "job(" << job->param().job_id << ") is finished";

        Lock locked(lock_);

        auto iter = jobs_.find(job->param().id);
        assert(iter != jobs_.end());
        if (iter == jobs_.end())
        {
            LOG_ERROR << "job(" << job->param().job_id << ") can not found in map";
            return;
        }

        if (delegate_)
        {
            delegate_->OnJobRunStatusChanged(this, job->result());
        }

        jobs_.erase(iter);

        stat_.running_job = jobs_.size();
    }

    void Worker::SendReport()
    {
        Lock locked(lock_);
        if (jobs_.empty() || !delegate_)
            return;

        for (const auto &iter : jobs_)
        {
            delegate_->OnJobRunStatusChanged(this, iter.second->result());
        }
    }
} // namespace elasticjob
