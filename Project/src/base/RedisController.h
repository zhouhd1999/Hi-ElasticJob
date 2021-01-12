#ifndef __BASE_REDISCONTROLLER_H__
#define __BASE_REDISCONTROLLER_H__

#include "Singleton.h"

#include <map>
#include <string>
#include <vector>

namespace base
{

    struct RedisTransaction;

    class RedisController : public base::Singleton<RedisController>
    {
        friend class base::Singleton<RedisController>;
        RedisController();
    public:
        enum RCStatus
        {
            RCStatus_OK = 0,   // 请求成功
            RCStatus_ERROR,    // 请求成功 redis返回失败
            RCStatus_CRITICAL, // 请求失败 redis无返回
        };

        enum RedisMode
        {
            Unknown,
            Single,  // 单例
            Cluster, // 集群
        };

        struct Options
        {
            RedisMode mode = RedisMode::Single;
            std::string addr;
            int port = 6379;
            bool ssl = false;
            std::string password;
            int max_connection = 4;
        };

        ~RedisController();

        // -------初始化-------
        bool Init(const Options &options);
        void Uninit();

        // -------目前只封装了string类型，需要其他类型的话后续再添加-------

        RCStatus DeleteKey(const std::string &key, RedisTransaction *ptr_transaction = nullptr);
        // key存在则直接覆盖
        RCStatus SetKeyValue(const std::string &key, const std::string &value, RedisTransaction *ptr_transaction = nullptr);

        RCStatus GetKeyValue(std::string &value, const std::string &key, RedisTransaction *ptr_transaction = nullptr);
        // key存在值且为整型才能执行
        RCStatus IncrByKey(const std::string &key, int increte_number, RedisTransaction *ptr_transaction = nullptr);

        // 为键值增加过期时间，msec为过期时间的毫秒数
        RCStatus SetExpireForKey(const std::string &key, int msec, RedisTransaction *ptr_transaction = nullptr);

        // 集群模式的事务只支持在单个节点下执行，需要使用key来选择节点
        RedisTransaction *BeginTransaction(const std::string &key);
        RCStatus EndTransaction(RedisTransaction *ptr_transaction);
        RedisController::RCStatus CancelTransaction(RedisTransaction *ptr_transaction);

        // -------锁-------
        // 返回0表示成功，1表示失败
        int RedisLock(const std::string &name, int msec);

        int RedisUnlock(const std::string &name);

        int ExpireLock(const std::string &name, int msec);
    };
} // namespace base

#endif
