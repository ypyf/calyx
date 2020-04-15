/*
 * calyx.graphics 模块提供了图形相关API
 *
 */

#ifndef CALYX_MODULE_GRAPHICS_H
#define CALYX_MODULE_GRAPHICS_H

#include "../../calyx.h"

#ifdef __cplusplus
extern "C"
{
#endif

    CALYX_EXPORT int luaopen_calyx_graphics(lua_State* L);

#ifdef __cplusplus
}
#endif

#endif // CALYX_MODULE_GRAPHICS_H