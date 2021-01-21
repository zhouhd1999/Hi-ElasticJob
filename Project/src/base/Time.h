#ifndef __TIME_H__
#define __TIME_H__

#include <stdint.h>
#include <string>

namespace base
{
    class Time
    {
    public:
        Time();

        static Time Now();
        static int64_t NowTicks();

        int64_t GetTicks() const { return ticks_; }

    private:
        int64_t ticks_ = 0; // milliseconds
    };
} // namespace base

#endif
