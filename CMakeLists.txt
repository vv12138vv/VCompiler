cmake_minimum_required(VERSION 3.26)
project(VCompiler)

set(CMAKE_CXX_STANDARD 14)

include_directories(lexer)

add_executable(VCompiler
        lexer/lexer.cpp
        lexer/lexer.hpp)
