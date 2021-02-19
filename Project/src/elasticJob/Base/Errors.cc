#include "Errors.h"

namespace elasticjob
{
#define DEFINE_ELASTJOB_SERVICE_CLIENT_ERROR(err) const char ClientErrors::k##err[] = #err;

    DEFINE_ELASTJOB_SERVICE_CLIENT_ERROR(RequestParamError);
    DEFINE_ELASTJOB_SERVICE_CLIENT_ERROR(WorkerIDIsNotExist)
    DEFINE_ELASTJOB_SERVICE_CLIENT_ERROR(IAmNotLeader)
    DEFINE_ELASTJOB_SERVICE_CLIENT_ERROR(DBError)
    DEFINE_ELASTJOB_SERVICE_CLIENT_ERROR(JobNotFound)
} // namespace elasticjob
