
#include "ConfigManager.h"

namespace base
{
    // 构造函数
    ConfigManager::ConfigManager() : is_init_(false) {}

    // 析构函数
    ConfigManager::~ConfigManager() {}

    // 初始化
    bool ConfigManager::InitManager()
    {
        if (is_init_)
        {
            return true;
        }

        //暂时先写死，后续希望从配置中心获取这些信息
        db_param_.str_address = "121.199.16.225";
        db_param_.str_db_name = "test";
        db_param_.str_user = "root";
        db_param_.str_password = "123456987";

        redis_param_.str_address = "121.199.16.225";
        redis_param_.str_passwd = "123456987";

        is_init_ = true;
        return true;
    }
}