#ifndef __ELASTIC_JOB_SERVICE__
#define __ELASTIC_JOB_SERVICE__

#include "../Base/ElasticJobTypeDefs.h"


class CTimer;

namespace elasticjob {

    class ElasticJobService {
    public:
        static std::unique_ptr<ElasticJobService> Create(const Configuration& config);
        virtual ~ElasticJobService() {}

        virtual bool Start() = 0;
        virtual bool Stop() = 0;
    };

}

#endif
