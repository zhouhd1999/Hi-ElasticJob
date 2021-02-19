#include "ServiceErrors.h"

namespace elasticjob
{
#define DEFINE_ELASTJOB_SERVICE_ERROR(err) const char ServiceErrors::k##err[] = #err;

    DEFINE_ELASTJOB_SERVICE_ERROR(RequestParamError)
    DEFINE_ELASTJOB_SERVICE_ERROR(WorkerIDIsNotExist)
    DEFINE_ELASTJOB_SERVICE_ERROR(IAmNotLeader)
} // namespace elasticjob