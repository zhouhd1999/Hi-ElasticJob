#include "ElasticLeader.h"

#include "Storage.h"
#include "WorkerClient.h"
#include "SheduleTime.h"

#include "../../base/StringUtils.h"
#include "../../base/ThreadUtils.h"

#include "../../base/Time.h"
#include "../Base/Errors.h"
#include "../Base/LogHelper.h"
#include "muduo/base/Logging.h"

#include "boost/asio/error.hpp"
#include "boost/date_time.hpp"
#include <algorithm>

using std::chrono::system_clock;
using namespace boost::posix_time;
using namespace base;

namespace elasticjob
{
    namespace
    {
        // 从小到大比较
        bool JobInfoCompare(const JobInfoDetail &l, const JobInfoDetail &r)
        {
            if (l.next_shedule_time.is_special() && !r.next_shedule_time.is_special())
                return false;
            if (!l.next_shedule_time.is_special() && r.next_shedule_time.is_special())
                return true;
            if (l.next_shedule_time.is_special() && r.next_shedule_time.is_special())
                return false;
            return l.next_shedule_time < r.next_shedule_time;
        }

        void RemoveJobResultByID(ElasticLeader::JobResultMap &results, const std::string &id)
        {
            auto iter = std::find_if(results.begin(), results.end(),
                                     [id](const ElasticLeader::JobResultMap::value_type &v) -> bool { return v.second.id == id; });
            assert(iter != results.end());
            if (iter != results.end())
                results.erase(iter);
        }

        ElasticLeader::JobList::iterator FindJobByJobID(ElasticLeader::JobList &job_list, const std::string &id)
        {
            return std::find_if(job_list.begin(), job_list.end(),
                                [id](ElasticLeader::JobList::value_type &i) -> bool { return i->second.id == id; });
        }

        bool DeleteJobInListByID(ElasticLeader::JobList &job_list, const std::string &id)
        {
            auto iter = FindJobByJobID(job_list, id);
            if (iter == job_list.end())
                return false;
            job_list.erase(iter);
            return true;
        }

        bool CheckListOrderIsASC(const ElasticLeader::JobList &job_list)
        {
            if (job_list.empty())
                return true;

            auto iter = job_list.begin();
            ScheduleTime::ptime temp = (*iter)->second.next_shedule_time;
            for (; iter != job_list.end(); iter++)
            {
                if (temp > (*iter)->second.next_shedule_time)
                    return false;
                temp = (*iter)->second.next_shedule_time;
            }
            return true;
        }

        template <typename FROM, typename TO>
        void UpdateJobInfo(TO &to, const FROM &from)
        {
            to.category = from.category;
            to.parallel_strategy = from.parallel_strategy;
            to.priority = from.priority;
            to.param = from.param;
            to.shedule_type = from.shedule_type;
            to.shedule_param = from.shedule_param;
            to.retry_strategy = from.retry_strategy;
        }

    } // namespace

    ElasticLeader::ElasticLeader(const Configuration &config)
        : config_(config), parallel_strategy_(config_.parallel_limits), storage_(new Storage()), waiting_timer_(context_), check_prepare_job_timer_(context_), check_job_results_timer_(context_), check_dispath_pending_job_timer_(context_), check_job_run_health_timer_(context_), report_stat_timer_(context_), cleanup_db_timer_(context_)
    {
    }

    ElasticLeader::~ElasticLeader() {}

    bool ElasticLeader::Start()
    {
        // 加载内存中并发策略的配置
        InitParallelStrategiesFromDB();

        // 获取系统里所有的任务，并插入waiting_jobs_中
        std::vector<JobInfo> jobs;
        storage_->GetAllJobs(jobs);
        for (const auto &job : jobs)
            AddJobToMap(job);

        // 获取系统里运行，但未完成的任务
        std::vector<JobResult> job_results;
        storage_->GetUnFinshedRunJobs(job_results);

        std::vector<std::string> need_delete_results;

        for (const auto &result : job_results)
        {
            auto iter = std::find_if(waiting_jobs_.begin(), waiting_jobs_.end(),
                                     [result](JobMap::iterator i) { return i->second.id == result.job_id; });
            // 如果这个待插入的任务在 waiting_jobs_ 中没有，证明是个不存在的任务，需要删除
            if (iter == waiting_jobs_.end())
            {
                LOG_WARN << "job run(" << result.job_id << ") can not find it's job info(" << result.job_id
                         << ")";
                need_delete_results.push_back(result.job_id);
                continue;
            }

            job_results_.insert(std::make_pair(result.job_id, result));

            // 如果job result为pending, 直接将任务添加到Prepared并等待调度
            if (result.status == JobResult::Status::Pending)
            {
                AddJobToPrepared(*iter);
            }
            // 其他状态表示已经开始执行
            else
            {
                // 重建时，不考虑并发策略的限制，只计算
                parallel_strategy_.RecalcuLimit((*iter)->second.parallel_item, (*iter)->second.id);
                AddJobToRun(*iter);
            }
            waiting_jobs_.erase(iter);
        }

        if (!need_delete_results.empty())
            storage_->RemoveRunJob(need_delete_results);

        work_guard_.reset(new WorkGurad(boost::asio::make_work_guard(context_)));
        work_thread_ = std::thread([this]() {
            SetThreadName("ej-leader");
            context_.run();
        });

        NextCheckWaitingJob(ScheduleTime::Now());
        NextCheckPreparedJob(ScheduleTime::Now());
        NextCheckJobRunHealth(ScheduleTime::Now());
        NextToReportStat(ScheduleTime::Now());
        NextCleanupStorage(ScheduleTime::Now());

        return true;
    }

    bool ElasticLeader::Stop()
    {
        LOG_INFO << "Stop leader";

        waiting_timer_.cancel();
        check_prepare_job_timer_.cancel();
        check_job_results_timer_.cancel();
        check_dispath_pending_job_timer_.cancel();
        check_job_results_timer_.cancel();
        report_stat_timer_.cancel();
        cleanup_db_timer_.cancel();

        work_guard_->reset();
        context_.stop();
        if (work_thread_.joinable())
            work_thread_.join();

        return true;
    }

    std::string ElasticLeader::AddJob(JobInfo &job_info)
    {
        if (!job_info.name.empty())
        {
            JobInfo job_old;
            storage_->GetJobByName(job_old, job_info.name);
            if (!job_old.id.empty())
            {
                UpdateJobInfo(job_old, job_info);

                if (!storage_->UpdateJob(job_old))
                {
                    LOG_ERROR << "Update job failed";
                    return ClientErrors::kDBError;
                }

                // 在工作线程里处理任务的
                context_.post([job_old, this]() {
                    auto iter = jobs_.find(job_old.id);
                    if (iter == jobs_.end())
                    {
                        LOG_WARN << "job(" << job_old.id << ") is not in memory";
                        AddJobToMap(job_old);
                        return;
                    }

                    // 任务调度
                    bool is_parallel_strategy_changed = iter->second.parallel_strategy != job_old.parallel_strategy;
                    UpdateJobInfo(iter->second, job_old);
                    // 如果并行策略修改了，需要重新解析
                    if (is_parallel_strategy_changed)
                    {
                        iter->second.parallel_item.Parse(iter->second.parallel_strategy);
                    }

                    if (iter->second.status == JobInfoDetail::Status::Waiting)
                    {
                        NextCheckWaitingJob(ScheduleTime::Now());
                    }
                });
            }
            else
            {
                if (!storage_->AddJob(job_info))
                {
                    LOG_ERROR << "Add " << job_info.id << " to storage failed.";
                    return ClientErrors::kDBError;
                }

                context_.post([job_info, this]() {
                    AddJobToMap(job_info);
                    NextCheckWaitingJob(ScheduleTime::Now());
                });
            }
        }
        else
        {
            if (!storage_->AddJob(job_info))
            {
                LOG_ERROR << "Add " << job_info.id << " to storage failed.";
                return ClientErrors::kDBError;
            }

            context_.post([job_info, this]() {
                AddJobToMap(job_info);
                NextCheckWaitingJob(ScheduleTime::Now());
            });
        }

        return std::string();
    }

    void ElasticLeader::RemoveJob(const std::string &id)
    {
        JobInfo job_old;
        storage_->RemoveJob(id);
        context_.post([this, id]() {
            auto iter = jobs_.find(id);
            if (iter == jobs_.end())
            {
                LOG_WARN << "job(" << id << ") is not in memory";
                return;
            }

            DeleteJobByID(id);
        });
    }

    void ElasticLeader::RemoveJobByName(const std::string &name)
    {
        JobInfo job_info;
        storage_->GetJobByName(job_info, name);
        if (!job_info.id.empty())
        {
            RemoveJob(job_info.id);
        }
    }

    std::string ElasticLeader::GetResult(JobResult &result, const std::string &id)
    {
        if (id.empty())
        {
            return ClientErrors::kRequestParamError;
        }

        if (!storage_->GetLastJobResult(result, id))
        {
            return ClientErrors::kDBError;
        }

        if (!result.id.empty())
        {
            return std::string();
        }

        JobInfo job_info;
        if (!storage_->GetJobByID(job_info, id))
        {
            return ClientErrors::kDBError;
        }

        if (job_info.id.empty())
        {
            return ClientErrors::kJobNotFound;
        }
        result.job_id = id;
        result.status = JobResult::Status::Pending;
        return std::string();
    }

    void ElasticLeader::AddWorker(WorkerClient *worker)
    {
        std::unique_lock<std::mutex> locked(worker_lock_);
        workers_.insert(std::make_pair(worker->GetWorkerID(), worker));

        LOG_INFO << "Add new worker(" << worker->GetWorkerID() << ")";
        NextDispathPendingJob(ScheduleTime::Now());
        NextCheckJobRunStatus(ScheduleTime::Now());
    }

    void ElasticLeader::RemoveWorker(WorkerClient *worker)
    {
        std::unique_lock<std::mutex> locked(worker_lock_);
        auto iter = workers_.find(worker->GetWorkerID());
        if (iter == workers_.end())
        {
            LOG_ERROR << "can not find woker in map";
            return;
        }

        workers_.erase(iter);

        LOG_INFO << "Remove a worker(" << worker->GetWorkerID() << ")";

        // 检测分配到这个Worker的任务
        NextCheckJobRunHealth(ScheduleTime::Now());
    }

    void ElasticLeader::OnTimeCheckWaitingTask(const error_code &ec)
    {
        if (ec == boost::asio::error::operation_aborted)
        {
            LOG_INFO << "check waiting operation abort";
            return;
        }

        if (jobs_.empty())
        {
            LOG_INFO << "There is not job in list";
            return;
        }

        // move all prepared job to prepared list
        bool has_prepared_job = false;
        for (auto iter = waiting_jobs_.begin(); iter != waiting_jobs_.end();)
        {
            auto &job = (*iter)->second;
            // jobs是从小到大排序的，遍历时，如果大于当前时间，那么，后面的肯定也是大于的。
            if (job.next_shedule_time > ScheduleTime::Now())
                break;

            job.last_shedule_time = job.next_shedule_time;
            storage_->UpdateJobLastSheduleTime(job.id, job.last_shedule_time);
            AddJobToPrepared(*iter);
            AddJobResult(*iter);

            iter = waiting_jobs_.erase(iter);
            has_prepared_job = true;
        }

        // 有新的可触发任务时，触发一次
        if (has_prepared_job)
            NextCheckPreparedJob(ScheduleTime::Now());

        // 没有任务，不需要再检查了。
        if (waiting_jobs_.empty())
            return;

        auto next_check = waiting_jobs_.front()->second.next_shedule_time;
        NextCheckWaitingJob(next_check);
    }

    void ElasticLeader::NextCheckWaitingJob(const ptime &next)
    {
        auto t = next;
        if (next.is_special() && !waiting_jobs_.empty())
        {
            t = waiting_jobs_.front()->second.next_shedule_time;
        }

        if (t.is_special() || t - ScheduleTime::Now() > minutes(1))
            t = ScheduleTime::Now() + minutes(1);

        LOG_INFO << "Next check waiting job time is " << boost::posix_time::to_iso_extended_string(t); // 将ptime输出为yyyy-mm-ddT00:00:00格式

        boost::system::error_code ec;
        waiting_timer_.expires_at(ScheduleTime::From(t), ec);
        if (ec)
        {
            LOG_ERROR << "Expire at failed, err=" << ec.message();
            return;
        }
        waiting_timer_.async_wait(std::bind(&ElasticLeader::OnTimeCheckWaitingTask, this, std::placeholders::_1));
    }

    void ElasticLeader::AddJobToMap(const JobInfo &job)
    {
        // map的返回值pair<pair, bool>
        auto ret = jobs_.insert(std::make_pair(job.id, job));
        /**
         * @brief 并发策略先从数据库以字符串的形式取出，再在这里解析存入ParallelItem对象
         * 
         */
        ret.first->second.parallel_item.Parse(ret.first->second.parallel_strategy);

        AddJobToWait(ret.first);
    }

    void ElasticLeader::RemoveJobFromMap(const JobInfo &job)
    {
        auto iter = jobs_.find(job.id);
        if (iter == jobs_.end())
            return;

        auto waiting_iter = std::find_if(waiting_jobs_.begin(), waiting_jobs_.end(),
                                         [&job](const JobMap::const_iterator &iter) { return iter->second.id == job.id; });

        if (waiting_iter == waiting_jobs_.end())
        {
            auto prepare_iter = std::find_if(prepared_jobs_.begin(), prepared_jobs_.end(),
                                             [&job](const JobMap::const_iterator &iter) { return iter->second.id == job.id; });
            assert(prepare_iter != prepared_jobs_.end());
            if (prepare_iter == prepared_jobs_.end())
            {
                LOG_ERROR << "IMPOSSIBLE THING";
            }

            if (prepare_iter != prepared_jobs_.end())
            {
                prepared_jobs_.erase(prepare_iter);
            }
        }
        else
        {
            waiting_jobs_.erase(waiting_iter);
        }

        jobs_.erase(iter);
    }

    bool ElasticLeader::AddJobToWait(JobMap::iterator iter)
    {
        // 获取任务下一次执行的时间
        iter->second.next_shedule_time = ScheduleTime::GetNextSheduleTime(iter->second);
        LOG_INFO << iter->second.id
                 << " next shedule time: " << to_iso_string(iter->second.next_shedule_time);

        // 任务根据执行时间排序，找到该任务插入的位置
        auto wait_iter = std::find_if(waiting_jobs_.begin(), waiting_jobs_.end(),
                                      [&iter](const JobMap::const_iterator &i) { return JobInfoCompare(iter->second, i->second); });

        iter->second.status = JobInfoDetail::Status::Waiting;

        LOG_INFO << iter->second.id << " add to Waiting";

        if (wait_iter == waiting_jobs_.end())
            waiting_jobs_.push_back(iter);
        else
            waiting_jobs_.insert(wait_iter, iter);

        return true;
    }

    bool ElasticLeader::AddJobToPrepared(JobMap::iterator iter)
    {
        auto iter2 = std::find_if(prepared_jobs_.begin(), prepared_jobs_.end(),
                                  [iter](JobMap::const_iterator i) { return iter->second.priority > i->second.priority; });

        iter->second.status = JobInfoDetail::Status::Prepared;

        LOG_INFO << iter->second.id << " add to Prepared";

        if (iter2 == prepared_jobs_.end())
        {
            prepared_jobs_.push_back(iter);
        }
        else
        {
            prepared_jobs_.insert(iter2, iter);
        }
        return true;
    }

    void ElasticLeader::AddJobToRun(JobMap::iterator iter)
    {
        LOG_INFO << iter->second.id << " add to run, status is pending to dispath";
        iter->second.status = JobInfoDetail::Status::Pending;
        runing_jobs_.push_back(iter);
    }

    void ElasticLeader::AddJobResult(JobIterator iter)
    {
        auto existed_iter = job_results_.find(iter->second.id);
        if (existed_iter != job_results_.end())
        {
            LOG_ERROR << iter->second.id << " is already in job result map";
            return;
        }

        LOG_INFO << iter->second.id << " add job result";

        JobResult job_result;
        job_result.job_id = iter->second.id;
        job_result.status = JobResult::Status::Pending;
        storage_->AddRunJob(job_result);
        job_results_.insert(std::make_pair(job_result.job_id, job_result));
    }

    void ElasticLeader::SwitchRunningToWait(const std::string &job_id)
    {
        auto running_iter = FindJobByJobID(runing_jobs_, job_id);

        if (running_iter == runing_jobs_.end())
        {
            parallel_strategy_.RemoteLimitByJobID(job_id);
            LOG_ERROR << "Can not find Job(" << job_id << ") in runing list, maybe it has been deleted";
            return;
        }

        JobInfoDetail &job_info = (*running_iter)->second;

        parallel_strategy_.RemoveLimitItem(job_info.parallel_item, job_info.id);

        LOG_INFO << job_info.id << " run -> pending";

        // 任务执行完成，将任务从运行队列移动到 等待队列
        job_info.next_shedule_time = ScheduleTime::GetNextSheduleTime(job_info);
        job_info.status = JobInfoDetail::Status::Waiting;

        AddJobToWait(*running_iter);
        runing_jobs_.erase(running_iter);

        NextCheckWaitingJob(ptime());
    }

    void ElasticLeader::SwitchRunningToPrepare(const std::string &job_id)
    {
        auto running_iter = FindJobByJobID(runing_jobs_, job_id);
        if (running_iter == runing_jobs_.end())
        {
            LOG_ERROR << "Can not find Job(" << job_id << ") in runing list";
            return;
        }

        JobInfoDetail &job_info = (*running_iter)->second;
        job_info.status = JobInfoDetail::Status::Prepared;

        LOG_INFO << job_info.id << " Running -> Prepare";

        parallel_strategy_.RemoveLimitItem(job_info.parallel_item, job_info.id);

        // 任务执行完成，将任务从运行队列移动到 等待队列
        AddJobToPrepared(*running_iter);
        runing_jobs_.erase(running_iter);
    }

    void ElasticLeader::DeleteJobByID(const std::string &job_id)
    {
        auto job_iter = jobs_.find(job_id);
        if (job_iter == jobs_.end())
        {
            LOG_ERROR << "can not find job(" << job_id << ")";
            return;
        }

        JobInfoDetail &job_info = job_iter->second;

        bool delete_succ = true;
        switch (job_info.status)
        {
        case JobInfoDetail::Status::Waiting:
            delete_succ = DeleteJobInListByID(waiting_jobs_, job_id);
            break;
        case JobInfoDetail::Status::Prepared:
            delete_succ = DeleteJobInListByID(prepared_jobs_, job_id);
            break;
        case JobInfoDetail::Status::Pending:
        case JobInfoDetail::Status::Running:
            parallel_strategy_.RemoveLimitItem(job_info.parallel_item, job_info.id);
            delete_succ = DeleteJobInListByID(runing_jobs_, job_id);
            break;
        default:
            assert(0);
            break;
        }

        if (!delete_succ)
        {
            assert(!"delete failed");
            LOG_ERROR << "Delete job failed, job(" << job_id
                      << "), status=" << static_cast<int>(job_iter->second.status);
        }
    }

    void ElasticLeader::SwitchWaitingToPrepare(const std::string &job_id)
    {
        auto wait_iter = FindJobByJobID(waiting_jobs_, job_id);
        assert(wait_iter != waiting_jobs_.end());
        if (wait_iter == waiting_jobs_.end())
        {
            LOG_ERROR << "Can not find Job(" << job_id << ") in waiting list";
            return;
        }

        // 任务执行完成，将任务从运行队列移动到 等待队列
        AddJobToPrepared(*wait_iter);
        waiting_jobs_.erase(wait_iter);
    }

    void ElasticLeader::OnTimeCheckPreparedJob(const error_code &ec)
    {
        if (ec == boost::asio::error::operation_aborted)
        {
            LOG_INFO << "check prepared jobs is canceled.";
            return;
        }

        // 根据优先级排队
        // std::sort(prepared_jobs_.begin(), prepared_jobs_.end())
        bool has_new_running_job = false;
        for (auto iter = prepared_jobs_.begin(); iter != prepared_jobs_.end();)
        {
            JobInfoDetail &job_info = (*iter)->second;
            if (parallel_strategy_.AddLimitItem(job_info.parallel_item, job_info.id))
            {
                (*iter)->second.status = JobInfoDetail::Status::Pending;
                AddJobToRun((*iter));
                has_new_running_job = true;
                iter = prepared_jobs_.erase(iter);
            }
            else
            {
                iter++;
            }
        }

        if (has_new_running_job)
        {
            NextDispathPendingJob(ScheduleTime::Now());
        }
    }

    void ElasticLeader::NextCheckPreparedJob(const ptime &next)
    {
        auto n = ScheduleTime::From(next);
        auto exp = check_prepare_job_timer_.expiry();
        if (exp > std::chrono::system_clock::now() && n > exp)
            return;

        boost::system::error_code ec;
        check_prepare_job_timer_.expires_at(ScheduleTime::From(next), ec);
        if (ec)
        {
            LOG_ERROR << "Expire at failed, err=" << ec.message();
            return;
        }
        check_prepare_job_timer_.async_wait(
            std::bind(&ElasticLeader::OnTimeCheckPreparedJob, this, std::placeholders::_1));
    }

    void ElasticLeader::NextCheckJobRunStatus(const ptime &next)
    {
        boost::system::error_code ec;
        check_job_results_timer_.expires_at(ScheduleTime::From(next), ec);
        if (ec)
        {
            LOG_ERROR << "Expire at failed, err=" << ec.message();
            return;
        }
        check_job_results_timer_.async_wait(
            std::bind(&ElasticLeader::OnTimeCheckJobRunStatus, this, std::placeholders::_1));
    }

    void ElasticLeader::OnTimeCheckJobRunStatus(const error_code &ec)
    {
        if (ec == boost::asio::error::operation_aborted)
        {
            LOG_INFO << "check jobs run status is canceled.";
            return;
        }

        bool has_pending_job = false;
        for (auto &result : job_results_)
        {
            if (result.second.status == JobResult::Status::Pending)
            {
                has_pending_job = true;
            }
            else
            {

            }
        }

        if (has_pending_job)
        {
            NextDispathPendingJob(ScheduleTime::Now());
        }
    }

    void ElasticLeader::OnTimeToReportStat(const error_code &ec)
    {
        if (ec == boost::asio::error::operation_aborted)
        {
            LOG_INFO << "report is canceled.";
            return;
        }

        LOG_INFO << "Job total: " << jobs_.size() << ", waiting: " << waiting_jobs_.size()
                 << ", prepared: " << prepared_jobs_.size() << ", runing: " << runing_jobs_.size()
                 << ", results: " << job_results_.size();

        LOG_INFO << "Parallel Limits:" << parallel_strategy_.GetStat();

        {
            std::unique_lock<std::mutex> locked(worker_lock_);
            LOG_INFO << "Workers num: " << workers_.size();
        }

        NextToReportStat(ptime());
    }

    void ElasticLeader::NextToReportStat(const ptime &next)
    {
        auto t = next;
        if (t.is_special())
        {
            t = ScheduleTime::Now() + minutes(10);
        }

        boost::system::error_code ec;
        report_stat_timer_.expires_at(ScheduleTime::From(t), ec);
        if (ec)
        {
            LOG_ERROR << "Expire at failed, err=" << ec.message();
            return;
        }
        report_stat_timer_.async_wait(std::bind(&ElasticLeader::OnTimeToReportStat, this, std::placeholders::_1));
    }

    bool ElasticLeader::HasPendingJob() const
    {
        for (auto &result : job_results_)
        {
            if (result.second.status != JobResult::Status::Pending)
                continue;

            return true;
        }
        return false;
    }

    void ElasticLeader::NextDispathPendingJob(const ptime &next)
    {
        boost::system::error_code ec;
        check_dispath_pending_job_timer_.expires_at(ScheduleTime::From(next), ec);
        if (ec)
        {
            LOG_ERROR << "Expire at failed, err=" << ec.message();
            return;
        }
        check_dispath_pending_job_timer_.async_wait(
            std::bind(&ElasticLeader::DispatchPendingJob, this, std::placeholders::_1));
    }

    void ElasticLeader::DispatchPendingJob(const error_code &ec)
    {
        if (ec == boost::asio::error::operation_aborted)
        {
            LOG_INFO << "dispatch pending job is canceled.";
            return;
        }

        if (job_results_.empty())
        {
            LOG_INFO << "There is not job to dispatch";
            return;
        }

        LOG_INFO << "dispatch pending job";

        std::unique_lock<std::mutex> locked(worker_lock_);
        std::vector<std::tuple<WorkerStatistics, WorkerClient *>> stats;
        for (auto &client : workers_)
        {
            WorkerStatistics stat;
            if (!client.second->GetStatistical(stat))
            {
                LOG_INFO << "client(" << client.first << ") get statistics failed. ";
                continue;
            }
            stats.push_back(std::make_tuple(stat, client.second));
        }
        auto comparer = [](const std::tuple<WorkerStatistics, WorkerClient *> &l,
                           const std::tuple<WorkerStatistics, WorkerClient *> &r) {
            return std::get<0>(l).running_job < std::get<0>(r).running_job;
        };

        std::sort(stats.begin(), stats.end(), comparer);

        std::vector<std::string> remove_results;
        std::vector<JobResult> update_results;
        std::vector<std::string> dispatch_faileds;

        bool has_dispatch_failed = false;

        for (auto &result : job_results_)
        {
            if (result.second.status != JobResult::Status::Pending)
                continue;

            auto job_iter = jobs_.find(result.first);
            if (job_iter == jobs_.end())
            {
                LOG_WARN << "JOB(" << result.second.job_id << ") is removed";
                remove_results.push_back(result.second.id);
                continue;
            }
            if (job_iter->second.status != JobInfoDetail::Status::Pending)
            {
                continue;
            }

            bool dispathed_job = false;
            // 找到有效的client去做这个任务
            for (auto &iter : stats)
            {
                // 判断服务是否还可以执行更多的任务。
                if (std::get<0>(iter).running_job >= std::get<0>(iter).max_job)
                    continue;

                // 判断Worker是否支持执行这个任务。
                if (std::get<0>(iter).categories.find(job_iter->second.category) ==
                    std::get<0>(iter).categories.end())
                    continue;

                // 添加任务一定是成功的，如果下发过程中失败WorkerClient需要通过回调返回过失败
                JobRunParam run_param;
                run_param.id = result.second.id;
                run_param.job_id = result.second.job_id;
                run_param.param = job_iter->second.param;
                run_param.category = job_iter->second.category;
                if (!std::get<1>(iter)->AddJob(run_param))
                {
                    LOG_ERROR << "Dispatch job(" << job_iter->second.id << ") to worker("
                              << std::get<1>(iter)->GetWorkerID() << ") failed";
                    break;
                }

                job_iter->second.status = JobInfoDetail::Status::Running;
                result.second.status = JobResult::Status::Doing;
                result.second.worker_id = std::get<1>(iter)->GetWorkerID();
                result.second.start_time = Time::Now().GetTicks();
                result.second.health_report_time = 0;
                update_results.push_back(result.second);

                std::get<0>(iter).running_job += 1;

                dispathed_job = true;
                LOG_INFO << "Dispath job(" << job_iter->first << ") to Worker("
                         << std::get<1>(iter)->GetWorkerID() << ")";
                break;
            }

            if (!dispathed_job)
            {
                // 没有能力执行的任务，就不要丢到Running里
                LOG_INFO << "no worker can do job(" << result.first << ") now";
                has_dispatch_failed = true;
                result.second.dispatch_failed_time += 1;
                dispatch_faileds.push_back(result.second.job_id);
            }
            else
            {
                std::sort(stats.begin(), stats.end(), comparer);
            }
        }

        if (!remove_results.empty())
        {
            storage_->RemoveRunJob(remove_results);
            for (auto &id : remove_results)
            {
                RemoveJobResultByID(job_results_, id);
            }
        }

        if (!update_results.empty())
        {
            storage_->UpdateJobResults(update_results);
        }

        bool has_add_to_prepared = false;
        // 如果由分配任务失败的情况，5秒后重试
        if (has_dispatch_failed)
        {
            // 分配失败的任务，处理
            for (const auto &id : dispatch_faileds)
            {
                auto result_iter = job_results_.find(id);
                if (result_iter == job_results_.end())
                {
                    continue;
                }

                if (result_iter->second.dispatch_failed_time > 3)
                {
                    LOG_WARN << "Job(" << result_iter->second.job_id << " failed 3, remove it";
                    result_iter->second.status = JobResult::Status::Failed;
                    result_iter->second.finished_time = Time::Now().GetTicks();
                    storage_->UpdateJobResult(result_iter->second);
                    job_results_.erase(result_iter);
                    HandleJobAfterRunFinished(result_iter->second.job_id);
                }
                else
                {
                    SwitchRunningToPrepare(result_iter->second.job_id);
                    has_add_to_prepared = true;
                }
            }
            LOG_INFO << "Dispath failed, retry after 5s";
            NextDispathPendingJob(ScheduleTime::Now() + boost::posix_time::seconds(5));
            if (has_add_to_prepared)
                NextCheckPreparedJob(ScheduleTime::Now() + boost::posix_time::seconds(5));
        }
    }

    void ElasticLeader::NextCheckJobRunHealth(const ptime &next)
    {
        auto temp_next = next;
        if (temp_next.is_special())
        {
            temp_next = ScheduleTime::Now() + boost::posix_time::seconds(5);
        }
        boost::system::error_code ec;
        check_job_run_health_timer_.expires_at(ScheduleTime::From(temp_next), ec);
        if (ec)
        {
            LOG_ERROR << "Expire at failed, err=" << ec.message();
            return;
        }
        check_job_run_health_timer_.async_wait(
            std::bind(&ElasticLeader::CheckJobRunHealth, this, std::placeholders::_1));
    }

    void ElasticLeader::CheckJobRunHealth(const error_code &ec)
    {
        if (ec == boost::asio::error::operation_aborted)
        {
            LOG_INFO << "Check job run health is aborted";
            return;
        }

        std::set<std::string> worker_ids;
        {
            std::unique_lock<std::mutex> locked(worker_lock_);
            for (const auto &worker : workers_)
                worker_ids.insert(worker.first);
        }

        bool has_redo_job = false;

        auto now = Time::Now();
        for (auto &result : job_results_)
        {
            if (result.second.status != JobResult::Status::Doing)
                continue;

            if (result.second.health_report_time == 0)
            {
                result.second.health_report_time = now.GetTicks();
                continue;
            }

            // 检查时间间隔超过15s任务认为异常，重做
            if (now.GetTicks() - result.second.health_report_time > 15000)
            {
                LOG_WARN << "Job Run(id" << result.second.id << ") is not health"
                         << ", current worker is " << result.second.worker_id;
                result.second.status = JobResult::Status::Pending;
                result.second.worker_id = std::string();
                storage_->UpdateJobResult(result.second);
                SwitchRunningToPrepare(result.second.job_id);
                has_redo_job = true;
                continue;
            }

            if (result.second.worker_id.empty() && result.second.status == JobResult::Status::Doing)
            {
                LOG_WARN << "Job Run(id" << result.second.id << ") is not health";
                result.second.status = JobResult::Status::Pending;
                result.second.worker_id = std::string();
                storage_->UpdateJobResult(result.second);
                SwitchRunningToPrepare(result.second.job_id);
                has_redo_job = true;
                continue;
            }

            if (worker_ids.find(result.second.worker_id) == worker_ids.end())
            {
                LOG_WARN << "Job Run(id" << result.second.id << ") is not health"
                         << ", current worker is " << result.second.worker_id << " is gone";
                result.second.status = JobResult::Status::Pending;
                result.second.worker_id = std::string();
                storage_->UpdateJobResult(result.second);
                SwitchRunningToPrepare(result.second.job_id);
                has_redo_job = true;
                continue;
            }
        }

        if (has_redo_job)
        {
            NextCheckPreparedJob(ScheduleTime::Now());
        }

        NextCheckJobRunHealth(ptime());
    }

    void ElasticLeader::HandleJobAfterRunFinished(const std::string &job_id)
    {
        auto iter = jobs_.find(job_id);
        if (iter == jobs_.end())
        {
            LOG_ERROR << "Can not find job(" << job_id << ")";
            return;
        }

        switch (iter->second.shedule_type)
        {
        case JobInfo::SheduleType::One:
            DeleteJobByID(job_id);
            storage_->RemoveJob(job_id);
            break;
        case JobInfo::SheduleType::Repeat:
        case JobInfo::SheduleType::Daily:
            SwitchRunningToWait(job_id);
            break;
        default:
            assert(!"unknow type");
        }
        NextCheckPreparedJob(ptime());
        NextDispathPendingJob(ScheduleTime::Now());
    }

    void ElasticLeader::UpdateJobResultStatus(const JobResult &result)
    {
        // 如果调用时不在工作线程中，分配任务，再处理。
        if (work_thread_.get_id() != std::this_thread::get_id())
        {
            assert(result.status != JobResult::Status::Pending);
            context_.post(std::bind(&ElasticLeader::UpdateJobResultStatus, this, result));
            return;
        }

        LOG_INFO << "Update job(" << result.job_id;

        auto result_iter = job_results_.find(result.job_id);
        if (result_iter == job_results_.end())
        {
            LOG_ERROR << "job(" << result.job_id << ") is not found, what's happen";
            return;
        }

        if (result_iter->second.worker_id != result.worker_id)
        {
            LOG_ERROR << "Job result(id=" << result.job_id << ")'s worker is diff. "
                      << "from worker_id=" << result.worker_id
                      << ", but record is=" << result_iter->second.worker_id;
            return;
        }

        result_iter->second.health_report_time = Time::Now().GetTicks();

        if (result.status == JobResult::Status::Doing)
        {
            LOG_INFO << "Worker is start to do job(" << result.job_id << ")";
            return;
        }

        JobResult &job_result = result_iter->second;

        assert(result.status == JobResult::Status::Failed || result.status == JobResult::Status::Succ);
        switch (result.status)
        {
        case JobResult::Status::Succ:
        {
            job_result.finished_time = Time::Now().GetTicks();
            job_result.status = result.status;
            job_result.err_code = result.err_code;
            job_result.result = result.result;

            storage_->UpdateJobResult(job_result);

            job_results_.erase(result_iter);

            HandleJobAfterRunFinished(result.job_id);
        }
        break;
        case JobResult::Status::Failed:
        {
            job_result.finished_time = Time::Now().GetTicks();
            job_result.status = result.status;
            job_result.err_code = result.err_code;
            job_result.result = result.result;
            job_result.failed_count += 1;

            // 使用retry strategy
            if (job_result.failed_count < 3)
            {
                job_result.worker_id.clear();
                job_result.status = JobResult::Status::Pending;
                storage_->UpdateJobResult(job_result);
                SwitchRunningToPrepare(result_iter->second.job_id);
                NextCheckPreparedJob(ptime());
                NextDispathPendingJob(ScheduleTime::Now());
            }
            else
            {
                storage_->UpdateJobResult(job_result);
                job_results_.erase(result_iter);
                HandleJobAfterRunFinished(result.job_id);
            }
        }
        break;
        default:
            break;
        }
    }

    void ElasticLeader::OnWorkerStarted() {}
    void ElasticLeader::OnWorkerWillStop(WorkerClient *client) {}
    void ElasticLeader::OnWorkerStopped(WorkerClient *client) {}

    void ElasticLeader::OnJobRunStatusChanged(WorkerClient *client, const JobResult &param)
    {
        context_.post(std::bind(&ElasticLeader::UpdateJobResultStatus, this, param));
    }

    void ElasticLeader::InitParallelStrategiesFromDB()
    {
        std::map<std::string, int> limits;
        storage_->LoadParallelStrategy(limits);

        parallel_strategy_.LoadLimits(limits);
    }

    void ElasticLeader::NextCleanupStorage(const ptime &next)
    {
        auto temp_next = next;
        if (temp_next.is_special())
        {
            temp_next = ScheduleTime::Now() + boost::posix_time::hours(24);
        }

        boost::system::error_code ec;
        cleanup_db_timer_.expires_at(ScheduleTime::From(temp_next), ec);
        if (ec)
        {
            LOG_ERROR << "Expire at failed, err=" << ec.message();
            return;
        }
        cleanup_db_timer_.async_wait(
            std::bind(&ElasticLeader::CleanupStorage, this));
    }

    void ElasticLeader::CleanupStorage()
    {
        auto before = std::chrono::system_clock::now() - std::chrono::hours(24 * 3);
        storage_->Cleanup(before);
    }

} // namespace elasticjob
