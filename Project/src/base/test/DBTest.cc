#include "../ConfigManager.h"
#include "../DBManager.h"

#include <iostream>
#include <vector>
#include <mysql++.h>

using namespace base;
using namespace mysqlpp;

int main(int argc, char *argv[])
{
    ConfigManager::Instance().InitManager();
    DBConfigParam param = ConfigManager::Instance().GetDBParam();

    // 初始化数据库连接池
    DBConnManager::Instance().InitDBConnManager(param);

    DBBaseConn db_conn;
    db_conn.Execute("INSERT INTO demo (id, name) VALUES (4, \"qwuiuqw\")");

    // 创建接受数据集对象
    StoreQueryResult res;
    db_conn.ExecuteQuary("SELECT * FROM demo", res);
    for (unsigned int i = 0; i < res.size(); ++i)
    {
        std::cout << res[i]["id"] << " " << res[i]["name"] << std::endl;
    }
    
    /*
    1 哈哈
    2 嘿嘿
    3 qwuiuqw
    4 qwuiuqw
    5 qwuiuqw
    */

    return 0;
}