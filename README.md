# Calyx

A Lua 3D Game Engine

## Prerequisites

1. DirectX 9 SDK

2. Visual Sutdio 2015 (or higher)

3. CMake and add the `bin` directory to `PATH`

## Building

### On Windows

1. Generate Visual Sutdio solution for `x86` platform

    ```shell
    cmake -G "Visual Studio 16 2019" -S . -A Win32
    ```

2. Build it for `Debug`

    ```shell
    cmake  --build . --config Debug
    ```

3. Run it

    ```shell
    Debug\calyx.exe
    ```
