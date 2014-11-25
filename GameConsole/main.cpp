//////////////////////////////////////
// Entry Point
//////////////////////////////////////
#include "Game.h"

// 控件外观
//#pragma comment(lib, "ComCtl32.lib")
//#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")

#pragma comment(linker, "/subsystem:\"console\" /entry:\"wWinMainCRTStartup\"")

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    using namespace calyx;
    SetCurrentDirectory(TEXT("../Test"));
    D3D9Console console;
    if (console.Init(hInstance))
        console.Run();
}
