#include "core.h"

#ifdef OS_WINDOWS
#include <windows.h>
#endif // OS_WINDOWS

extern "C" int luaopen_calyx_core(lua_State * L)
{
    lua_newtable(L);
    lua_setglobal(L, "calyx");

    // Set version information.
    lua_getglobal(L, "calyx");
    lua_pushstring(L, calyx::CALYX_VER);
    lua_setfield(L, -2, "version");

#ifdef OS_WINDOWS
	OSVERSIONINFOEXA os_version;
	ZeroMemory(&os_version, sizeof(OSVERSIONINFOEXA));
	os_version.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXA);
	GetVersionExA((OSVERSIONINFOA*)&os_version);
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
	default:
		break;
	}
	char sb[1024];
	sprintf(sb, "%s (%d.%d) Build %d %s",
			os_name,
		    os_version.dwMajorVersion,
			os_version.dwMinorVersion,
			os_version.dwBuildNumber,
			os_version.szCSDVersion);
	lua_pushstring(L, sb);
#elif defined(OS_MAC)
    lua_pushstring(L, "OSX");
#elif defined(OS_LINUX)
    lua_pushstring(L, "Linux");
#else
    lua_pushstring(L, "Unknown Operating System");
#endif
    lua_setfield(L, -2, "os");

    return 1;
}