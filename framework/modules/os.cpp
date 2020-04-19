#include "../mylua.h"
#include "../calyx.h"
#include "../D3D9Console.h"

#ifdef OS_WINDOWS
#include <windows.h>
#endif // OS_WINDOWS

using namespace calyx;

int init_calyx_os(lua_State *L)
{
    // Set version information.
    lua_getglobal(L, "calyx");
    lua_pushstring(L, calyx::CALYX_VER);
    lua_setfield(L, -2, "version");

#ifdef OS_WINDOWS
    OSVERSIONINFOEXA os_version;
    ZeroMemory(&os_version, sizeof(OSVERSIONINFOEXA));
    os_version.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXA);
    GetVersionExA((OSVERSIONINFOA *)&os_version);
    char os_name[128] = "Windows";
    switch (os_version.dwMajorVersion)
    {
    case 5:
        switch (os_version.dwMinorVersion)
        {
        case 0:
            break;
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        default:
            break;
        }
    case 6:
        switch (os_version.dwMinorVersion)
        {
        case 0:
            break;
        case 1:
            strcpy(os_name, "Windows 7");
            break;
        case 2:
            break;
        case 3:
            break;
        default:
            break;
        }
    case 10:
        if (os_version.wProductType == VER_NT_WORKSTATION)
        {
            strcpy(os_name, "Windows 10");
        }
        else
        {
            strcpy(os_name, "Windows Server 2016");
        }
    default:
        break;
    }

    lua_pushstring(L, os_name);

#elif defined(OS_MAC)
    lua_pushstring(L, "OSX");
#elif defined(OS_LINUX)
    lua_pushstring(L, "Linux");
#else
    lua_pushstring(L, "Unknown Operating System");
#endif

    lua_setfield(L, -2, "os");

    lua_pop(L, 1);

    return TRUE;
}
