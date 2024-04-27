#include"lexer.hpp"

//int main(int args, char *argv[]) {
//    if (args < 4) {
//        return 0;
//    }
//
//    string rules_file_name = argv[1];
//    string key_words_file_name = argv[2];
//    string code_file_name = argv[3];
//    Lexer lexer(rules_file_name, key_words_file_name);
//    list<Token> tokens = lexer.analyze(code_file_name);
//    Lexer::save_to("./tokens.txt", tokens);
//    Lexer::print_tokens(tokens);
//    return 0;
//}

int main(){
    string rule_file_name(R"(C:\Users\jgss9\Desktop\VCompiler\lexer\test\lex-rule.txt)");
    string key_words_file_name(R"(C:\Users\jgss9\Desktop\VCompiler\lexer\test\key_words.txt)");
    //test parser
//    string code_file_name(R"(C:\Users\jgss9\Desktop\VCompiler\lexer\test\test_code.txt)");
    //test analyzer
    string code_file_name(R"(C:\Users\jgss9\Desktop\VCompiler\analyzer\test\test_code_1.txt)");
    Lexer lexer(rule_file_name,key_words_file_name);
    list<Token> tokens=lexer.analyze(code_file_name);
    Lexer::save_to("./tokens.txt",tokens);
    Lexer::print_tokens(tokens);
    return 0;
}