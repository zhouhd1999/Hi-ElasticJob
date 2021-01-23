#ifndef __THREAD_WRAP_H__
#define __THREAD_WRAP_H__

#include "MessageLoopByThreadPool.h"
#include <thread>
#include <assert.h>
#include <boost/thread.hpp>
#include <string>

#include <sys/prctl.h>
#include <sys/types.h>

namespace base
{
    class ThreadWrap
    {
    public:
        explicit ThreadWrap(MessageLoopDyThreadPool *ptr_pool)
            : ptr_pool_(ptr_pool) {}

        bool Start()
        {
            std::string name = ptr_pool_->GetName();
            if (!name.empty())
            {
                prctl(PR_SET_NAME, name.c_str());   // 设置进程的名字
            }

            t_ = boost::thread([this]() { run(); });
            return t_.joinable();
        }

        void Join()
        {
            if (t_.joinable())
            {
                t_.join();
            }
        }

    private:
        void run()
        {
            while (ptr_pool_->RunTask(this))
            {
            }
        }

    private:
        MessageLoopDyThreadPool *ptr_pool_;
        boost::thread t_;
    };
} // namespace base

#endif
