#include "modules.h"

#include "modules/core/core.h"
#include "modules/array/array.h"
#include "modules/event/event.h"
#include "modules/processing/processing.h"

static const luaL_Reg modules[] = {
	//{ "calyx.event", luaopen_calyx_event },
	{ "calyx.core", luaopen_calyx_core },
	{ "calyx.array", luaopen_calyx_array },
	{"calyx.processing", luaopen_calyx_processing},
	{
		NULL, NULL
	}
};


int preload_builtin_modules(lua_State *L)
{
	for (int i = 0; modules[i].name != 0; i++)
	{
		calyx::luax_preload(L, modules[i].func, modules[i].name);
	}

	// ÔØÈëºËÐÄÄ£¿é
	lua_getglobal(L, "require");
	lua_pushstring(L, "calyx.core");
	lua_call(L, 1, 0);
	return 1;
}
