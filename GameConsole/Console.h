#pragma once

#include "calyx.h"

class Console {
public:
#ifdef OS_WINDOWS
    bool InitWindows();
#endif
};
