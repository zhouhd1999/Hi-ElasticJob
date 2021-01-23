#include "MessageLoopByThreadPool.h"
#include "Task.h"
#include "ThreadWrap.h"
#include "ThreadSafeList.h"

#include <atomic>
#include <condition_variable>
#include <memory>
namespace base
{
    // 动态线程池私有数据
    struct MessageLoopWorkDyThreadPoolData
    {
        MessageLoopWorkDyThreadPoolData(int min_thread, int max_thread, int avg_task_num_thread, const std::string &name)
            : min_thread_(min_thread),
              max_thread_(max_thread),
              avg_task_num_thread_(avg_task_num_thread),
              name_(name),
              active_thread_(0),
              active_task_(0),
              exit_(false) {}
        ~MessageLoopWorkDyThreadPoolData()
        {
            std::lock_guard<std::mutex> lock_task(mutex_task_);
            tasks_.clear();
        }

        int min_thread_;          // 最小线程数量
        int max_thread_;          // 最大线程数量
        int avg_task_num_thread_; // 每个线程排队的任务数量，排队任务超过该数量，则开启新线程
        std::string name_;

        std::atomic<int> active_thread_; // 当前活动中的线程个数

        std::atomic<int> active_task_; // 当前活动中的任务数
        std::atomic<bool> exit_;       // 是否退出

        std::mutex mutex_task_;                  // 锁定list队列
        std::list<std::shared_ptr<Task>> tasks_; // 队列中的任务

        std::condition_variable task_condition_; // 等待任务来了，或则线程结束

        std::condition_variable remove_condition_;
        ThreadSafeList<ThreadWrap *> remove_threads_; // 待退出的线程
    };

    MessageLoopDyThreadPool::MessageLoopDyThreadPool() : ptr_data_(nullptr) {}

    MessageLoopDyThreadPool::~MessageLoopDyThreadPool()
    {
        Stop();
    }

    bool MessageLoopDyThreadPool::Start(int min_thread, int max_thread, int avg_task_num_thread, const std::string &name)
    {
        ptr_data_ = new MessageLoopWorkDyThreadPoolData(min_thread, max_thread, avg_task_num_thread, name);

        for (int i = 0; i < ptr_data_->min_thread_; ++i)
        {
            AddThread();
        }
        return true;
    }

    void MessageLoopDyThreadPool::Stop()
    {
        {
            std::unique_lock<std::mutex> lock_wait(ptr_data_->mutex_task_);

            // 等待全部任务执行完成
            ptr_data_->remove_condition_.wait(lock_wait, [this]() { return ptr_data_->tasks_.empty(); });
        }

        ptr_data_->exit_.store(true);
        ptr_data_->task_condition_.notify_all(); // 通知结束

        {
            while (ptr_data_->active_thread_ > 0)
            {
                ptr_data_->task_condition_.notify_all(); // 再次通知结束，防止上面的task_condition_.notify_all()通知后，线程才开始等待
                std::unique_lock<std::mutex> lock_wait(ptr_data_->mutex_task_);
                ptr_data_->remove_condition_.wait_for(lock_wait, std::chrono::microseconds(100)); // 等待全部线程结束
            }
        }

        DoRemoveThread();

        delete ptr_data_;
        ptr_data_ = nullptr;
    }

    void MessageLoopDyThreadPool::PostTask(Task *ptr_task)
    {
        PostTask(std::shared_ptr<Task>(ptr_task));
    }

    void MessageLoopDyThreadPool::PostTask(std::shared_ptr<Task> ptr_task)
    {
        if (nullptr == ptr_data_ || ptr_data_->exit_.load())
        {
            return;
        }

        {
            std::lock_guard<std::mutex> lock_task(ptr_data_->mutex_task_);
            ptr_data_->tasks_.push_back(ptr_task);
            if (ptr_data_->active_thread_<ptr_data_->max_thread_ &&static_cast<int>(ptr_data_->tasks_.size()) + ptr_data_->active_task_>
                    ptr_data_->active_thread_ *
                ptr_data_->avg_task_num_thread_)
            {
                AddThread();
            }
        }

        ptr_data_->task_condition_.notify_one();
    }

    void MessageLoopDyThreadPool::AddThread()
    {
        ThreadWrap *ptr_wrap = new ThreadWrap(this);
        if (ptr_wrap->Start())
        {
            ++ptr_data_->active_thread_;
        }
        else
        {
            delete ptr_wrap;
        }
    }

    bool MessageLoopDyThreadPool::RunTask(ThreadWrap *ptr_thread)
    {
        if (ptr_data_->exit_.load())
        {
            RemoveThread(ptr_thread);
            return false;
        }
        DoRemoveThread();

        std::shared_ptr<Task> ptr_task = nullptr;

        {
            std::unique_lock<std::mutex> lock_wait(ptr_data_->mutex_task_);
            if (ptr_data_->tasks_.empty())
            {
                if (ptr_data_->active_thread_ > ptr_data_->min_thread_)
                {
                    // 任务为空，不用立刻删除线程。等待一段时间，可能马上就有任务投递过来了,减少不断删线程，启线程的操作
                    auto ret = ptr_data_->task_condition_.wait_for(lock_wait, std::chrono::milliseconds(1000));
                    if (ret == std::cv_status::timeout)
                    {
                        if (ptr_data_->tasks_.empty() && ptr_data_->active_thread_ > ptr_data_->min_thread_)
                        {
                            RemoveThread(ptr_thread);
                            return false;
                        }
                    }
                }
                else
                {
                    // 等待有新的任务过来
                    ptr_data_->task_condition_.wait(lock_wait);
                }
            }
            if (!ptr_data_->tasks_.empty())
            {
                ptr_task = ptr_data_->tasks_.front();
                ptr_data_->tasks_.pop_front();
            }
        }
        if (ptr_task != nullptr)
        {
            ++ptr_data_->active_task_;
            ptr_task->Exec();
            --ptr_data_->active_task_;
            ptr_data_->remove_condition_.notify_one();
        }

        return true;
    }

    void MessageLoopDyThreadPool::RemoveThread(ThreadWrap *ptr_thread)
    {
        --ptr_data_->active_thread_;
        ptr_data_->remove_threads_.PushBack(ptr_thread);
        ptr_data_->remove_condition_.notify_one();
    }

    void MessageLoopDyThreadPool::DoRemoveThread()
    {
        ThreadWrap *ptr_thread;
        while (ptr_data_->remove_threads_.TryPop(ptr_thread))
        {
            ptr_thread->Join();
            delete ptr_thread;
        }
    }

    int MessageLoopDyThreadPool::GetWorkTaskCount() const
    {
        if (nullptr == ptr_data_)
        {
            return 0;
        }
        std::lock_guard<std::mutex> lock_task(ptr_data_->mutex_task_);
        return ptr_data_->tasks_.size();
    }
} // namespace base
