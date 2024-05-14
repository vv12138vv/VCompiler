cmake_minimum_required(VERSION 3.26)
project(VCompiler)

set(CMAKE_CXX_STANDARD 17)



include_directories(
        src/common
        src/lexer
        src/parser
)

add_subdirectory(src/common)
add_subdirectory(src/lexer)
add_subdirectory(src/parser)

add_compile_options(-std=c++17)

add_executable(VCompiler src/main.cpp)

target_link_libraries(VCompiler lexer_lib parser_lib)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)