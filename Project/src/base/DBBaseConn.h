
#ifndef __BASE_DBBASECONN_H__
#define __BASE_DBBASECONN_H__

#include <mysql++.h>
#include <iostream>
#include <memory>
#include <boost/thread/shared_mutex.hpp>

using namespace mysqlpp;
using namespace std;

//
namespace base
{
    class DBBaseConn
    {
    public:
        DBBaseConn() {}
        ~DBBaseConn() {}
        // 申请连接
        bool ApplyConn();

        // 释放连接
        bool FreeConn();

        // 执行sql，不需要返回数据集，一般为增删改
        bool Execute(const string &sql);

        // 执行sql，返回数据集，一般为查
        bool ExecuteQuary(const string &sql, StoreQueryResult &res);

        // 开启事务
        bool Begin(
            Transaction::IsolationLevel isolationLevel = Transaction::IsolationLevel::serializable, 
            Transaction::IsolationScope isolationScope = Transaction::IsolationScope::global);

        // 提交事务
        bool Commit();

        // 回滚
        void Rollback();

        // 执行事务中的sql
        bool DoTranSql();

        
        // string to_str(const mysqlpp::String& in);

        // int to_int(const mysqlpp::String& in);
        
    private:
        typedef boost::shared_mutex CRWMutex;
        typedef boost::shared_lock<CRWMutex> CReadLock;  // 读锁
        typedef boost::unique_lock<CRWMutex> CWriteLock; // 写锁

        CRWMutex redis_locker_;

        shared_ptr<Transaction> trans_ = nullptr;
        Connection* conn_ = nullptr;   // mysql++内部连接池自动管理创建的Connection对象，不用使用智能指针
    };

} // namespace base

#endif