#include "modules.h"
#include "array/array.h"
#include "event/event.h"
#include "processing/processing.h"

using namespace calyx;

int init_calyx_graphics(lua_State *L);
int init_calyx_os(lua_State *L);
int init_calyx_mouse(lua_State *L);

namespace
{
// 所有内置模块在此声明
luaL_Reg modules[] = {
    //{ "calyx.event", luaopen_calyx_event },
    {"array", luaopen_array},
    {"processing", luaopen_processing},
    {NULL, NULL}};
} // namespace

int init_modules(lua_State *L)
{
    // 添加模块loader到package.preload表中
    for (size_t i = 0; modules[i].name != NULL; i++)
        lua::preload_module(L, modules[i]);

    // 初始化calyx核心模块

    // 创建calyx表
    lua_newtable(L);
    lua_setglobal(L, "calyx");

    init_calyx_os(L);
    init_calyx_graphics(L);
    init_calyx_mouse(L);

    return 1;
}
