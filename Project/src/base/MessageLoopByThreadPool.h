#ifndef __MESSAGE_LOOP_BY_THREAD_POOL_H__
#define __MESSAGE_LOOP_BY_THREAD_POOL_H__

#include <thread>
#include <memory>
#include <string>

namespace base
{
    class Task;
    class ThreadWrap;
    struct MessageLoopWorkDyThreadPoolData;

    class MessageLoopDyThreadPool
    {
        friend class ThreadWrap;

    public:
        MessageLoopDyThreadPool();
        virtual ~MessageLoopDyThreadPool();

        // int min_thread: 最大个数
        // int max_thread: 线程最小个数
        // int avg_task_num_thread: 每个线程允许排队任务数，排队任务超过了，则开新的线程
        virtual bool Start(int min_thread, int max_thread, int avg_task_num_thread = 5, const std::string &name = std::string());

        // 等待线程中的任务全部结束后才会关闭
        virtual void Stop();

        // 添加一个普通任务
        void PostTask(Task *pTask);

        // 添加一个普通任务,智能指针类型
        void PostTask(std::shared_ptr<Task> ptr_task);

        // 获取任务数量
        int GetWorkTaskCount() const;

        std::string GetName() const;

    private:
        // 执行任务
        bool RunTask(ThreadWrap *worker);

        // 添加线程
        void AddThread();

        // 移除线程，投递到移除的缓存中
        void RemoveThread(ThreadWrap *ptr_thread);

        // 真正执行移除线程的操作
        void DoRemoveThread();

    private:
        MessageLoopWorkDyThreadPoolData *ptr_data_; // 私有数据，申明为指针能够保证类大小不变，后续能够向下兼容
    };
} // namespace base

#endif
