cmake_minimum_required(VERSION 3.24.2)

project("CHIP-8 emulator in C" LANGUAGES C)

add_executable(chip_8 main.c)

target_include_directories(chip_8 PRIVATE include/)

#BEGIN SDL2_UI_IMPL

find_package(SDL2 REQUIRED)

add_library(sdl2_ui STATIC sdl2_ui.c)

target_link_libraries(sdl2_ui ${SDL2_LIBRARIES})

target_include_directories(sdl2_ui PRIVATE include/)

target_link_libraries(chip_8 sdl2_ui)

#END SDL2_UI_IMPL