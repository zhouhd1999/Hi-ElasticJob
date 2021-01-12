### 项目简介
简易的分布式定时任务系统，目前还在开发中，依赖的基础库已经封装完成


### 目录树
├── Common   **依赖的三方库，也放进来了**
│  ├──hiredis
│  ├──muduo
│  ├──mysql
│  └──redis-plus-plus
├── Project
│  ├── build
│  │  └── bin  **生成的demo都在这个文件夹中**
│  ├── src
│  │  ├── base
│  │  │  ├── test        **base库测试**
│  │  │  ├── CMakeLists.txt   **base库的CMAKE**
│  │  │  └── base库的c++代码
│  │  └── Elastic相关代码
│  └── CMakeLists.txt
└── README.md

### 当前依赖

[muduo](https://github.com/chenshuo/muduo)	使用了muduo库的日志系统

[MySQL++](https://tangentsoft.com/mysqlpp/home)	对MySQL++进行了二次封装，方便上层应用调用

[redis++](https://github.com/sewenew/redis-plus-plus)   使用了redis-plus-plus，对redis-plus-plus进行二次封装，方便上层应用调用