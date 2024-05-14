set(PARSER_LIB item_set.cpp parser.cpp rule.cpp)

find_package(PkgConfig REQUIRED)

pkg_search_module(Graphviz REQUIRED libgvc)

message("${Graphviz_LIBRARIES}")
message("${Graphviz_INCLUDE_DIRS}")

add_library(parser_lib STATIC ${PARSER_LIB})

target_link_libraries(parser_lib ${Graphviz_LIBRARIES})

target_include_directories(parser_lib PRIVATE ${Graphviz_INCLUDE_DIRS})

target_link_libraries(parser_lib common)

add_executable(parser main.cpp)

target_link_libraries(parser parser_lib)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

