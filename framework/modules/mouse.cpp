#include "../mylua.h"
#include "../calyx.h"
#include "../D3D9Console.h"

using namespace calyx;

namespace
{
const struct luaL_Reg exports[] =
    {
        {NULL, NULL}};
}

int init_calyx_mouse(lua_State *L)
{
    // calyx.graphics = {}
    lua_getglobal(L, "calyx");
    lua_newtable(L);
    lua_setfield(L, 1, "mouse");

    // load calyx.graphics
    lua_getfield(L, -2, "mouse");

    for (size_t i = 0; exports[i].name != NULL; i++)
    {
        lua_pushcfunction(L, exports[i].func);
        lua_setfield(L, -2, exports[i].name);
    }

    lua_pop(L, 2);

    return TRUE;
}
