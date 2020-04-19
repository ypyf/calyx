#pragma once

#include <lua.hpp>

typedef int (*LuaGlueFunction)(lua_State *L);

namespace lua
{

struct UserData
{
    virtual void GC() = 0;
};

void show_error_message(lua_State *L);
int preload_module(lua_State *L, luaL_Reg module);
int require_module(lua_State *L, const char *module);
int register_printf(lua_State *L);
int create_userdata(lua_State *L, void *val, size_t size);

} // namespace lua
