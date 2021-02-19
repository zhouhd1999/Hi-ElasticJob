#ifndef __LOG_HELPER_H__
#define __LOG_HELPER_H__

#include <iosfwd>
#include "ElasticJobTypeDefs.h"

std::ostream& operator<<(std::ostream& stream, const elasticjob::JobInfo& param);

std::ostream& operator<<(std::ostream& stream, const elasticjob::JobRunParam& param);

std::ostream& operator<<(std::ostream& stream, const elasticjob::JobResult::Status& s);

std::ostream& operator<<(std::ostream& stream, const elasticjob::JobResult& s);

#endif
