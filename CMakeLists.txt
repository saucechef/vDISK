cmake_minimum_required(VERSION 3.16)
project(vDISK C)

set(CMAKE_C_STANDARD 99)

add_executable(vDISK
        src/vdisk.c src/core/vDISK_macros.h src/core/vDISK_structs.h src/core/vDISK_core.c src/core/vDISK_core.h)
