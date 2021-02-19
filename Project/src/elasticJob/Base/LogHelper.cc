#include "LogHelper.h"

#include <ostream>

std::ostream& operator<<(std::ostream& stream, const elasticjob::JobInfo& param)
{
    stream << "Job(id=" << param.id << ", name=" << param.name << ", category=" << param.category
           << ", param=" << param.param << ")";
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const elasticjob::JobRunParam& param)
{
    stream << "RunParam(id=" << param.id << ", category=" << param.category << ", params=" << param.param << ")";
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const elasticjob::JobResult::Status& s)
{
    using Status = elasticjob::JobResult::Status;

    switch (s)
    {
    case Status::Pending:
        stream << "Pending";
        break;
    case Status::Doing:
        stream << "Doing";
        break;
    case Status::Succ:
        stream << "Succ";
        break;
    case Status::Failed:
        stream << "Failed";
        break;
    default:
        assert(0);
        break;
    }

    return stream;
}

std::ostream& operator<<(std::ostream& stream, const elasticjob::JobResult& s)
{
    stream << "JobResult(id=" << s.id << ", job_id=" << s.job_id << ", status=" << s.status << ", err=" << s.err_code
           << ")";
    return stream;
}
