#include "TimerCounter.h"

namespace base
{
    TimeCounter::TimeCounter() :
        start_time_(std::chrono::steady_clock::now()),
        stop_time_(std::chrono::steady_clock::time_point::max())
    {}

    TimeCounter::~TimeCounter() {}

    void TimeCounter::Restart()
    {
        start_time_ = std::chrono::steady_clock::now();
        stop_time_ = std::chrono::steady_clock::time_point::max();
    }

    void TimeCounter::Stop()
    {
        stop_time_ = std::chrono::steady_clock::now();
    }

    double TimeCounter::GetCount() const
    {
        auto stop_time = stop_time_;
        if (stop_time == std::chrono::steady_clock::time_point::max())
        {
            stop_time = std::chrono::steady_clock::now();
        }

        auto span = std::chrono::duration_cast<std::chrono::duration<double>>
                (stop_time - start_time_);

        return span.count();
    }
} // namespace base
