#include "modules.h"
#include "array/array.h"
#include "event/event.h"
#include "processing/processing.h"
#include "core/core.h"

using namespace calyx;

namespace {
    // ��������ģ���ڴ�����
    luaL_Reg modules[] = {
        //{ "calyx.event", luaopen_calyx_event },
        {"array", luaopen_array},
        {"processing", luaopen_processing},
        {NULL, NULL}
    };
}

int init_modules(lua_State* L)
{
    // ���ģ��loader��package.preload����
    for (size_t i = 0; modules[i].name != NULL; i++)
        lua::preload_module(L, modules[i]);

    // ��ʼ��calyx����
    init_calyx_core(L);

    return 1;
}