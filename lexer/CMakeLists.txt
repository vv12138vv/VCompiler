aux_source_directory(${CMAKE_CURRENT_LIST_DIR} LEXER_SRC)

add_executable(lexer ${LEXER_SRC})

target_link_libraries(lexer common)
