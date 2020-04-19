#pragma once

#include "../calyx.h"
#include "../mylua.h"
#include "../calyx.h"
#include "../D3D9Console.h"

#ifdef __cplusplus
extern "C"
{
#endif

    CALYX_EXPORT int init_modules(lua_State *L /*, D3D9Application* app*/);

#ifdef __cplusplus
}
#endif
