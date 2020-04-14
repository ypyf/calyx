#include "modules.h"

#include "modules/core/core.h"
#include "modules/array/array.h"
#include "modules/event/event.h"
#include "modules/processing/processing.h"

using namespace calyx;

// 所有内置模块在此声明
static luaL_Reg modules[] = {
    //{ "calyx.event", luaopen_calyx_event },
    {"calyx.core", luaopen_calyx_core},
    {"calyx.array", luaopen_calyx_array},
    {"calyx.processing", luaopen_calyx_processing},
    {NULL, NULL}
};


int init_modules(lua_State* L)
{
    // 添加模块loader到package.preload表中
    for (size_t i = 0; modules[i].name != NULL; i++)
        luax_preload_module(L, modules[i]);

    luax_require_module(L, "calyx.core");

    return 1;
}