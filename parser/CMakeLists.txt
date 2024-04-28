set(PARSER_LIB item_set.cpp parser.cpp rule.cpp)

add_library(parser_lib STATIC ${PARSER_LIB})

target_link_libraries(parser_lib common)

add_executable(parser main.cpp)

target_link_libraries(parser parser_lib)


