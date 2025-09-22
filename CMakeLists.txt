# By default a static library will be produced, if a shared library is desired, use: 
# "cmake -S . -B build -DCTOOLBOX_BUILD_SHARED=ON"
# "cmake --build build"
# and define CTOOLBOX_USE_SHARED on your application, like:
# "target_compile_definitions(myapp PRIVATE CTOOLBOX_USE_SHARED)"


cmake_minimum_required(VERSION 3.15)
project(ctoolbox C)

# user option to control library type
option(CTOOLBOX_BUILD_SHARED "Build ctoolbox as a shared library" OFF)

if(CTOOLBOX_BUILD_SHARED)
    add_library(ctoolbox SHARED darray.c)
    target_compile_definitions(ctoolbox PRIVATE CTOOLBOX_BUILD_SHARED)
else()
    add_library(ctoolbox STATIC darray.c)
endif()

# add include directory for public headers
target_include_directories(ctoolbox PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
