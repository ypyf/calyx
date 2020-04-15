#ifndef __calyx_h__
#define __calyx_h__

#include <lua.hpp>

// Platform stuff.
#if defined(WIN32) || defined(_WIN32)
#	define OS_WINDOWS 1
#endif

#if defined(linux) || defined(__linux) || defined(__linux__)
#	define OS_LINUX 1
#endif

#if defined(__APPLE__)
#	define OS_MAC 1
#endif

// DLL-stuff.
#ifdef OS_WINDOWS
#	define CALYX_EXPORT __declspec(dllexport)
#else
#	define CALYX_EXPORT
#endif

namespace calyx
{
    extern const char* CALYX_VER;
} // namespace calyx

#endif // __calyx_h__