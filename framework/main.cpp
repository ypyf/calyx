#include "D3D9Console.h"

using namespace calyx;

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    SetCurrentDirectory(TEXT("../Test"));
    D3D9Console console;
    if (!console.Init(hInstance))
    {
        MessageBox(NULL, TEXT("Failed to initialize application"), NULL, NULL);
        exit(1);
    }
    console.Run();
}
