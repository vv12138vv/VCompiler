set(LEXER_LIB auto_machine.cpp lexer.cpp)


add_library(lexer_lib STATIC ${LEXER_LIB})

target_link_libraries(lexer_lib common)


add_executable(lexer main.cpp)

target_link_libraries(lexer lexer_lib)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
