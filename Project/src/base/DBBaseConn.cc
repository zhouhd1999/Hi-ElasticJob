#include "DBBaseConn.h"
#include "DBManager.h"
#include "muduo/base/Logging.h"

#include <iostream>

using namespace std;
using namespace mysqlpp;

namespace base
{
    Connection *DBBaseConn::ApplyConn()
    {
        Connection* conn= DBConnManager::Instance().GetConnPool()->grab();
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

    bool DBBaseConn::FreeConn(Connection* conn)
    {
        DBConnManager::Instance().GetConnPool()->release(conn);
        return true;
    }

    bool DBBaseConn::Execute(const std::string& sql)
    {
        Connection* conn = ApplyConn();
        if (!conn)
        {
            return false;
        }
        Query query = conn->query(sql);
        if (query.exec())
        {
            LOG_INFO << "Execuate sql success";
            FreeConn(conn);
            return true;
        }
        else
        {
            LOG_ERROR << "Execuate sql failed, sql is: " << sql;
            FreeConn(conn);
            return false;
        }
    }

    bool DBBaseConn::ExecuteQuary(const std::string& sql, StoreQueryResult& res)
    {
        Connection* conn = ApplyConn();
        if (!conn)
        {
            return false;
        }
        Query query = conn->query(sql);
        res = query.store();
        FreeConn(conn);
        return true;
    }
}