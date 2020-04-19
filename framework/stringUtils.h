#pragma once

#include "calyx.h"
#include <string>

#ifndef UNICODE
typedef std::string cstring;
#else
typedef std::wstring cstring;
#endif

#ifdef OS_WINDOWS
wchar_t *ansi_to_unicode(const char *pcMbcs);
wchar_t *utf8_to_unicode(const char *pcMbcs);
#endif // OS_WINDOWS
