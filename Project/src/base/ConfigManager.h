#ifndef __BASE_CONFIGMANAGER_H__
#define __BASE_CONFIGMANAGER_H__

#include "Singleton.h"

#include <iostream>

namespace base
{
    // 端口配置
    struct ServicePort
    {
        int http_port = 9999; // Http所使用的端口号
    };

    // 数据库连接参数
    struct DBConfigParam
    {
        int type = 1; // 数据库类型，1、mysql 2、pg
        std::string str_address;
        int port = 3306;
        std::string str_user;
        std::string str_password;
        std::string str_db_name;
        int max_conn = 10;
        int min_conn = 5;
    };

    // redis连接参数
    struct RedisConfigParam
    {
        std::string str_address;
        std::string str_passwd;
        int port = 6379;
        bool ssl = false;
        bool cluster = false;
        int max_conn = 4;
    };

    class ConfigManager : public base::Singleton<ConfigManager>
    {
        friend class base::Singleton<ConfigManager>;
        ConfigManager();

    public:
        ~ConfigManager();

        // 初始化
        bool InitManager();

        // 从配置文件中获取对外开放的服务端口号
        ServicePort GetServicePort() const { return service_port_; };

        // 从配置文件中获取数据库连接信息
        DBConfigParam GetDBParam() const { return db_param_; };

        // 从配置文件中获取redis连接信息
        RedisConfigParam GetRedisParam() const { return redis_param_; };

    private:
        bool is_init_;                  // 是否已经初始化
        ServicePort service_port_;      // 服务端口
        DBConfigParam db_param_;        // 数据库连接参数
        RedisConfigParam redis_param_;  // redis连接参数
    };
}

#endif