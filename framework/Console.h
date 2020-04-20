#pragma once

#include "calyx.h"

namespace calyx
{

class Console
{
public:
#ifdef OS_WINDOWS
    bool InitOS();
#endif
};
} // namespace calyx
