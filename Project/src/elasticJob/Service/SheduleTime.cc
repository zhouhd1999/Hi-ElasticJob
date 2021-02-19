#include "SheduleTime.h"

#include "../Base/ElasticJobTypeDefs.h"
#include "muduo/base/Logging.h"

#include "boost/date_time.hpp"

using namespace boost::posix_time;

namespace elasticjob
{
    namespace
    {
        int ParseTime(const std::string &time)
        {
            int hours = 0;
            int minutes = 0;
            int seconds = 0;
            sscanf(time.c_str(), "%2d:%2d:%2d", &hours, &minutes, &seconds);
            if (hours < 0 || hours >= 24 || minutes < 0 || minutes >= 60 || seconds < 0 || seconds >= 60)
                return -1;

            return hours * 60 * 60 + minutes * 60 + seconds;
        }

    } // namespace

    // 获取当前UTC时间
    ptime ScheduleTime::Now() { return ptime(second_clock::universal_time()); }

    ptime Now() { return ptime(second_clock::universal_time()); }

    ptime ScheduleTime::GetNextSheduleTime(const JobInfo &job_info)
    {
        switch (job_info.shedule_type)
        {
        case JobInfo::SheduleType::One:
        {
            if (job_info.shedule_param.empty())
            {
                // 默认现在就开始
                return Now();
            }
            else
            {
                auto ms = milliseconds(atoi(job_info.shedule_param.c_str()));
                return Now() + ms;
            }
        }
        break;
        case JobInfo::SheduleType::Repeat:
        {
            if (job_info.shedule_param.empty())
            {
                // return a invalidate time
                return ptime();
            }
            long long sec = atoll(job_info.shedule_param.c_str());
            if (sec <= 0)
                return ptime();

            if (job_info.last_shedule_time.is_special())
                return Now();

            return Now() + seconds(sec);
        }
        break;
        case JobInfo::SheduleType::Daily:
        {
            ptime next_shedule_time;
            if (job_info.shedule_param.empty())
            {
                next_shedule_time = ptime(Now().date(), time_duration(boost::date_time::special_values::min_date_time));
            }
            else
            {
                int seconds = ParseTime(job_info.shedule_param);
                if (seconds < 0)
                    seconds = 0;

                // 没指定时间默认一天的0点处理
                auto shedule_time = ptime(Now().date(), boost::posix_time::seconds(seconds));
                next_shedule_time = shedule_time;
            }

            if (!job_info.last_shedule_time.is_special() && job_info.last_shedule_time >= next_shedule_time)
            {
                next_shedule_time += boost::posix_time::hours(24);
            }
            return next_shedule_time;
        }
        break;
        default:
            LOG_INFO << "Unknown shedule type";
            return Now();
        }
    }

    // static
    ScheduleTime::TimePoint ScheduleTime::From(const ptime &t)
    {
        return Clock::from_time_t(boost::posix_time::to_time_t(t));
    }

} // namespace elasticjob
