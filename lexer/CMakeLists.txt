#aux_source_directory(${CMAKE_CURRENT_LIST_DIR} LEXER_SRC)

set(LEXER_SRC auto_machine.cpp lexer.cpp main.cpp)

add_executable(lexer ${LEXER_SRC})

target_link_libraries(lexer common)
