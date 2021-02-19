#ifndef __SERVICE_ERROES_H
#define __SERVICE_ERROES_H

namespace elasticjob
{
    struct ServiceErrors
    {
#define DECLARE_ELASTJOB_SERVICE_ERROR(err) static const char k##err[];

        DECLARE_ELASTJOB_SERVICE_ERROR(RequestParamError);
        DECLARE_ELASTJOB_SERVICE_ERROR(WorkerIDIsNotExist);
        DECLARE_ELASTJOB_SERVICE_ERROR(IAmNotLeader);
    };
} // namespace elasticjob

#endif
