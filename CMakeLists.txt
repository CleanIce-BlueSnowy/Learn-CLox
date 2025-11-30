cmake_minimum_required(VERSION 3.28)

project(clox)

set(CMAKE_C_STANDARD 23)

add_executable(clox src/main.c)
target_compile_options(clox PUBLIC -Wall -Wextra -O2)
