#include"analyzer.hpp"

int main(){
    string rule_file_name(R"(C:\Users\jgss9\Desktop\VCompiler\analyzer\rules\rules.txt)");
    Analyzer analyzer(rule_file_name);
    string token_file_name(R"(C:\Users\jgss9\Desktop\VCompiler\cmake-build-debug\lexer\tokens.txt)");
    analyzer.call(token_file_name);
}