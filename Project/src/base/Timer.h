
#ifndef __BASE_TIMER_H__
#define __BASE_TIMER_H__

#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

#include <cstdint>

namespace base
{
    class Timer
    {
    public:
        // 析构函数
        virtual ~Timer();

    public:
        // 构造传入回调函数
        template<typename F>
        Timer(F func, const std::string& thread_name = std::string())
            : m_fun(func), thread_name_(thread_name)
        {
        }

        // 开始
        void start(uint32_t imsec, bool immediately_run = true);

        // 结束
        void stop();

    private:
        void run();
    private:
        bool m_bexit = false;           // 退出标志
        uint32_t m_imsec = 1000;        // 计时器执行时间间隔，毫秒
        bool m_immediately_run = true;  // 启动的时候是否立即执行以下
        std::function<void()> m_fun;    // 外部传入回调函数
        boost::thread m_thread;         // 线程
        boost::mutex mutex_cond_;       // 条件变量使用
        boost::condition_variable cond_;
        std::string thread_name_;
    };
}

#endif
