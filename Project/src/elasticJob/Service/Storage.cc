#include "Storage.h"

#include <sstream>
#include <time.h>

#include "../../base/DBBaseConn.h"
#include "../../base/StringUtils.h"
#include "../Base/ElasticJobTypeDefs.h"
#include "muduo/base/Logging.h"
#include "../../base/GuidUtil.h"
#include "boost/date_time.hpp"
#include "boost/date_time/posix_time/ptime.hpp"

using namespace boost::posix_time;
using namespace base;

namespace elasticjob
{
    namespace
    {
        void ParseJobIfo(std::vector<JobInfo> &infos, StoreQueryResult &res)
        {
            JobInfo info;
            for (unsigned int i = 0; i < res.size(); ++i)
            {
                info.id = to_str(res[i]["id"]);
                info.name = to_str(res[i]["name"]);
                info.category = to_str(res[i]["category"]);
                info.priority = to_int(res[i]["priority"]);
                info.shedule_type = static_cast<JobInfo::SheduleType>(to_int(res[i]["shedule_type"]));
                info.shedule_param = to_str(res[i]["shedule_param"]);
                info.param = to_str(res[i]["param"]);
                info.parallel_strategy = to_str(res[i]["parallel_strategy"]);
                info.retry_strategy = to_str(res[i]["retry_strategy"]);
                int64_t last = to_int(res[i]["last_shedule_time"]);
                if (last > 0)
                {
                    info.last_shedule_time = from_time_t(last / 1000);
                }
                infos.push_back(info);
            }
        }

        void ParseJobResultInfo(std::vector<JobResult> &results, StoreQueryResult &res)
        {
            JobResult result;
            for (unsigned int i = 0; i < res.size(); ++i)
            {
                result.id = to_str(res[i]["id"]);
                result.job_id = to_str(res[i]["job_id"]);
                result.status = static_cast<JobResult::Status>(to_int(res[i]["status"]));
                result.result = to_str(res[i]["result"]);
                auto temp_code = to_str(res[i]["error_code"]);
                if (!temp_code.empty())
                {
                    result.err_code = ErrorCode::FromString(temp_code);
                    result.err_code.SetDesc(to_str(res[i]["error_msg"]));
                }
                result.start_time = to_int(res[i]["start_time"]);
                result.finished_time = to_int(res[i]["finish_time"]);
                result.failed_count = to_int(res[i]["failed_count"]);
                result.worker_id = to_str(res[i]["worker_id"]);
            }
        }
    } // namespace
    Storage::Storage() {}

    Storage::~Storage() {}

    bool Storage::GetAllJobs(std::vector<JobInfo> &job_infos)
    {
        ostringstream os;
        os << "SELECT * FROM job_info WHERE deleted_flag IS NULL";

        DBBaseConn db_conn;
        StoreQueryResult res;

        db_conn.ExecuteQuary(os.str(), res);

        ParseJobIfo(job_infos, res);

        return true;
    }

    bool Storage::GetJobByName(JobInfo &job_info, const std::string &name)
    {
        ostringstream os;
        os << "SELECT * FROM job_info WHERE deleted_flag IS NULL AND name = "
           << sqlfmt(name)
           << " LIMIT 1";

        DBBaseConn db_conn;
        StoreQueryResult res;

        db_conn.ExecuteQuary(os.str(), res);

        vector<JobInfo> job_infos;
        ParseJobIfo(job_infos, res);
        job_info = job_infos[0];

        return true;
    }

    bool Storage::GetJobByID(JobInfo &job_info, const std::string &id)
    {
        std::ostringstream os;
        os << "SELECT * FROM job_info WHERE deleted_flag IS NULL AND id = "
           << sqlfmt(id)
           << " LIMIT 1";

        DBBaseConn db_conn;
        StoreQueryResult res;

        db_conn.ExecuteQuary(os.str(), res);

        vector<JobInfo> job_infos;
        ParseJobIfo(job_infos, res);
        job_info = job_infos[0];

        return true;
    }

    bool Storage::AddJob(JobInfo &job)
    {
        job.id = CreateGuid();
        std::ostringstream os;
        os << "INSERT job_info(id, name, category, priority, shedule_type, shedule_param, param"
           << " , parallell_strategy, retry_strategy, last_shedule_time) VALUES("
           << sqlfmt(job.id) << ", " << sqlfmt(job.name) << ", " << sqlfmt(job.category) << ", "
           << job.priority << ", " << static_cast<int>(job.shedule_type) << ", "
           << sqlfmt(job.shedule_param) << ", " << sqlfmt(job.param) << ", "
           << sqlfmt(job.parallel_strategy) << ", " << sqlfmt(job.retry_strategy) << ", "
           << sqlfmt(boost::posix_time::to_time_t(job.last_shedule_time) * 1000) << ")";

        DBBaseConn db_conn;
        return db_conn.Execute(os.str());
    }

    bool Storage::UpdateJobLastSheduleTime(const std::string &id, const ptime &last_shedul_time)
    {
        std::ostringstream os;
        os << "UPDATE job_info SET last_shedule_time = "
           << sqlfmt(boost::posix_time::to_time_t(last_shedul_time) * 1000)
           << " WHERE id = " << sqlfmt(id);

        DBBaseConn db_conn;
        return db_conn.Execute(os.str());
    }

    bool Storage::UpdateJob(const JobInfo &job)
    {
        std::ostringstream os;
        os << "UPDATE job_info SET last_shedule_time = "
           << sqlfmt(boost::posix_time::to_time_t(job.last_shedule_time) * 1000)
           << ", category = " << sqlfmt(job.category)
           << ", priority = " << job.priority
           << ", shedule_type = " << static_cast<int>(job.shedule_type)
           << ", shedule_param = " << sqlfmt(job.shedule_param)
           << ", param = " << sqlfmt(job.param)
           << ", parallell_strategy = " << sqlfmt(job.parallel_strategy)
           << ", retry_strategy = " << sqlfmt(job.retry_strategy)
           << ", last_shedule_time = " << sqlfmt(boost::posix_time::to_time_t(job.last_shedule_time) * 1000)
           << " WHERE id = " << sqlfmt(job.id);

        DBBaseConn db_conn;
        return db_conn.Execute(os.str());
    }

    bool Storage::RemoveJob(const std::string &id)
    {
        std::ostringstream os;
        os << "UPDATE job_info SET deleted_at = " 
           // 获取当前时间，精确到毫秒
           << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()
           << " WHERE id = " << sqlfmt(id);

        DBBaseConn db_conn;
        return db_conn.Execute(os.str());
    }

    bool Storage::AddRunJob(JobResult &run_param)
    {
        run_param.id = CreateGuid();
        std::ostringstream os;
        os << "INSERT job_run(id, job_id, status, result, error_code, error_msg,"
           << " start_time, finish_time, failed_count, worker_id) VALUES ( "
           << sqlfmt(run_param.id) << ", " << sqlfmt(run_param.job_id) << ", "
           << static_cast<int>(run_param.status) << ", " << sqlfmt(run_param.result) << ", "
           << sqlfmt(run_param.err_code.ToString()) << ", " << sqlfmt(run_param.err_code.GetDesc()) << ", "
           << sqlfmt(run_param.start_time) << ", " << sqlfmt(run_param.finished_time) << ", "
           << run_param.failed_count << ", " << sqlfmt(run_param.worker_id) << ")";

        DBBaseConn db_conn;
        return db_conn.Execute(os.str());
    }

    bool Storage::RemoveRunJob(std::vector<std::string> &ids)
    {
        std::ostringstream os;
        os << "DELETE FROM job_run"
           << " WHERE id IN (" << sqlfmt(std::vector<std::string>(ids)) << ")";

        DBBaseConn db_conn;
        return db_conn.Execute(os.str());
    }

    bool Storage::GetUnFinshedRunJobs(std::vector<JobResult> &job_runs)
    {
        std::ostringstream os;
        os << "SELECT * FROM job_run WHERE finish_time IS NULL OR finish_time = 0";

        DBBaseConn db_conn;
        StoreQueryResult res;

        db_conn.ExecuteQuary(os.str(), res);

        ParseJobResultInfo(job_runs, res);

        return true;
    }
    bool Storage::UpdateJobResults(std::vector<JobResult> &job_runs)
    {
        std::vector<std::string> sqls;
        std::ostringstream os;

        for (auto &run : job_runs)
        {
            os.str("");
            os << "UPDATE job_run SET"
               << " status=" << static_cast<int>(run.status)
               << ", result=" << sqlfmt(run.result)
               << ", error_code=" << sqlfmt(run.err_code.ToString())
               << ", error_msg=" << sqlfmt(run.err_code.GetDesc())
               << ", start_time=" << sqlfmt(run.start_time)
               << ", finish_time=" << sqlfmt(run.finished_time)
               << ", failed_count=" << run.failed_count
               << ", worker_id=" << sqlfmt(run.worker_id)
               << " WHERE id=" << sqlfmt(run.id);
            sqls.push_back(os.str());
        }

        bool bRet = false;
        // 开始事务
        DBBaseConn db_conn;
        do
        {
            if (!db_conn.Begin())
            {
                LOG_ERROR << "Begin transcation error";
                break;
            }

            bool bFlag = true;
            for (auto &sql : sqls)
            {
                if (!db_conn.Execute(sql))
                {
                    LOG_ERROR << "Do sql error, the sql is: " << sql ;
                    bFlag = false;
                    break;
                }
            }
            if (!bFlag)
            {
                break;
            }

            if (!db_conn.Commit())
            {
                LOG_ERROR << "Transcation commit error";
                break;
            }

            bRet = true;
        } while (0);

        // 如果bRet != true 说明程序执行异常，需要释放资源
        if (!bRet)
        {
            db_conn.Rollback();
            return false;
        }

        return true;
    }

    bool Storage::UpdateJobResult(const JobResult &job_result)
    {
        std::vector<JobResult> results({job_result});
        return UpdateJobResults(results);
    }

    bool Storage::GetLastJobResult(JobResult &job_result, const std::string &job_id)
    {
        std::ostringstream os;
        os << "SELECT * FROM job_run"
           << " WHERE job_id = " << sqlfmt(job_id)
           << "ORDER BY start_time DESC LIMIT 1";

        DBBaseConn db_conn;
        StoreQueryResult res;

        db_conn.ExecuteQuary(os.str(), res);

        vector<JobResult> job_results;
        ParseJobResultInfo(job_results, res);
        job_result = job_results[0];

        return true;
    }

    bool Storage::LoadParallelStrategy(std::map<std::string, int> &strategies)
    {
        std::ostringstream os;
        os << "SELECT `key`, `limit` FROM job_parallel_strategy";

        DBBaseConn db_conn;
        StoreQueryResult res;

        db_conn.ExecuteQuary(os.str(), res);

        for (unsigned int i = 0; i < res.size(); ++i)
        {
            auto key = to_str(res[i]["id"]);
            auto limit = to_int(res[i]["limit"]);
            strategies.insert(std::make_pair(key, limit));
        }

        return true;
    }

    bool Storage::Cleanup(const std::chrono::system_clock::time_point &before)
    {
        using namespace std::chrono;

        int64_t ticks = duration_cast<std::chrono::milliseconds>(before.time_since_epoch()).count();

        std::ostringstream os;
        os << "DELETE FROM job_info"
           << " WHERE deleted_at < " << ticks;

        DBBaseConn db_conn;

        db_conn.Execute(os.str());

        os.str("");
        os << "DELETE FROM job_run"
           << " WHERE finish_time < " << ticks << " AND finish_time!=0";

        db_conn.Execute(os.str());

        return true;
    }
} // namespace elasticjob