
#ifndef __BASE_DBCONNPOOL_H__
#define __BASE_DBCONNPOOL_H__

#include "ConfigManager.h"

#include <mysql++.h>

namespace base
{
    class DBConnPool : public mysqlpp::ConnectionPool
    {
    public:
        DBConnPool();
        DBConnPool(const DBConfigParam& param);
        ~DBConnPool();

        // 从连接迟中获取一个空闲连接
        mysqlpp::Connection* grab() override;

        // 释放一个连接
        void release(const mysqlpp::Connection* pc) override;

    protected:
        mysqlpp::Connection* create() override;

        void destroy(mysqlpp::Connection* cp) override;

        unsigned int max_idle_time() override;

    private:
        unsigned int conns_in_use_;
        std::string db_, server_, user_, password_;
    };
}

#endif