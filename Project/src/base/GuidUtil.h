#ifndef __GUID_UTIL_H__
#define __GUID_UTIL_H__

#include <uuid/uuid.h>
#include <string>

namespace base
{
    std::string CreateGuid();
}

#endif