set(BASE_SRCS
    "./ConfigManager.cc"
    "./ConfigManager.h"
    "./DBBaseConn.cc"
    "./DBBaseConn.h"
    "./DBConnPool.cc"
    "./DBConnPool.h"
    "./DBManager.cc"
    "./DBManager.h"
    "./Error.cc"
    "./Error.h"
    "./GuidUtil.cc"
    "./GuidUtil.h"
    "./MessageLoopByThreadPool.cc"
    "./MessageLoopByThreadPool.h"
    "./RedisController.cc"
    "./RedisController.h"
    "./Singleton.h"
    "./StringUtils.h"
    "./Task.h"
    "./ThreadSafeList.h"
    "./ThreadUtils.cc"
    "./ThreadUtils.h"
    "./ThreadWrap.h"
    "./Time.cc"
    "./Time.h"
    "./Timer.cc"
    "./Timer.h"
    "./TimerCounter.cc"
    "./TimerCounter.h"
)

add_library(Base
    STATIC
    ${BASE_SRCS}
)

target_include_directories(Base
    PUBLIC
    /usr/local/include/mysql++
    /usr/include/mysql
)

find_library(muduo_base muduo_base)
find_library(muduo_net muduo_net)


target_link_libraries(Base
    mysqlpp
    redis++
    hiredis
    uuid
    ${muduo_base}
    ${muduo_net}
    pthread rt
)
