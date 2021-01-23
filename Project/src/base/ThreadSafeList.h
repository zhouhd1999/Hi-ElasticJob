#ifndef __THREAD_SAFE_LIST_H__
#define __THREAD_SAFE_LIST_H__
#include <mutex>
#include <list>
namespace base
{
    // 支持线程安全的list
    template <typename T>
    class ThreadSafeList
    {
    public:
        // 尝试pop数据，true则表示弹出成功，false则表示弹出失败
        bool TryPop(T &popValue)
        {
            std::lock_guard<std::mutex> lock(data_mutex_);
            if (datas_.empty())
            {
                return false;
            }
            popValue = std::move(datas_.front());
            datas_.pop_front();
            return true;
        }

        // push数据
        void PushBack(T new_value)
        {
            std::lock_guard<std::mutex> lock(data_mutex_);
            datas_.push_back(std::move(new_value));
        }

        void PushFront(T new_value)
        {
            std::lock_guard<std::mutex> lock(data_mutex_);
            datas_.push_front(std::move(new_value));
        }

        // 清空数据
        void Clear()
        {
            std::lock_guard<std::mutex> lock(data_mutex_);
            std::list<T> tmp;
            datas_.swap(tmp);
        }

        // 获取数量
        int Count() const
        {
            std::lock_guard<std::mutex> lock(data_mutex_);
            return datas_.size();
        }

        // 是否为空
        bool Empty() const
        {
            std::lock_guard<std::mutex> lock(data_mutex_);
            return datas_.empty();
        }

        // 删除某元素
        void Erase(T value)
        {
            std::lock_guard<std::mutex> lock(data_mutex_);
            datas_.remove(value);
        }

    private:
        mutable std::mutex data_mutex_;
        std::list<T> datas_;
    };
} // namespace base

#endif
