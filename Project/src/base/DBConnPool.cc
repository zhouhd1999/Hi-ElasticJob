
#include "DBConnPool.h"
#include "ConfigManager.h"

#include <mysql++.h>
#include <iostream>

using namespace std;

namespace base
{
    DBConnPool::DBConnPool() {}

    DBConnPool::DBConnPool(const DBConfigParam &param)
        : conns_in_use_(0),
          db_(param.str_db_name),
          server_(param.str_address),
          user_(param.str_user),
          password_(param.str_password)
    {
    }

    DBConnPool::~DBConnPool()
    {
        clear();
    }

    // 向连接池获取一个连接，没有空闲连接会自动创建
    mysqlpp::Connection *DBConnPool::grab()
    {
        // 暂时先写死只能创建100个，后续改为灵活变化的
        if (conns_in_use_ >= 100)
        {
            return nullptr;
        }
        ++conns_in_use_;
        return mysqlpp::ConnectionPool::grab();
    }

    // 释放一个连接
    void DBConnPool::release(const mysqlpp::Connection *pc)
    {
        mysqlpp::ConnectionPool::release(pc);
        --conns_in_use_;
    }

    // 模板方法模式，grab获取一个连接时当此时没有空闲连接，会调用该方法创建一个连接
    mysqlpp::Connection *DBConnPool::create()
    {
        mysqlpp::Connection* conn = new mysqlpp::Connection(false);

        // 设置字符集
        conn->set_option(new mysqlpp::SetCharsetNameOption("utf8"));
        conn->connect(
            db_.empty() ? 0 : db_.c_str(),
            server_.empty() ? 0 : server_.c_str(),
            user_.empty() ? 0 : user_.c_str(),
            password_.empty() ? "" : password_.c_str());
        return conn;
    }

    void DBConnPool::destroy(mysqlpp::Connection* cp)
	{
		delete cp;
	}

    unsigned int DBConnPool::max_idle_time()
	{
		return 3;
	}
}