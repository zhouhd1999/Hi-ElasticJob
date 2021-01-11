#include "../DBBaseConn.h"
#include "../ConfigManager.h"
#include "../DBManager.h"
#include "muduo/base/Logging.h"

using namespace base;

int main()
{
    // 初始化配置文件
    ConfigManager::Instance().InitManager();
    DBConfigParam param = ConfigManager::Instance().GetDBParam();

    // 初始化数据库连接池
    DBConnManager::Instance().InitDBConnManager(param);

    bool bRet = false;
    // 初始化数据库操作对象
    DBBaseConn db_conn;
    do
    {
        if (!db_conn.Begin())
        {
            LOG_ERROR << "Begin transcation error";
            break;
        }

        string sql = "INSERT INTO demo (id, name) VALUES (10, \"qwuiuqw\")";
        if (!db_conn.Execute(sql))
        {
            LOG_ERROR << "Do sql error, the sql is: " << sql ;
            break;
        }
        
        sql = "INSERT INTO demo (id, name) VALUES (11, \"qwuiuqw\")";
        if (!db_conn.Execute(sql))
        {
            LOG_ERROR << "Do sql error, the sql is: " << sql ;
            break;
        }

        if (!db_conn.Commit())
        {
            LOG_ERROR << "Transcation commit error";
            break;
        }

        bRet = true;
    } while (0);

    // 如果bRet != true 说明程序执行异常，需要释放资源
    if (!bRet)
    {
        db_conn.Rollback();
    }
    
    
    return 0;
}