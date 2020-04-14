#ifndef __calyx_modules_h__
#define __calyx_modules_h__

#include "Calyx.h"

#ifdef __cplusplus
extern "C"
{
#endif

    CALYX_EXPORT int init_modules(lua_State* L/*, D3D9Application* app*/);

#ifdef __cplusplus
}
#endif

#endif // __calyx_modules_h__