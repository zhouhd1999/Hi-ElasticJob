#include "../ConfigManager.h"
#include "../DBManager.h"

using namespace base;

int main(int argc, char *argv[])
{
    ConfigManager::Instance().InitManager();
    DBConfigParam param = ConfigManager::Instance().GetDBParam();

    // 连接数据库成功会打印
    DBConnManager::Instance().InitDBConnManager(param);

    return 0;
}