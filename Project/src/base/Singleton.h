#ifndef __BASE_SINGLETON_H__
#define __BASE_SINGLETON_H__

#include "muduo/base/noncopyable.h"
#include <mutex>
#include <memory>
namespace base
{
    /** @class 
     *  @brief 设计模式的单件模式，用来取代全局变量
     *
     *  全局只有一份内存，当需要使用的时候才被分配内存
     *  使用的时候通过函数Instance()获取对象的引用
     *
     *
     * 从boost切换为std后，单件有BUG
     * 线程1进入Instance，调用static std::mutex mutex;初始化切换到线程2
     * 线程2不会再进行static，会直接使用mutex，此时mutex未初始化完成，会崩溃
     * boost不会，boost中初始化mutex是不允许被中断切换的
     * 修改方法，将static变量从函数中移除，这样和全局变量一样，在程序启动时，就会优先初始化完成
     *  单件模式，基类使用的模型
     *  class MyClass : public Singleton<MyClass>
     *  {
     *      friend class Singleton<MyClass>;
     *  private:
     *      MyClass() {}
     *  public:
     *      ...
     *  };
     */

    template <class T>
    class Singleton : private muduo::noncopyable
    {
    public:
        // 访问单件的唯一方式
        static T& Instance();

    protected:
        Singleton() {}

        static std::unique_ptr<T> s_instance_;
        static std::mutex mutex_;
    };

    template<class T>
    std::unique_ptr<T> Singleton<T>::s_instance_;

    template<class T>
    std::mutex Singleton<T>::mutex_;

    /** @fn	T& Singleton<T>::Instance()
     *  @brief	获取实例的接口
     *  @param	void
     *  @return	T& 对象的引用
     */
    template <class T>
    inline T& Singleton<T>::Instance()
    {
        if (s_instance_.get() == NULL)
        {
            std::unique_lock<std::mutex> lock(mutex_); //! 加锁避免同时被创建两个实例
                                                          //! Singleton<T>作用域下互斥量，不需要担心不同类型之间的互斥
            if (s_instance_.get() == NULL) // 第二次判断的目的在于不被多线程重复创建
            {
                s_instance_ = std::unique_ptr<T>(new T);
            }
        }
        return *s_instance_.get();
    }

}

#endif