#ifndef __TASK_H__
#define __TASK_H__

namespace base
{
    class Task
    {
    public:
        Task() {}
        virtual ~Task() {}

        // 任务运行函数
        virtual void Exec() = 0;
    };
} // namespace base

#endif
