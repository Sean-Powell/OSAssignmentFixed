cmake_minimum_required(VERSION 3.9)
project(OSAssingment C)

set(CMAKE_C_STANDARD 99)

add_executable(OSAssingment main.c linenoise/linenoise.c main.c variables.c variables.h)