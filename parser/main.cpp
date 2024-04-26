#include"parser.hpp"

int main(int argc, char *argv[]) {
//    std::string file_path(R"(C:\Users\jgss9\Desktop\VCompiler\parser\test\test_2.txt)");
    std::string file_path(R"(C:\Users\jgss9\Desktop\VCompiler\parser\rules\grammar_rules.txt)");
    Parser parser(file_path);
    parser.call(R"(C:\Users\jgss9\Desktop\VCompiler\cmake-build-debug\lexer\tokens.txt)");
    return 0;
}