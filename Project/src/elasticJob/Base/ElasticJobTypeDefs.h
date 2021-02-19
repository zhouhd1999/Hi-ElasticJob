#ifndef __ELASTIC_JOB_TYPE_DEFS__
#define __ELASTIC_JOB_TYPE_DEFS__

#include <stdint.h>
#include <memory>
#include <set>
#include <string>

#include "../../base/Error.h"

#include <boost/date_time/posix_time/ptime.hpp>

using namespace base;

namespace elasticjob
{
    class JobRunner;
    class JobFactory;

    struct WorkerAbility
    {
        std::set<std::string> categories;
        int max_parallel_job = 100;
    };

    struct Configuration
    {
        bool try_as_leader = true;
        std::string cluster_name;
        std::string host_ip;
        uint16_t host_port;
        int work_pool_max_thread = 100;
        int work_pool_min_thread = 1;
        JobFactory *job_factory;
        std::string parallel_limits;
        WorkerAbility ability;
    };

    struct JobInfo
    {
        using ptime = boost::posix_time::ptime;
        enum class SheduleType
        {
            One = 0,    // 即时任务
            Repeat = 1, // 定时间隔, shedule_param 中的参数单位为秒
            Daily = 2,  // 每日, shedule_param 格式为：%2d:%2d:%2d
        };
        std::string id;                              // 任务ID
        std::string name;                            // 任务名称
        std::string category;                        // 任务类型
        int priority = 0;                            // 调度优先级
        SheduleType shedule_type = SheduleType::One; // 调度方式
        std::string shedule_param;                   // 调度参数
        std::string param;                           // 任务运行参数
        std::string parallel_strategy;               // 并发策略
        std::string retry_strategy;                  // 异常重试策略
        ptime last_shedule_time;                     // 记录下上次触发的时间
    };

    struct JobRunParam
    {
        std::string id;
        std::string job_id;
        std::string category;
        std::string param;
    };

    struct JobResult
    {
        enum class Status
        {
            Pending = 0, // 任务已经触发，但未执行
            Doing = 1,   // 任务执行中
            Succ = 2,    // 任务执行成功
            Failed = 3,  // 任务执行失败
        };
        std::string id;
        std::string job_id;
        Status status = Status::Pending;
        std::string result;
        ErrorCode err_code;
        int64_t start_time = 0;
        int64_t finished_time = 0;
        int failed_count = 0;  // 任务执行失败的次数
        std::string worker_id; // 执行任务的服务
        int64_t health_report_time = 0;

        int dispatch_failed_time = 0;
    };
} // namespace elasticjob

#endif