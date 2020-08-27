cmake_minimum_required(VERSION 3.16)
project(vdisk_cli C)

set(CMAKE_C_STANDARD 99)

add_executable(vdisk_cli
        src/vdisk_cli.c src/lib/vDISK_macros.h src/lib/vDISK_drive.c src/lib/vDISK_drive.h src/lib/vDISK.h src/lib/vDISK_utility.c src/lib/vDISK_utility.h src/lib/vDISK_fat16.c src/lib/vDISK_fat16.h)

target_link_libraries(vdisk_cli PRIVATE m)