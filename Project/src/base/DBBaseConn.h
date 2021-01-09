
#ifndef __BASE_DBBASECONN_H__
#define __BASE_DBBASECONN_H__

#include <mysql++.h>
#include <iostream>

using namespace mysqlpp;

//
namespace base
{
    class DBBaseConn
    {
    public:
        DBBaseConn() {}
        ~DBBaseConn() {}
        // 申请连接
        Connection *ApplyConn();

        // 释放连接
        bool FreeConn(Connection* ptr_conn);

        // 执行sql，不需要返回数据集，一般为增删改
        bool Execute(const std::string& sql);

        // 执行sql，返回数据集，一般为查
        bool ExecuteQuary(const std::string& sql, StoreQueryResult& res);
    };
}

#endif