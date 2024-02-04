add_executable(lexer
        auto_machine.hpp auto_machine.cpp
        lexer.hpp lexer.cpp
        token.hpp
        token.cpp)

target_link_libraries(lexer common)
