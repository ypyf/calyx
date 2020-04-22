#include "D3D9Console.h"

using namespace calyx;

int main(int argc, char *argv[])
{
    SetCurrentDirectory(TEXT("../Test"));
    D3D9Console console;
    if (!console.Init())
    {
        MessageBox(NULL, TEXT("Failed to initialize application"), NULL, NULL);
        exit(1);
    }
    console.Run();
}
