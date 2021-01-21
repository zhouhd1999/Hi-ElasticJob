#ifndef __TIMER_COUNTER_H__
#define __TIMER_COUNTER_H__

#include <chrono>

namespace base
{
    class TimeCounter
    {
    public:
        TimeCounter();
        ~TimeCounter();

        // 重新开始计时
        void Restart();
        // 停止计时
        void Stop();

        // 获取当前过去的时间，单位是秒 s
        double GetCount() const;

    private:
        std::chrono::steady_clock::time_point start_time_;
        std::chrono::steady_clock::time_point stop_time_;
    };
} // namespace base


#endif
