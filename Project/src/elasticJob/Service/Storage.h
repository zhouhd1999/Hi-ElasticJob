#ifndef __STORAGR_H__
#define __STORAGR_H__

#include <string>
#include <vector>
#include <chrono>

#include <boost/date_time/posix_time/ptime.hpp>

#include "../Base/ElasticJobTypeDefs.h"

namespace elasticjob
{
    class Storage
    {
    public:
        using ptime = boost::posix_time::ptime;

        Storage();
        ~Storage();

        bool GetAllJobs(std::vector<JobInfo> &job_infos);
        bool GetJobByName(JobInfo &job_info, const std::string &name);
        bool GetJobByID(JobInfo &job_info, const std::string &id);
        bool AddJob(JobInfo &job);
        bool UpdateJob(const JobInfo &job);
        bool UpdateJobLastSheduleTime(const std::string &id, const ptime &last_shedul_time);
        bool RemoveJob(const std::string &id);

        bool AddRunJob(JobResult &run_param);
        bool RemoveRunJob(std::vector<std::string> &ids);
        // bool GetRunJobs(std::vector<JobResult>& job_runs);
        bool GetUnFinshedRunJobs(std::vector<JobResult> &job_runs);
        bool UpdateJobResults(std::vector<JobResult> &job_runs);
        bool UpdateJobResult(const JobResult &job_result);
        bool GetLastJobResult(JobResult &job_result, const std::string &job_id);

        bool LoadParallelStrategy(std::map<std::string, int> &strategies);

        bool Cleanup(const std::chrono::system_clock::time_point &before);
    };
} // namespace elasticjob
#endif
