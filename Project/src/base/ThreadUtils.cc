#include "ThreadUtils.h"
#include "muduo/base/Logging.h"

#include <sys/prctl.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>

namespace base {

    void SetThreadName(const std::string& name)
    {
        int err = prctl(PR_SET_NAME, name.c_str());
        if (err < 0 && errno != EPERM)
        {
            LOG_INFO << " Set thread(" << getpid() << ") name(" << name << ") failed";
        }
    }
}
