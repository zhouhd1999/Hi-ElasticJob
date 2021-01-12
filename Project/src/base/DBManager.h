
#ifndef __BASE_DBMANAGER_H__
#define __BASE_DBMANAGER_H__

#include "Singleton.h"
#include "ConfigManager.h"
#include "DBConnPool.h"
#include "DBBaseConn.h"

#include <iostream>
#include <mysql++.h>
#include <vector>
#include <memory>
#include <boost/thread/shared_mutex.hpp>

namespace base
{
    class DBConnManager : public base::Singleton<DBConnManager>
    {
        friend class base::Singleton<DBConnManager>;
        DBConnManager();

    public:
        bool InitDBConnManager(const DBConfigParam& st_param);

        // 反初始化
        bool UninitDBConnManager();

        std::shared_ptr<DBConnPool> GetConnPool();

    private:
        typedef boost::shared_mutex CRWMutex;
        typedef boost::shared_lock<CRWMutex> CReadLock;  // 读锁
        typedef boost::unique_lock<CRWMutex> CWriteLock; // 写锁

        CRWMutex redis_locker_;

        // 目前认为项目中认为连接池是唯一的，但mysql++支持多个连接池，后续有必要的话再封装
        std::shared_ptr<DBConnPool> conn_pool_;
    };
}

#endif