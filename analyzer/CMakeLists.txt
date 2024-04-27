set(ANALYZER_SRC
        ./main.cpp
        ./analyzer.cpp
        ../parser/parser.cpp
        ../parser/rule.cpp
        ../parser/item_set.cpp
    )

add_executable(analyzer ${ANALYZER_SRC})

target_link_libraries(analyzer common)
