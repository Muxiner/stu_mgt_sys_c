cmake_minimum_required(VERSION 3.10)
project(StudentMS C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(include)

add_executable(studentms
    src/*.c
)
