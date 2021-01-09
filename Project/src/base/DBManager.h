
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

        // 申请连接
        mysqlpp::Connection *ApplyConn();

        // 释放连接
        bool FreeConn(mysqlpp::Connection* ptr_conn);

        std::shared_ptr<DBConnPool> GetConnPool();

    private:
        // 目前认为项目中只有一个连接池，mysql++支持多个连接池，后续有必要的话再封装
        std::shared_ptr<DBConnPool> conn_pool_;
    };
}

#endif