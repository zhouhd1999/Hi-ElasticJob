

set(CC_ELASTIC_JOB_COMMON_SRC
"./Base/ElasticJobTypeDefs.h"
"./Base/LogHelper.h"
"./Base/LogHelper.cc"
"./Base/Errors.h"
"./Base/Errors.cc"
"./Base/elastic_job.pb.h"
"./Base/elastic_job.pb.cc"
"./Base/elastic_job.grpc.pb.h"
"./Base/elastic_job.grpc.pb.cc"
)

set(CC_ELASTIC_JOB_CLIENT_SRC
"./Client/ElasticJobClient.h"
"./Client/ElasticJobClientImpl.h"
"./Client/ElasticJobClientImpl.cc"
)

set(CC_ELASTIC_JOB_SERVICE_SRC
"./Service/Proto/elasticjob_worker_service.pb.h"
"./Service/Proto/elasticjob_worker_service.pb.cc"
"./Service/Proto/elasticjob_worker_service.grpc.pb.h"
"./Service/Proto/elasticjob_worker_service.grpc.pb.cc"
"./Service/ElasticJobService.h"
"./Service/ElasticJobServiceImpl.h"
"./Service/ElasticJobServiceImpl.cc"
"./Service/ElasticLeader.h"
"./Service/ElasticLeader.cc"
"./Service/Job.h"
"./Service/Job.cc"
"./Service/JobRunner.h"
"./Service/LocalWorkClient.h"
"./Service/LocalWorkClient.cc"
"./Service/ParallelStrategy.h"
"./Service/ParallelStrategy.cc"
"./Service/ServiceErrors.h"
"./Service/ServiceErrors.cc"
"./Service/SheduleTime.h"
"./Service/SheduleTime.cc"
"./Service/Storage.h"
"./Service/Storage.cc"
"./Service/Worker.h"
"./Service/Worker.cc"
"./Service/WorkerClient.h"
"./Service/WorkerClientProxy.h"
"./Service/WorkerClientProxy.cc"
)

add_library(Cyclotronic_ElasticJobBase
STATIC
${CC_ELASTIC_JOB_COMMON_SRC}
)

add_library(Cyclotronic_ElasticJobClient
STATIC
${CC_ELASTIC_JOB_CLIENT_SRC}
)

add_library(Cyclotronic_ElasticJobService
STATIC 
${CC_ELASTIC_JOB_SERVICE_SRC}
)

target_include_directories(Cyclotronic_ElasticJobBase
PRIVATE
/root/cpp/Project/src/base
/usr/local/include/google/protobuf
/usr/local/include/grpc
)

target_include_directories(Cyclotronic_ElasticJobClient
PRIVATE
/root/cpp/Project/src/base
/usr/local/include/google/protobuf
/usr/local/include/grpc
)

target_include_directories(Cyclotronic_ElasticJobService
PRIVATE
/root/cpp/Project/src/base
/usr/local/include/mysql++
/usr/include/mysql
/usr/local/include/google/protobuf
/usr/local/include/grpc
)

target_link_libraries(Cyclotronic_ElasticJobBase
PUBLIC
/usr/local/lib/libboost_date_time.so.1.66.0
/usr/local/lib/libgrpc++_unsecure.a
/usr/local/lib/libgrpc++_reflection.a
/usr/local/lib/libgrpc_unsecure.a
/usr/local/lib/libgpr.a
/usr/local/lib/libre2.so
/usr/local/lib/libcares.a
/usr/local/lib/libprotobuf.a
/lib/x86_64-linux-gnu/libz.so.1

-Wl,--no-as-needed
/usr/local/lib/libupb.a
/usr/local/lib/libaddress_sorting.a
/usr/local/lib/libabsl_base.so
/usr/local/lib/libabsl_cord.so
/usr/local/lib/libabsl_status.so
/usr/local/lib/libabsl_throw_delegate.so
/usr/local/lib/libabsl_strings.so
/usr/local/lib/libabsl_str_format_internal.so
/usr/local/lib/libabsl_bad_optional_access.so
/usr/local/lib/libabsl_time.so
/usr/local/lib/libabsl_time_zone.so
/usr/local/lib/libabsl_raw_logging_internal.so
/usr/local/lib/libabsl_int128.so
/usr/local/lib/libabsl_spinlock_wait.so
/usr/local/lib/libabsl_synchronization.so
-Wl,--as-needed
)

target_link_libraries(Cyclotronic_ElasticJobClient
PUBLIC
Base
Cyclotronic_ElasticJobBase
)

target_link_libraries(Cyclotronic_ElasticJobService
PUBLIC
Base
Cyclotronic_ElasticJobBase
) 

# # Unittest =====================================================================
# if (HC_BUID_DEBUG)

# # Cyclotronic_ElasticJob_Unittest ----------------------------------------------
# add_executable(Cyclotronic_ElasticJob_Unittest
# "./Service/SheduleTime_Unittest.cc"
# )

# set_common_target_properities(Cyclotronic_ElasticJob_Unittest)
# target_link_libraries(Cyclotronic_ElasticJob_Unittest
# Cyclotronic
# Cyclotronic_ElasticJobClient
# Cyclotronic_ElasticJobService
# ${HC_BIN_COMMON_LIB_DIR}/gtest${LIB_DIR_SUFFIX}/libgtest${HC_LIB_SUFFIX}.a
# ${HC_BIN_COMMON_LIB_DIR}/gtest${LIB_DIR_SUFFIX}/libgtest_main${HC_LIB_SUFFIX}.a
# )

# Cyclotronic_ElasticJob_ServiceUnittest ---------------------------------------

add_executable(Cyclotronic_ElasticJob_ServiceUnittest
"./Base/MainFor_Unittest.cc"
"./Service/ElasticJobService_Unittest.cc"
)

target_include_directories(Cyclotronic_ElasticJob_ServiceUnittest
PUBLIC
${HC_BIN_COMMON_INCLUDE_DIR}
PRIVATE_HEADER
/root/cpp/Project/src/base
/usr/local/include/mysql++
/usr/include/mysql
)

target_link_libraries(Cyclotronic_ElasticJob_ServiceUnittest
Base
Cyclotronic_ElasticJobClient
Cyclotronic_ElasticJobService
/usr/lib/libgtest.a
boost_system
boost_thread
pthread rt
)

# endif(HC_BUID_DEBUG)
