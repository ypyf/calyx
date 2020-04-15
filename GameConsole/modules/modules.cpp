#include "modules.h"
#include "../mylua.h"
#include "os/os.h"
#include "array/array.h"
#include "event/event.h"
#include "graphics/graphics.h"
#include "processing/processing.h"

using namespace calyx;

namespace {
    // 所有内置模块在此声明
    luaL_Reg modules[] = {
        //{ "calyx.event", luaopen_calyx_event },
        {"calyx.os", luaopen_calyx_os},
        {"calyx.array", luaopen_calyx_array},
        {"calyx.processing", luaopen_calyx_processing},
        {"calyx.graphics", luaopen_calyx_graphics},
        {NULL, NULL}
    };
}

int init_modules(lua_State* L)
{
    // 添加模块loader到package.preload表中
    for (size_t i = 0; modules[i].name != NULL; i++)
        lua::preload_module(L, modules[i]);

    // 创建calyx表
    lua_newtable(L);
    lua_setglobal(L, "calyx");

    lua::require_module(L, "calyx.os");
    lua::require_module(L, "calyx.graphics");

    return 1;
}