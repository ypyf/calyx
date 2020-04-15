#include "mylua.h"
#include <memory.h>

namespace {

    // 释放userdata对象
    int userdata_gc(lua_State* L)
    {
        lua::UserData* ud = (lua::UserData*)lua_touserdata(L, 1);
        ud->GC();
        return 0;
    }


    int l_printf(lua_State* L)
    {
        lua_pushvalue(L, lua_upvalueindex(2));  // retrieve string.format
        lua_insert(L, 1);
        lua_call(L, lua_gettop(L) - 1, 1);
        lua_pushvalue(L, lua_upvalueindex(1));  // retrieve io.write
        lua_pushvalue(L, -2);
        lua_call(L, 1, 0);
        return 0;
    }
}

namespace lua {
    // 创建userdata对象
    int create_userdata(lua_State* L, void* val, size_t size)
    {
        void* ud = lua_newuserdata(L, size);
        // 初始化userdata对象
        memcpy(ud, val, size);
        lua_newtable(L); /* create metatable. */
        lua_pushliteral(L, "__gc"); /* push key '__gc' */
        lua_pushcfunction(L, userdata_gc); /* push gc method. */
        lua_rawset(L, -3);    /* metatable['__gc'] = userdata_gc_method */
        lua_setmetatable(L, -2); /* set the userdata's metatable. */
        return 1;
    }

    int preload_module(lua_State* L, luaL_Reg module)
    {
        // 添加模块载入函数到表package.preload
        lua_getglobal(L, "package");
        lua_getfield(L, -1, "preload");
        lua_pushcfunction(L, module.func);
        lua_setfield(L, -2, module.name);
        lua_pop(L, 2);
        return 1;
    }

    // require 'xxx'
    int require_module(lua_State* L, const char* module)
    {
        lua_getglobal(L, "require");
        lua_pushstring(L, module);
        return lua_pcall(L, 1, 0, 0);
    }


    // register lua function 'printf'
    int register_printf(lua_State* L)
    {
        lua_getglobal(L, "io");
        lua_getglobal(L, "string");
        lua_pushliteral(L, "write");
        lua_gettable(L, -3);    // push io.write
        lua_pushliteral(L, "format");
        lua_gettable(L, -3);    // push string.format
        lua_pushcclosure(L, l_printf, 2);
        /* With 5.1, I'd probably just return 1 at this point */
        lua_setglobal(L, "printf");
        return 0;
    }

} // namespace lua