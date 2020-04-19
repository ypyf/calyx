# Calyx

A Lua 3D Game Engine

## Prerequisites

1. DirectX 9 SDK installed

2. Visual Sutdio 2015 (or higher) installed

3. CMake installed and added the `bin` directory to `PATH`

## Building

### On Windows

1. Generate Visual Sutdio solution for `x86` platform

    ```shell
    cmake -G "Visual Studio 16 2019" -S . -A Win32
    ```

2. Buil it for `Debug`

    ```shell
    cmake  --build . --config Debug
    ```

3. Run it

    ```shell
    Debug\calyx.exe
    ```
