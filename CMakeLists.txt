cmake_minimum_required(VERSION 3.18.4)
project(CHIP8-EMULATOR)

set (SOURCE_FILES src/main.c src/chip8.c )
add_executable(chip8emu ${SOURCE_FILES})

