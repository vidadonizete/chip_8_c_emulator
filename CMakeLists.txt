cmake_minimum_required(VERSION 3.24.2)

project("CHIP-8 emulator in C")

add_executable(
    chip_8
    main.c
)

target_include_directories(chip_8 PRIVATE include)