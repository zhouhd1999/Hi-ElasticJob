#ifndef __BASE_DBMANAGER_H__
#define __BASE_DBMANAGER_H__

#include "Singleton.h"
#include "ConfigManager.h"
#include "DBConnPool.h"

#include <mysql++.h>
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

    private:
        std::unique_ptr<DBConnPool> conn_pool_;
    };
}

#endif