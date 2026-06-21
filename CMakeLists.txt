cmake_minimum_required(VERSION 3.10)
project(StudentMS C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(studentms
    src/main.c
    src/menu.c
    src/student.c
    src/input.c
    src/fileio.c
    src/display.c
)
