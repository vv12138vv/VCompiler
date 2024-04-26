#aux_source_directory(${CMAKE_CURRENT_LIST_DIR} PARSER_SRC)

set(PARSER_SRC item_set.cpp parser.cpp rule.cpp main.cpp)

add_executable(parser ${PARSER_SRC})

target_link_libraries(parser common)
