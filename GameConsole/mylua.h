#ifndef MYLUA_H
#define MYLUA_H

#include <lua.hpp>

typedef int (*LuaGlueFunction)(lua_State* L);

namespace lua {
    
    struct UserData
    {
        virtual void GC() = 0;
    };

    int preload_module(lua_State* L, luaL_Reg module);
    int require_module(lua_State* L, const char* module);
    int register_printf(lua_State* L);
    int create_userdata(lua_State* L, void* val, size_t size);
}

#endif // MYLUA_H