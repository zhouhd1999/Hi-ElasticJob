#ifndef __JOB_RUNNER_H__
#define __JOB_RUNNER_H__
#include <memory>

#include "../Base/ElasticJobTypeDefs.h"

namespace elasticjob
{
    class Job;

    class JobRunner
    {
    public:
        virtual ~JobRunner() {}

        virtual void DoJob(ErrorCode &err_code, const std::string &param) = 0;

        void SetProcess(int);

    private:
        friend class Job;
        Job *job_;
    };

    class JobFactory
    {
    public:
        virtual ~JobFactory() {}

        virtual std::unique_ptr<JobRunner> Create(const JobRunParam &param) = 0;
    };

} // namespace elasticjob

#endif
