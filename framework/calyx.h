#pragma once

#include <lua.hpp>

// Platform stuff.
#if defined(WIN32) || defined(_WIN32)
#include <windows.h>
#define OS_WINDOWS 1
#define WIN32_LEAN_AND_MEAN
#define CALYX_EXPORT __declspec(dllexport)
#else
#define CALYX_EXPORT
#endif

#if defined(linux) || defined(__linux) || defined(__linux__)
#define OS_LINUX 1
#endif

#if defined(__APPLE__)
#define OS_MAC 1
#endif

namespace calyx
{
extern const char *CALYX_VER;
} // namespace calyx
