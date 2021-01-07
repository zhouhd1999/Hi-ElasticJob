
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

    // 数据库初始化
    bool DBConnManager::InitDBConnManager(const DBConfigParam& st_param)
    {
        LOG_INFO << "Connect to database: " << st_param.str_address << ":" << st_param.port
                 << "/" << st_param.str_db_name;

        conn_pool_ = std::unique_ptr<DBConnPool>(new DBConnPool(st_param));


        return false;
    }

    // 反初始化
    bool DBConnManager::UninitDBConnManager()
    {
        
    }
}