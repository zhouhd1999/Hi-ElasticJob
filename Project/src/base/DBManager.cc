
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

        conn_pool_ = std::shared_ptr<DBConnPool>(new DBConnPool(st_param));
        return true;
    }

    // 反初始化
    bool DBConnManager::UninitDBConnManager()
    {
        // 智能指针自动释放
        return true;
    }

    mysqlpp::Connection *DBConnManager::ApplyConn()
    {
        mysqlpp::Connection* conn= conn_pool_->grab();
        if (conn)
        {
            return conn;
        }
        else
        {
            LOG_INFO << "Apply Conn Failed";
            return nullptr;
        }
    }

    bool DBConnManager::FreeConn(mysqlpp::Connection* conn)
    {
        conn_pool_->release(conn);
        return true;
    }

    std::shared_ptr<DBConnPool> DBConnManager::GetConnPool()
    {
        return conn_pool_;
    }
}