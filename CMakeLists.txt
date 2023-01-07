cmake_minimum_required(VERSION 3.24.2)

project("CHIP-8 emulator in C")

add_executable(
    chip_8
    main.c
)

find_package(SDL2 REQUIRED)
target_link_libraries(chip_8 ${SDL2_LIBRARIES})