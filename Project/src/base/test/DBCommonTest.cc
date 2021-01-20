#include "../ConfigManager.h"
#include "../DBManager.h"
#include "../StringUtils.h"

#include <iostream>
#include <vector>
#include <typeinfo>
#include <mysql++.h>


using namespace base;
using namespace std;
using namespace mysqlpp;

int main(int argc, char *argv[])
{
    ConfigManager::Instance().InitManager();
    DBConfigParam param = ConfigManager::Instance().GetDBParam();

    // 初始化数据库连接池
    DBConnManager::Instance().InitDBConnManager(param);

    DBBaseConn db_conn;
    // db_conn.Execute("INSERT INTO demo (id, name) VALUES (4, \"qwuiuqw\")");
    std::ostringstream os;
    std::string s = "哈哈";
    os << "SELECT * FROM demo WHERE name = " << sqlfmt("哈哈");
    // 创建接受数据集对象
    StoreQueryResult res;
    db_conn.ExecuteQuary(os.str(), res);
    for (unsigned int i = 0; i < res.size(); ++i)
    {
        std::string id = to_str(res[i]["id"]);
        std::cout << id << std::endl;   
    }

    string s1("1,2,3,4,5,6,7,8,9");
    vector<string> vStr;
    String2Array(vStr,s1);
    for (auto& a : vStr)
    {
        cout << a << endl;
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