#include "calyx.h"
#include <memory.h>

namespace calyx
{
	const char* CALYX_VER = "0.0.1";

	int luax_preload(lua_State *L, lua_CFunction f, const char *name)
	{
		lua_getglobal(L, "package");
		lua_getfield(L, -1, "preload");
		lua_pushcfunction(L, f);
		lua_setfield(L, -2, name);
		lua_pop(L, 2);
		return 0;
	}

    // 释放userdata对象
    static int userdata_gc_method(lua_State *L) 
    {
        UserData *ud = (UserData*)lua_touserdata(L, 1);
        ud->GC();
        return 0;
    }

    // 创建userdata对象
    int create_userdata(lua_State *L, void *val, size_t size)
    {
        void *ud = lua_newuserdata(L, size);
        // 初始化userdata对象
        memcpy(ud, val, size);
        lua_newtable(L); /* create metatable. */
        lua_pushliteral(L, "__gc"); /* push key '__gc' */
        lua_pushcfunction(L, userdata_gc_method); /* push gc method. */
        lua_rawset(L, -3);    /* metatable['__gc'] = userdata_gc_method */
        lua_setmetatable(L, -2); /* set the userdata's metatable. */
        return 1;
    }
}