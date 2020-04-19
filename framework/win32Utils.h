#pragma once

#include <windows.h>

namespace calyx
{

HICON LoadIconEx(HINSTANCE hInstance, LPCWSTR lpIconName, int iconWidth, int iconHeight);

int MouseScrollDeltaPerLine();
} // namespace calyx
