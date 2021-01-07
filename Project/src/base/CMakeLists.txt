set(BASE_SRCS
    "./ConfigManager.cc"
    "./ConfigManager.h"
    "./ThreadUtils.cc"
    "./ThreadUtils.h"
    "./Timer.cc"
    "./Timer.h"
    "./DBManager.cc"
    "./DBManager.h"
    "./Singleton.h"
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
    pthread rt
    ${muduo_base}
    ${muduo_net}
)
