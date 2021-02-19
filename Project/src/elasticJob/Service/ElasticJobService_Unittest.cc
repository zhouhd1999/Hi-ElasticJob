#include <thread>
#include "gtest/gtest.h"

#include "../Client/ElasticJobClient.h"
#include "ElasticJobService.h"
#include "JobRunner.h"

#include "muduo/base/Logging.h"

#include "../../base/Error.h"
#include "../../base/RedisController.h"
#include "../../base/DBManager.h"

using namespace base;
using namespace elasticjob;

class TestJob1 : public JobRunner
{
public:
    static const char kJobName[];

    void DoJob(ErrorCode& err_code, const std::string& param) override
    {
        printf("TestJob1 ---- start\n");
        std::this_thread::sleep_for(std::chrono::seconds(1));
        printf("TestJob1 ---- stop\n");
    };
};

const char TestJob1::kJobName[] = "test-job-1";

class TestJob2 : public JobRunner
{
public:
    static const char kJobName[];

    void DoJob(ErrorCode& err_code, const std::string& param) override
    {
        printf("TestJob2 ---- start\n");
        std::this_thread::sleep_for(std::chrono::seconds(1));
        printf("TestJob2 ---- stop\n");
    };
};

const char TestJob2::kJobName[] = "test-job-2";

class TestDailyJob : public JobRunner
{
public:
    static const char kJobName[];

    void DoJob(ErrorCode& err_code, const std::string& param) override
    {
        printf("TestDailyJob ---- start\n");
        std::this_thread::sleep_for(std::chrono::seconds(1));
        printf("TestDailyJob ---- stop\n");
    };
};

const char TestDailyJob::kJobName[] = "test-daily-job";

template <typename T>
std::unique_ptr<T> WrapUnique(T* t)
{
    return std::unique_ptr<T>(t);
}

class TestJobFactory : public JobFactory
{
public:
    ~TestJobFactory() override = default;

    std::unique_ptr<JobRunner> Create(const JobRunParam& param) override
    {
        if (param.category == TestJob1::kJobName)
        {
            return WrapUnique(new TestJob1());
        }
        else if (param.category == TestJob2::kJobName)
        {
            return WrapUnique(new TestJob2());
        }
        else if (param.category == TestDailyJob::kJobName)
        {
            return WrapUnique(new TestDailyJob());
        }

        LOG_INFO << "Not support job";
        return nullptr;
    };
};

std::unique_ptr<TestJobFactory> g_jobfactory;
std::unique_ptr<ElasticJobService> g_job_service_1;
std::unique_ptr<ElasticJobService> g_job_service_2;
std::unique_ptr<ElasticJobClient> g_client;

class ElasticJobServiceUnittestBase : public ::testing::Test
{
public:
    static void SetUpTestCase()
    {
        g_jobfactory.reset(new TestJobFactory());

        Configuration config;
        config.host_ip      = "127.0.0.1";
        config.host_port    = 7888;
        config.cluster_name = "unittest";
        config.job_factory  = g_jobfactory.get();
        config.ability.categories.insert(TestJob2::kJobName);
        config.ability.categories.insert(TestJob1::kJobName);
        config.ability.categories.insert(TestDailyJob::kJobName);

        g_job_service_1 = ElasticJobService::Create(config);
        bool ret             = g_job_service_1->Start();
        ASSERT_TRUE(ret) << "Start service 1 failed";
        ASSERT_TRUE(g_job_service_1);

        config.host_port = 7889;
        g_job_service_2  = ElasticJobService::Create(config);
        ret              = g_job_service_2->Start();
        ASSERT_TRUE(ret) << "Start service 2 failed";
        ASSERT_TRUE(g_job_service_2);

        g_client = ElasticJobClient::Create("unittest");

        std::this_thread::sleep_for(std::chrono::seconds(10));
        ASSERT_TRUE(g_client);
    }

    static void TearDownTestCase()
    {
        std::this_thread::sleep_for(std::chrono::seconds(10));
        g_client.reset();

        g_job_service_1->Stop();
        g_job_service_2->Stop();

        g_job_service_1.reset();
        g_job_service_2.reset();
    }
};

TEST_F(ElasticJobServiceUnittestBase, TestOnceJob)
{
    JobInfo job_info;
    job_info.category = TestJob1::kJobName;
    job_info.param    = "abc";

    std::string err;
    EXPECT_TRUE(g_client->AddJobInfo(job_info, &err)) << err;

    std::this_thread::sleep_for(std::chrono::seconds(30000));

    JobResult result;
    EXPECT_TRUE(g_client->GetJobResult(result, job_info.id, &err)) << err;
    EXPECT_TRUE(!result.err_code) << result.err_code;
}

TEST_F(ElasticJobServiceUnittestBase, TestRepeatJob)
{
    JobInfo job_info;
    job_info.category      = TestJob2::kJobName;
    job_info.param         = "abc";
    job_info.shedule_type  = JobInfo::SheduleType::Repeat;
    job_info.shedule_param = "1";

    std::string err;
    EXPECT_TRUE(g_client->AddJobInfo(job_info, &err)) << err;

    std::this_thread::sleep_for(std::chrono::seconds(10));

    JobResult result;
    EXPECT_TRUE(g_client->GetJobResult(result, job_info.id, &err)) << err;

    EXPECT_TRUE(g_client->DeleteJobInfo(job_info.id, &err)) << err;
}

TEST_F(ElasticJobServiceUnittestBase, TestDailyJob)
{
    JobInfo job_info;
    job_info.category      = TestDailyJob::kJobName;
    job_info.param         = "abc";
    job_info.shedule_type  = JobInfo::SheduleType::Daily;
    job_info.shedule_param = "13:58:00";

    std::string err;
    EXPECT_TRUE(g_client->AddJobInfo(job_info, &err)) << err;

    std::this_thread::sleep_for(std::chrono::seconds(10));

    JobResult result;
    EXPECT_TRUE(g_client->GetJobResult(result, job_info.id, &err)) << err;

    EXPECT_TRUE(g_client->DeleteJobInfo(job_info.id, &err)) << err;
}
