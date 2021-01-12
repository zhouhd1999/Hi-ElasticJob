#include "DBBaseConn.h"
#include "DBManager.h"
#include "muduo/base/Logging.h"

#include <iostream>

using namespace std;
using namespace mysqlpp;

namespace base
{
    bool DBBaseConn::ApplyConn()
    {
        CWriteLock wlocked(redis_locker_);
        conn_ = DBConnManager::Instance().GetConnPool()->grab();
        if (conn_)
        {
            return true;
        }
        else
        {
            LOG_INFO << "Apply Conn Failed";
            return false;
        }
    }

    bool DBBaseConn::FreeConn()
    {
        CWriteLock wlocked(redis_locker_);
        DBConnManager::Instance().GetConnPool()->release(conn_);
        conn_ = nullptr;
        return true;
    }

    bool DBBaseConn::Execute(const std::string &sql)
    {
        bool isTran = false;

        // 如果conn_不为空指针，说明是事务，事务中已经申请过连接了
        if (conn_ == nullptr)
        {
            if (!ApplyConn())
            {
                return false;
            }
        }
        else
        {
            isTran = true;
        }

        Query query = conn_->query(sql);
        if (query.exec())
        {
            LOG_INFO << "Execuate sql success";

            // 如果是事务，则不用释放链接
            if (!isTran)
            {
                FreeConn();
            }
            return true;
        }
        else
        {
            LOG_ERROR << "Execuate sql failed, sql is: " << sql;

            // 异常情况不管是否是事务都需要释放
            FreeConn();
            return false;
        }
    }

    bool DBBaseConn::ExecuteQuary(const std::string &sql, StoreQueryResult &res)
    {
        bool isTran = false;

        // 如果conn_不为空指针，说明是事务，事务中已经申请过连接了
        if (conn_ == nullptr)
        {
            if (!ApplyConn())
            {
                return false;
            }
        }
        else
        {
            isTran = true;
        }
        Query query = conn_->query(sql);
        res = query.store();
        if (res)
        {
            LOG_INFO << "Execuate sql success";

            // 如果是事务，则不用释放链接
            if (!isTran)
            {
                FreeConn();
            }
            return true;
        }
        else
        {
            LOG_ERROR << "Execuate sql failed, sql is: " << sql;

            // 异常情况不管是否是事务都需要释放
            FreeConn();
            return false;
        }
    }

    bool DBBaseConn::Begin(
        Transaction::IsolationLevel isolationLevel,
        Transaction::IsolationScope isolationScope)
    {
        if (!ApplyConn())
        {
            return false;
        }
        trans_.reset(new Transaction(*conn_, isolationLevel, isolationScope));
        return true;
    }

    bool DBBaseConn::Commit() // 如果Begin了以后代码异常没走到Commit或者Rollback，内存泄漏
    {
        trans_->commit();
        LOG_INFO << "Transcation commit";
        FreeConn();
        return true;
    }

    void DBBaseConn::Rollback()
    {
        trans_->rollback();
        LOG_INFO << "Transcation rollback";
        FreeConn();
    }
} // namespace base