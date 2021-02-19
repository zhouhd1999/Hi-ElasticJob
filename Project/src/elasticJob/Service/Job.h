#ifndef __JOB_H__
#define __JOB_H__

#include <memory>

#include "../Base/ElasticJobTypeDefs.h"

namespace elasticjob
{
    class Worker;
    class JobRunParam;
    class JobRunner;

    class Job : public std::enable_shared_from_this<Job>
    {
    public:
        class Delegate
        {
        public:
            virtual ~Delegate(){};

            virtual void OnJobStarted(Job *job) = 0;
            virtual void OnJobFinished(Job *job) = 0;
        };
        Job(const JobRunParam &param, std::unique_ptr<JobRunner> runner, const std::string &worker_id);
        ~Job();

        void Run();

        bool IsFinished() const;
        int GetProcess() const;

        void SetDelegate(Delegate *delegate);
        Delegate *delegate() const { return delegate_; }

        const JobRunParam &param() const { return param_; }
        const JobResult &result() const { return result_; }

        const ErrorCode &err_code() const { return err_code_; }

    private:
        int process_ = 0;
        ErrorCode err_code_;

        std::unique_ptr<JobRunner> runner_;
        Delegate *delegate_ = nullptr;

        JobRunParam param_;
        JobResult result_;
    };

} // namespace elasticjob

#endif
