#pragma once

#include "calyx.h"

namespace calyx {

    class Console {
    public:
#ifdef OS_WINDOWS
        bool InitWindows();
#endif

        int GetWindowWidth() const;
        int GetHeight() const;
    protected:
        int m_windowWidth, m_windowHeight;
    };
} // namespace calyx