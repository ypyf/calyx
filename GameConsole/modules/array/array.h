#ifndef __calyx_array_module_h__
#define __calyx_array_module_h__

// 这是一个Lua模块的范例

#include "../../calyx.h"

#ifdef __cplusplus
extern "C"
{
#endif

    CALYX_EXPORT int luaopen_array(lua_State* L);

#ifdef __cplusplus
}
#endif

#endif // __calyx_array_module_h__