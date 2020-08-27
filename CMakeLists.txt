cmake_minimum_required(VERSION 3.16)
project(vDISK C)

set(CMAKE_C_STANDARD 99)

add_executable(vDISK
        src/vdisk_cli.c src/core/vDISK_macros.h src/core/vDISK_drive.c src/core/vDISK_drive.h src/core/vDISK.c src/core/vDISK.h src/core/vDISK_helpers.c src/core/vDISK_helpers.h)
