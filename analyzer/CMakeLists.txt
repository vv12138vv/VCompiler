set(ANALYZE_LIB analyzer.cpp)

add_library(analyzer_lib ${ANALYZE_LIB})

target_link_libraries(analyzer_lib parser_lib)

add_executable(analyzer main.cpp)

target_link_libraries(analyzer analyzer_lib)