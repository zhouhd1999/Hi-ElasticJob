#ifndef __ERRORS_H__
#define __ERRORS_H__

namespace elasticjob
{
#define DECLARE_ELASTJOB_SERVICE_CLIENT_ERROR(err) static const char k##err[];

    struct ClientErrors
    {
        DECLARE_ELASTJOB_SERVICE_CLIENT_ERROR(RequestParamError)
        DECLARE_ELASTJOB_SERVICE_CLIENT_ERROR(WorkerIDIsNotExist)
        DECLARE_ELASTJOB_SERVICE_CLIENT_ERROR(IAmNotLeader)
        DECLARE_ELASTJOB_SERVICE_CLIENT_ERROR(DBError)
        DECLARE_ELASTJOB_SERVICE_CLIENT_ERROR(JobNotFound)
    };
} // namespace elasticjob

#endif
