
#include "DBManager.h"
#include "DBConnPool.h"
#include "muduo/base/Logging.h"

#include <mysql++.h>
#include <iostream>
#include <memory>

namespace base
{
    // 构造函数
    DBConnManager::DBConnManager() {}

    // 初始化连接池
    bool DBConnManager::InitDBConnManager(const DBConfigParam& st_param)
    {
        LOG_INFO << "Trying to connect database: " << st_param.str_address << ":" << st_param.port
                 << "/" << st_param.str_db_name << "...";

        CWriteLock wlocked(redis_locker_);
        conn_pool_ = std::shared_ptr<DBConnPool>(new DBConnPool(st_param));
        return true;
    }

    // 反初始化
    bool DBConnManager::UninitDBConnManager()
    {
        conn_pool_.reset();
        return true;
    }

    std::shared_ptr<DBConnPool> DBConnManager::GetConnPool()
    {
        CReadLock rlocked(redis_locker_);
        return conn_pool_;
    }
}