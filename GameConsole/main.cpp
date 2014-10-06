//////////////////////////////////////
// Entry Point
//////////////////////////////////////
#include "Game.h"

using namespace std;
using namespace calyx;

// 控件外观
#pragma comment(lib, "ComCtl32.lib")
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(linker, "/subsystem:\"console\" /entry:\"wWinMainCRTStartup\"")

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	D3D9Application game;
	if (!game.Init(hInstance))
		return 1;
	game.Run();
    return 0;
}