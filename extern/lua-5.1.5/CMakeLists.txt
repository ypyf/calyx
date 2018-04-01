cmake_minimum_required (VERSION 2.6)
project (lua)  # project here actually means solution in premake

if(WIN32)
    add_definitions( -D_CRT_SECURE_NO_WARNINGS )
    add_definitions(-D_UNICODE -DUNICODE)
endif()

file(GLOB_RECURSE SOURCES "src/*.c")
file(GLOB_RECURSE HEADERS "src/*.h")

# 1. lua static library
# how to rename library name?
add_library (lualib STATIC ${SOURCES})
set_target_properties(lualib PROPERTIES OUTPUT_NAME "lua") # rename the library output name

# 2. lua interpreter
link_directories (${LUA_BINARY_DIR})
add_executable (lua src/lua.c)
target_link_libraries (lua lualib)

# 3. lua compiler
link_directories (${LUA_BINARY_DIR})
add_executable (luac src/luac.c)
target_link_libraries (luac lualib)