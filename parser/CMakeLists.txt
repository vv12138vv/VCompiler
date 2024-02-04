add_executable(parser
        parser.hpp parser.cpp
        rule.hpp rule.cpp
        item_set.hpp item_set.cpp
        ../lexer/token.hpp ../lexer/token.cpp
)

target_link_libraries(parser common)
