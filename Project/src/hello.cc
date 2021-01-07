
#include "./base/ConfigManager.h"
#include "./base/DBManager.h"

using namespace base;

int main(int argc, char *argv[])
{
    ConfigManager::Instance().InitManager();
    DBConfigParam param = ConfigManager::Instance().GetDBParam();
    DBConnManager::Instance().InitDBConnManager(param);
}
