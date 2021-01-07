
#include "DBConnPool.h"
#include "ConfigManager.h"

#include <mysql++.h>
#include <iostream>

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

    mysqlpp::Connection *DBConnPool::grab()
    {
        // 暂时先写死，后续改为灵活变化的
        if (conns_in_use_ >= 100)
        {
            return nullptr;
        }
        ++conns_in_use_;
        return mysqlpp::ConnectionPool::grab();
    }

    void DBConnPool::release(const mysqlpp::Connection *pc)
    {
        mysqlpp::ConnectionPool::release(pc);
        --conns_in_use_;
    }

    mysqlpp::Connection *DBConnPool::create()
    {
        return new mysqlpp::Connection(
            db_.empty() ? 0 : db_.c_str(),
            server_.empty() ? 0 : server_.c_str(),
            user_.empty() ? 0 : user_.c_str(),
            password_.empty() ? "" : password_.c_str());
    }

    void DBConnPool::destroy(mysqlpp::Connection* cp)
	{
		delete cp;
	}

    unsigned int max_idle_time()
	{
		return 3;
	}

}