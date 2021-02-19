#ifndef __ELASTIC_JOB_CLIENT_IMPL__
#define __ELASTIC_JOB_CLIENT_IMPL__

#include <memory>
#include <string>
#include <mutex>

#include "../Base/elastic_job.grpc.pb.h"
#include "./ElasticJobClient.h"

#include <grpcpp/channel.h>

namespace elasticjob
{
    class ElasticJobClientImpl : public ElasticJobClient
    {
    public:
        ElasticJobClientImpl(const std::string &cluster_name);
        ~ElasticJobClientImpl() override;

        bool AddJobInfo(JobInfo &job_info, std::string *err = nullptr) override;
        bool DeleteJobInfo(const std::string &job_id, std::string *err = nullptr) override;
        bool DeleteJobInfoByName(const std::string &name, std::string *err = nullptr) override;
        bool GetJobResult(JobResult &job_result, const std::string &job_id, std::string *err = nullptr) override;

    private:
        bool Ping();
        bool DeleteJobInfoHelper(const std::string &job_id, const std::string &name, std::string *err = nullptr);
        std::unique_ptr<elasticjob::proto::ElasticJobService::Stub> GetStub(bool reinit = false);

    private:
        std::string cluster_name_;

        std::mutex lock_;
        std::shared_ptr<grpc::Channel> channel_;
    };
} // namespace elasticjob

#endif