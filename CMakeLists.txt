cmake_minimum_required(VERSION 3.29)
project(EnseaShell C)

set(CMAKE_C_STANDARD 11)

include_directories(inc)

add_executable(EnseaShell src/main.c
        src/io.c)