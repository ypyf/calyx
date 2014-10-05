#include "modules.h"

#include "modules/core/core.h"
#include "modules/array/array.h"
#include "modules/event/event.h"
#include "modules/processing/processing.h"
#include "Game.h"

using namespace calyx;

static luaL_Reg modules[] = {
	//{ "calyx.event", luaopen_calyx_event },
	{"calyx.core", luaopen_calyx_core},
	{"calyx.array", luaopen_calyx_array},
	{"calyx.processing", luaopen_calyx_processing},
	{NULL, NULL}
};


int init_modules(lua_State *L/*, D3D9Application* app*/)
{
    // 添加模块载入程序到表package.preload
    luax_preload_modules(L, modules);
    luax_require_module(L, "calyx.core");   // 载入核心模块
	return 1;
}
