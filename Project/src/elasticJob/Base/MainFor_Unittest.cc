#include <assert.h>
#include "gtest/gtest.h"

#include "../../base/RedisController.h"
#include "../../base/DBManager.h"
#include "../../base/ConfigManager.h"

using namespace base;


int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);

    // 初始化数据库和Redis
    ConfigManager::Instance().InitManager();
    DBConfigParam db_param = ConfigManager::Instance().GetDBParam();

    db_param.min_conn = 1;
    db_param.max_conn = 4;
    bool ret          = DBConnManager::Instance().InitDBConnManager(db_param);
    assert(ret);

    RedisConfigParam param = ConfigManager::Instance().GetRedisParam();

    RedisController::Options options;
    options.addr = param.str_address;
    options.port = param.port;
    options.password = param.str_passwd;
    options.mode = RedisController::RedisMode::Single;
    options.ssl = param.ssl;
    options.max_connection = param.max_conn;

    ret          = RedisController::Instance().Init(options);
    assert(ret);

    return RUN_ALL_TESTS();
}
