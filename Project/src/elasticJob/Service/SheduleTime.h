#ifndef __SHEDULE_TIME_H__
#define __SHEDULE_TIME_H__

#include <chrono>

#include "boost/date_time/posix_time/ptime.hpp"

namespace elasticjob
{
    struct JobInfo;

    class ScheduleTime
    {
    public:
        using ptime = boost::posix_time::ptime;
        using Clock = std::chrono::system_clock;
        using TimePoint = Clock::time_point;
        static ptime Now();

        static ptime GetNextSheduleTime(const JobInfo &job_info);

        static TimePoint From(const ptime &t);
    };

} // namespace elasticjob
#endif
