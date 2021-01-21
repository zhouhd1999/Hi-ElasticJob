#include "Time.h"

#include <time.h>
#include <stdio.h>

#include <chrono>

namespace base
{
    Time::Time() {}

    Time Time::Now()
    {
        Time t;
        t.ticks_ = Time::NowTicks();
        return t;
    }

    int64_t Time::NowTicks()
    {
        using namespace std::chrono;
        return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    }
} // namespace base
