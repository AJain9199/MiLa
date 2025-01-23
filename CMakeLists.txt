cmake_minimum_required(VERSION 3.30)
project(MiLa)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_executable(MiLa src/lexer.cpp main.cpp)
