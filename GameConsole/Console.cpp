#include "Console.h"
#include <windows.h>

using namespace calyx;

#ifdef OS_WINDOWS
bool Console::InitWindows() {
#if(WINVER >= 0x0605)
    // 改善高分辨率设备的显示效果
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);
#endif // WINVER >= 0x0605
    return true;
}
#endif // OS_WINDOWS

int Console::GetWindowWidth() const {
    return m_windowWidth;

}

int Console::GetHeight() const {
    return m_windowHeight;
}