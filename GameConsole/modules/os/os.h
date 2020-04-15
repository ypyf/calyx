#ifndef __calyx_os_modules_h__
#define __calyx_os_modules_h__

#include "../../calyx.h"

#ifdef __cplusplus
extern "C"
{
#endif

    CALYX_EXPORT int luaopen_calyx_os(lua_State* L);

#ifdef __cplusplus
}
#endif

#endif // __calyx_os_modules_h__