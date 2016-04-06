cmake_minimum_required(VERSION 3.3)
project(TP-Thread)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")

set(SOURCE_FILES main.c question1.c question2.c question3.c question5.c)
add_executable(Tp-Thread ${SOURCE_FILES})