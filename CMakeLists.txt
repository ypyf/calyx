cmake_minimum_required (VERSION 3.11)

project (Caylx)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
message ("cxx Flags:" ${CMAKE_CXX_FLAGS})

# Disable C4828 warning
add_compile_options(/wd4828)

# Enable Utf-8 source files
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

add_definitions(-D_UNICODE -DUNICODE)

add_subdirectory(framework)
