
include_directories(.)

set(SOURCES "main.cpp" "calyx.rc" "calyx.cpp" "Console.cpp" "D3D9Console.cpp" "GameTime.cpp" "MatrixStack.cpp"
    "mylua.cpp" "MyModels.cpp" "stringUtils.cpp" "TexCube.cpp" "win32Utils.cpp")

 set(MODULE_SOURCES "modules/modules.cpp" "modules/modules.h"
    "modules/array/array.cpp" "modules/array/array.h"
    "modules/event/event.cpp" "modules/event/event.h"
    "modules/processing/processing.cpp" "modules/processing/processing.h"
    "modules/graphics.cpp" "modules/mouse.cpp" "modules/os.cpp")

# DirectX SDK
include_directories("D:/Dev/Microsoft DirectX SDK (June 2010)/Include")
link_directories("D:/Dev/Microsoft DirectX SDK (June 2010)/Lib/x86")

# Lua library
include_directories(${CMAKE_SOURCE_DIR}/include)
link_directories(${CMAKE_SOURCE_DIR}/lib)

add_executable(calyx WIN32 ${SOURCES} ${MODULE_SOURCES})

target_link_libraries (calyx lua51 d3d9 d3dx9)

set_property(TARGET calyx PROPERTY FOLDER "executables")
set_target_properties(calyx PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

install (TARGETS calyx
         RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/_install)
