#include "D3D9Console.h"

#pragma comment(linker, "/subsystem:\"console\" /entry:\"wWinMainCRTStartup\"")

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    using namespace calyx;
    SetCurrentDirectory(TEXT("../Test"));
    D3D9Console console;
    if (!console.Init(hInstance))
    {
        MessageBox(NULL, TEXT("Failed to initialize application"), NULL, NULL);
    }
    console.Run();
}
