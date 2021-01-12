#include "RedisController.h"
#include "muduo/base/Logging.h"

extern "C"
{
#include <hiredis/hiredis.h>
#include <hiredis/read.h>
}

#include <iostream>
#include <mutex>
#include <thread>
#include <list>
#include <cstring>
#include <chrono>
#include <atomic>

#include <boost/thread/shared_mutex.hpp>
#include <sw/redis++/redis++.h>

namespace base
{
    using namespace std;

    // 仅供内部使用，外界访问不到
    namespace
    {
        typedef boost::shared_mutex CRWMutex;
        typedef boost::shared_lock<CRWMutex> CReadLock;  // 读锁
        typedef boost::unique_lock<CRWMutex> CWriteLock; // 写锁

        atomic<RedisController::RedisMode> g_redis_mode(RedisController::RedisMode::Unknown);

        CRWMutex g_redis_locker;
        shared_ptr<sw::redis::Redis> g_redis = nullptr;
        shared_ptr<sw::redis::RedisCluster> g_cluster_redis = nullptr;

        void SetRedis(const shared_ptr<sw::redis::Redis> &redis)
        {
            CWriteLock wlocked(g_redis_locker);
            g_redis = redis;
            g_redis_mode = RedisController::RedisMode::Single;
        }

        shared_ptr<sw::redis::Redis> GetRedis()
        {
            CReadLock rlocked(g_redis_locker);
            return g_redis;
        }

        void SetCluster(const shared_ptr<sw::redis::RedisCluster> &cluster)
        {
            CWriteLock wlocked(g_redis_locker);
            g_cluster_redis = cluster;
            g_redis_mode = RedisController::RedisMode::Cluster;
        }

        shared_ptr<sw::redis::RedisCluster> GetCluster()
        {
            CReadLock rlocked(g_redis_locker);
            return g_cluster_redis;
        }
    } // namespace

/*
 * 返回的redisReply中type类型分别为

 * REDIS_REPLY_STRING  1  字符串   字符串       reply->str reply->len  字符数组+长度
 * REDIS_REPLY_ARRAY   2  列表    reply->elements  reply->element  获得链表指针+长度
 * REDIS_REPLY_INTEGER 3  整型    整型值通过    reply->integer
 * REDIS_REPLY_NIL     4  无数据
 * REDIS_REPLY_STATUS  5  状态    状态信息通过  reply->str reply->len  字符数组+长度
 * REDIS_REPLY_ERROR   6  错误    错误信息通过  reply->str reply->len  字符数组+长度
 */

    void PrintReplyInfo(redisReply *ptr_reply)
    {
        switch (ptr_reply->type)
        {
        case REDIS_REPLY_STRING:
            cout << "reply with string("
                 << "len " << ptr_reply->len << ") : " << ptr_reply->str << endl;
            break;
        case REDIS_REPLY_ARRAY:
            cout << "reply with elements("
                 << "size " << ptr_reply->elements << ")" << endl;
            for (size_t i = 0; i < ptr_reply->elements; ++i)
            {
                PrintReplyInfo(ptr_reply->element[i]);
            }
            break;
        case REDIS_REPLY_INTEGER:
            cout << "reply with interger : " << ptr_reply->integer << endl;
            break;
        case REDIS_REPLY_NIL:
            cout << "reply with no data" << endl;
            break;
        case REDIS_REPLY_STATUS:
            cout << "reply with status : " << ptr_reply->str << endl;
            break;
        case REDIS_REPLY_ERROR:
            cout << "reply with error : " << ptr_reply->str << endl;
            break;

        default:
            break;
        }
    }

    typedef struct RedisTransaction
    {
        RedisTransaction(sw::redis::Transaction &&ts)
            : transaction(move(ts)) {}

        ~RedisTransaction() {}

        sw::redis::Transaction transaction;
    } redisTransaction;

    string GetClusterLockerName(const string &name)
    {
        const char kHashTagForClusterLock[] = "{zhd-cluster-lock}";
        return name + kHashTagForClusterLock;
    }

    string GetRandomString()
    {
        static string random_string;
        if (random_string.empty())
        {
            auto tmp = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
            random_string = to_string(tmp);
        }
        return random_string;
    }

    RedisController::RedisController() {}

    RedisController::~RedisController()
    {
        Uninit();
    }

    bool RedisController::Init(const Options& options)
    {
        if (g_redis != nullptr || g_cluster_redis != nullptr)
            return false;

        sw::redis::ConnectionOptions conn_options;
        conn_options.host = options.addr;
        conn_options.port = options.port;
        conn_options.type = sw::redis::ConnectionType::TCP;
        conn_options.db = 0;
        conn_options.password = options.password;
        conn_options.socket_timeout = chrono::seconds(20);

        sw::redis::ConnectionPoolOptions pool_options;
        pool_options.size = options.max_connection;

        if (options.mode == RedisMode::Single)
        {
            auto redis = make_shared<sw::redis::Redis>(conn_options, pool_options);
            SetRedis(redis);
        }
        else
        {
            auto cluster = make_shared<sw::redis::RedisCluster>(conn_options, pool_options);
            SetCluster(cluster);
        }

        return true;
    }

    void RedisController::Uninit()
    {
        if (g_redis != nullptr)
        {
            g_redis.reset();
        }
        else
        {
            g_cluster_redis.reset();
        }
    }

    RedisController::RCStatus RedisController::DeleteKey(const string &key, RedisTransaction *ptr_transaction)
    {
        if (ptr_transaction != nullptr)
        {
            ptr_transaction->transaction = move(ptr_transaction->transaction.del(key));
            return RCStatus_OK;
        }

        if (g_redis_mode == RedisMode::Single)
        {
            auto redis = GetRedis();
            if (!redis)
                return RCStatus_ERROR;
            return (redis->del(key) > 0) ? RCStatus_OK : RCStatus_ERROR;
        }
        else
        {
            auto redis = GetCluster();
            if (!redis)
                return RCStatus_CRITICAL;

            return (redis->del(key) > 0) ? RCStatus_OK : RCStatus_ERROR;
        }
    }

    RedisController::RCStatus RedisController::SetKeyValue(const string &key, const string &value, RedisTransaction *ptr_transaction)
    {
        if (ptr_transaction != nullptr)
        {
            ptr_transaction->transaction = move(ptr_transaction->transaction.set(key, value));
            return RCStatus_OK;
        }

        if (g_redis_mode == RedisMode::Single)
        {
            auto redis = GetRedis();
            if (!redis)
                return RCStatus_ERROR;

            return redis->set(key, value) ? RCStatus_OK : RCStatus_ERROR;
        }
        else
        {
            auto redis = GetCluster();
            if (redis == nullptr)
                return RCStatus_CRITICAL;

            return redis->set(key, value) ? RCStatus_OK : RCStatus_ERROR;
        }
    }

    RedisController::RCStatus RedisController::GetKeyValue(string &value, const string &key, RedisTransaction *ptr_transaction)
    {
        if (ptr_transaction != nullptr)
        {
            ptr_transaction->transaction = move(ptr_transaction->transaction.get(key));
            return RCStatus_OK;
        }

        if (g_redis_mode == RedisMode::Single)
        {
            auto redis = GetRedis();
            if (!redis)
                return RCStatus_ERROR;

            auto optional = redis->get(key);
            if (!optional)
                return RCStatus_ERROR;

            value = *optional;
            return RCStatus_OK;
        }
        else
        {
            auto redis = GetCluster();
            if (redis == nullptr)
                return RCStatus_CRITICAL;

            auto optional = redis->get(key);
            if (!optional)
                return RCStatus_ERROR;

            value = *optional;
            return RCStatus_OK;
        }
    }

    RedisController::RCStatus RedisController::IncrByKey(const string &key, int increte_number, RedisTransaction *ptr_transaction)
    {
        if (ptr_transaction != nullptr)
        {
            ptr_transaction->transaction = move(ptr_transaction->transaction.incrby(key, increte_number));
            return RCStatus_OK;
        }

        if (g_redis_mode == RedisMode::Single)
        {
            auto redis = GetRedis();
            if (!redis)
                return RCStatus_ERROR;

            redis->incrby(key, increte_number);
            return RCStatus_OK;
        }
        else
        {
            auto redis = GetCluster();
            if (redis == nullptr)
                return RCStatus_CRITICAL;

            redis->incrby(key, increte_number);
            return RCStatus_OK;
        }
    }

    RedisController::RCStatus RedisController::SetExpireForKey(const string &key, int msec, RedisTransaction *ptr_transaction)
    {
        if (ptr_transaction != nullptr)
        {
            ptr_transaction->transaction = move(ptr_transaction->transaction.pexpire(key, msec));
            return RCStatus_OK;
        }

        if (g_redis_mode == RedisMode::Single)
        {
            auto redis = GetRedis();
            if (!redis)
                return RCStatus_ERROR;

            return redis->pexpire(key, msec) ? RCStatus_OK : RCStatus_ERROR;
        }
        else
        {
            auto redis = GetCluster();
            if (redis == nullptr)
                return RCStatus_CRITICAL;

            return redis->pexpire(key, msec) ? RCStatus_OK : RCStatus_ERROR;
        }
    }

    RedisTransaction *RedisController::BeginTransaction(const string &key)
    {
        if (g_redis_mode == RedisMode::Single)
        {
            auto redis = GetRedis();
            if (!redis)
                return nullptr;

            return new RedisTransaction(redis->transaction(false, false));
        }
        else
        {
            auto redis = GetCluster();
            if (!redis)
                return nullptr;

            return new RedisTransaction(redis->transaction(key));
        }
    }

    RedisController::RCStatus RedisController::CancelTransaction(RedisTransaction *ptr_transaction)
    {
        if (ptr_transaction != nullptr)
        {
            ptr_transaction->transaction.discard();
            delete ptr_transaction;
        }
        return RCStatus_OK;
    }

    RedisController::RCStatus RedisController::EndTransaction(RedisTransaction *ptr_transaction)
    {
        RCStatus status = RCStatus_OK;

        if (ptr_transaction != nullptr)
        {
            auto reply = ptr_transaction->transaction.exec();

            delete ptr_transaction;
            ptr_transaction = nullptr;
        }

        return status;
    }

    int RedisController::RedisLock(const string &name, int msec)
    {
        if (g_redis_mode == RedisMode::Single)
        {
            auto redis = GetRedis();
            if (!redis)
                return RCStatus_ERROR;

            auto ret = redis->set(name, GetRandomString(), chrono::milliseconds(msec), sw::redis::UpdateType::NOT_EXIST);
            return ret ? RCStatus_OK : RCStatus_ERROR;
        }
        else
        {
            auto key = GetClusterLockerName(name);
            auto redis = GetCluster();
            if (redis == nullptr)
                return RCStatus_CRITICAL;

            auto node = redis->redis(key, false);

            auto ret = node.set(key, GetRandomString(), chrono::milliseconds(msec), sw::redis::UpdateType::NOT_EXIST);
            return ret ? RCStatus_OK : RCStatus_ERROR;
        }
    }

    int RedisController::RedisUnlock(const string &name)
    {
        // 返回被删除键的数量
        const char script[] =  "if redis.call(\"get\",KEYS[1]) == ARGV[1] then\
                                    return redis.call(\"del\",KEYS[1])\
                                else\
                                    return 100\
                                end";

        if (g_redis_mode == RedisMode::Single)
        {
            auto redis = GetRedis();
            if (!redis)
                return RCStatus_ERROR;

            auto ret = redis->eval<long long>(script, {name}, {GetRandomString()});
            if (ret == 1)
                return RCStatus_OK;

            if (ret == 100)
            {
                LOG_INFO << "lock is not yours or lock does not exist" << name;
            }

            return RCStatus_ERROR;
        }
        else
        {
            auto key = GetClusterLockerName(name);
            auto redis = GetCluster();
            if (redis == nullptr)
                return RCStatus_CRITICAL;

            auto node = redis->redis(key, false);

            auto ret = node.eval<long long>(script, {key}, {GetRandomString()});
            if (ret == 1)
                return RCStatus_OK;

            if (ret == 100)
            {
                LOG_INFO << "lock is not yours or lock does not exist" << name;
            }

            return RCStatus_ERROR;
        }
    }

    int RedisController::ExpireLock(const string &name, int msec)
    {
        int result = 1;
        string str_time = to_string(msec);
        // 设置成功返回1   不存在或设置失败，返回 0
        const char script[] =  "if redis.call(\"get\",KEYS[1]) == ARGV[1] then\
                                    return redis.call(\"pexpire\",KEYS[1], ARGV[2])\
                                else\
                                    return 150\
                                end";

        if (g_redis_mode == RedisMode::Single)
        {
            auto redis = GetRedis();
            if (!redis)
                return RCStatus_ERROR;

            auto ret = redis->eval<long long>(script, {name}, {GetRandomString(), str_time});
            if (ret == 1)
                return RCStatus_OK;

            if (ret == 150)
            {
                LOG_INFO << "lock is not yours or lock does not exist" << name;
            }

            return RCStatus_ERROR;
        }
        else
        {
            string key = GetClusterLockerName(name);
            auto redis = GetCluster();
            if (redis == nullptr)
                return RCStatus_CRITICAL;

            auto node = redis->redis(key, false);

            auto ret = node.eval<long long>(script, {key}, {GetRandomString(), str_time});
            if (ret == 1)
                return RCStatus_OK;

            if (ret == 150)
            {
                LOG_INFO << "lock is not yours or lock does not exist" << name;
            }

            return RCStatus_ERROR;
        }

        return result;
    }
} // namespace base
