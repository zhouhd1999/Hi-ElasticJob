#ifndef __ELASTIC_JOB_CLIENT__
#define __ELASTIC_JOB_CLIENT__

#include <string>
#include <memory>

#include "../Base/ElasticJobTypeDefs.h"

namespace elasticjob
{
    namespace proto
    {
        namespace client
        {
            class Stub;
        } // namespace client

    } // namespace proto

    class ElasticJobClient
    {
    public:
        static std::unique_ptr<ElasticJobClient> Create(const std::string &cluster_name);
        virtual ~ElasticJobClient() {}

        virtual bool AddJobInfo(JobInfo &job_info, std::string *err = nullptr) = 0;
        virtual bool DeleteJobInfo(const std::string &job_id, std::string *err = nullptr) = 0;
        virtual bool DeleteJobInfoByName(const std::string &name, std::string *err = nullptr) = 0;
        virtual bool GetJobResult(JobResult &job_result, const std::string &job_id, std::string *err = nullptr) = 0;
    };
} // namespace elasticjob

#endif