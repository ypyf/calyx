#include "core.h"

int init_calyx_graphics(lua_State* L);
int init_calyx_os(lua_State* L);

int init_calyx_core(lua_State* L) {
    // ´´½¨calyx±í
    lua_newtable(L);
    lua_setglobal(L, "calyx");

    init_calyx_os(L);
    init_calyx_graphics(L);

    return 0;
}