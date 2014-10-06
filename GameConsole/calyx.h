#ifndef __calyx_h__
#define __calyx_h__

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

#include <lua.hpp>

typedef int (*LuaGlueFunction)(lua_State*L);

namespace calyx
{
    struct UserData
    {
        virtual void GC()=0;
    };

	extern const char* CALYX_VER;

	int luax_preload_module(lua_State *L, luaL_Reg module);
    int luax_require_module(lua_State *L, const char *module);
    int luaopen_printf(lua_State *L);
    int create_userdata(lua_State *L, void *val, size_t size);

} // namespace calyx

#endif // __calyx_h__