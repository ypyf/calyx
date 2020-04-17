#include "win32Utils.h"
#include <commctrl.h>

// ¿Ø¼þÍâ¹Û
#pragma comment(lib, "ComCtl32.lib")
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")

HICON LoadIconEx(HINSTANCE hInstance, LPCWSTR lpIconName, int iconWidth, int iconHeight) {
    if (iconWidth == 0)
        iconWidth = ::GetSystemMetrics(SM_CXSMICON);
    if (iconHeight == 0)
        iconHeight = ::GetSystemMetrics(SM_CYSMICON);

    HICON hIcon;
    if (FAILED(::LoadIconWithScaleDown(hInstance, lpIconName, iconWidth, iconHeight, &hIcon))) {
        exit(1);
    }
    return hIcon;
}