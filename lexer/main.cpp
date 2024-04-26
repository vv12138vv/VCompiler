#include"lexer.hpp"

int main(int args, char *argv[]) {
    if (args < 4) {
        return 0;
    }

    string rules_file_name = argv[1];
    string key_words_file_name = argv[2];
    string code_file_name = argv[3];
    Lexer lexer(rules_file_name, key_words_file_name);
    list<Token> tokens = lexer.analyze(code_file_name);
    Lexer::save_to("./tokens.txt", tokens);
    Lexer::print_tokens(tokens);
    return 0;
}