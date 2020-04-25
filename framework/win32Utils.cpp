#include "win32Utils.h"
#include <commctrl.h>

// 控件外观
#pragma comment(lib, "ComCtl32.lib")
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")

namespace calyx
{

// 加载图标
HICON LoadIconEx(HINSTANCE hInstance, LPCWSTR lpIconName, int iconWidth, int iconHeight)
{
    if (iconWidth == 0)
        iconWidth = ::GetSystemMetrics(SM_CXSMICON);
    if (iconHeight == 0)
        iconHeight = ::GetSystemMetrics(SM_CYSMICON);

    HICON hIcon;
    if (FAILED(::LoadIconWithScaleDown(hInstance, lpIconName, iconWidth, iconHeight, &hIcon)))
    {
        return nullptr;
    }
    return hIcon;
}

// 计算鼠标滚轮滚动一行的距离
int MouseScrollDeltaPerLine()
{
    int scrolllines;
    ::SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &scrolllines, 0);
    return scrolllines ? WHEEL_DELTA / scrolllines : 0;
}

} // namespace calyx
