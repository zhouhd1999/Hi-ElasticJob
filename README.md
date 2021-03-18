## 项目简介

简易的分布式定时任务系统，目前还在测试中



项目主要分为base库和Elastic Job两部分。



### base

base封装了例如Redis、MySQL供上层使用方便，以及实现了简单动态线程池、定时器等，base库为Elastic Job服务



### Elastic Job

Elastic Job又分为Client和Service。



#### 如何使用

用户使用前需要继承Service中的两个类，一个是job_factory，另一个是job_runner。job_runner包含任务执行时真正要调用的DoJob方法及JobName，DoJob方法可由用户自由实现，job_factory主要作用为根据Job的类型，与job_runner的成员变量JobName相匹配，从而让服务端能找到任务具体要执行DoJob方法（可参考src/elasticJob/Service目录中的test示例）。重写完成后需要重新编译后并且初始化好MySQL及Redis才可运行启动，等待客户端的连接。接下来仅需要启动客户端，初始化任务属性（JobInfo），通过调用客户端的AddJob方法，即可讲想要执行的定时任务发送到服务端，等待服务端的调度执行。



#### 具体设计

任务属性主要有以下几个参数：

1. 任务类型：供服务端找到需要执行的具体方法使用
2. 调度方式：有三种，分别为立刻执行一次、每隔多久执行一次、每天的几点执行一次
3. 任务优先级：数字越小，优先级越高，越先执行
4. 并发策略：指定需要消耗的资源数

用户定义好任务属性后，将该任务通过调用Client的AddJob发送到主节点。

任务有四个状态，Waiting、Prepared、Pending、Running，以及维护了对应的四个list，一个是waiting_job，未到执行时间的任务会被按即将执行的时间先后插入到waiting_job中。按一定时间间隔轮询该list，到达执行时间的任务状态变为Prepared，并按优先级插入到prepared_list，此时需要匹配并发策略（即该任务需要消耗的资源），并发策略由用户提前配置在数据库中，服务端启动时将并发策略加载到内存，当前剩余的资源够该任务执行，该任务状态转变为Pengding并消耗掉相应的资源。从节点会向主节点持续不断的拉取Pending状态的任务，拉到任务后，任务状态变为Running，并直接开始执行。

主从节点通过抢占Redis锁的方式实现，最先加锁成功的节点为主节点并设置过期时间，key为集群的名字，所有节点及客户端都知道，value为主节点的ip:port，从节点加锁失败后读取value，与主节点建立连接。


### 目录树

├── Project  
│  ├── build  
│  │  └── bin  **生成的demo都在这个文件夹中**  
│  ├── src  
│  │  ├── base  **base库测试**  
│  │  └── Elastic相关代码  **项目核心代码**  
│  └── CMakeLists.txt  
└── README.md  

### 当前依赖

[muduo](https://github.com/chenshuo/muduo)	使用了muduo库的日志系统

[MySQL++](https://tangentsoft.com/mysqlpp/home)	对MySQL++进行二次封装，方便上层应用调用

[hiredis]](https://github.com/redis/hiredis)   对hiredis进行二次封装，方便上层应用调用
