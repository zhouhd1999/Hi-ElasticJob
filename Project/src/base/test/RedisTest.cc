#include "../RedisController.h"
#include "../ConfigManager.h"

#include <iostream>

using namespace std;
using namespace base;

int main()
{
    ConfigManager::Instance().InitManager();
    RedisConfigParam param = ConfigManager::Instance().GetRedisParam();

    RedisController::Options options;
    options.addr = param.str_address;
    options.port = param.port;
    options.password = param.str_passwd;
    options.mode = param.cluster ? RedisController::RedisMode::Cluster : RedisController::RedisMode::Single;
    options.ssl = param.ssl;
    options.max_connection = param.max_conn;

    RedisController::Instance().Init(options);

    // 简单操作
    // RedisController::Instance().SetKeyValue("key1", "value1");
    // for (unsigned int i = 0; i < 10; ++i)
    // {
    //     string key = "test" + to_string(i); 
    //     string value;
    //     RedisController::Instance().GetKeyValue(value, key);
    //     cout << value <<endl;
    // }

    // 事务
    RedisTransaction *transaction = RedisController::Instance().BeginTransaction("key1");
    if (!transaction)
    {
        cout << "BeginTransaction failed, when clear device route";
        return 0;
    }

    RedisController::RCStatus status;
    for (unsigned int i = 0; i < 10; ++i)
    {
        string key = "test" + to_string(i);
        string value = "value" + to_string(i);
        status = RedisController::Instance().SetKeyValue(key, value, transaction);
        if (status != RedisController::RCStatus_OK)
        {
            cout << "failed" << status;

            RedisController::Instance().CancelTransaction(transaction);
            return 0;
        }
    }

    status = RedisController::Instance().EndTransaction(transaction);
    if (status != RedisController::RCStatus_OK)
    {
        cout << "End transaction failed";
        return 0;
    }

    return 0;
}