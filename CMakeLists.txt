cmake_minimum_required(VERSION 3.26)
project(VCompiler)

set(CMAKE_CXX_STANDARD 17)

include_directories(
        ${CMAKE_CURRENT_LIST_DIR}/common
        ${CMAKE_CURRENT_LIST_DIR}/lexer
        ${CMAKE_CURRENT_LIST_DIR}/parser
        ${CMAKE_CURRENT_LIST_DIR}/analyzer
)

add_subdirectory(common)
add_subdirectory(lexer)
add_subdirectory(parser)
add_subdirectory(analyzer)



add_executable(VCompiler ${CMAKE_CURRENT_LIST_DIR}/main.cpp)

target_link_libraries(VCompiler lexer_lib parser_lib)