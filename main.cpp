#include "lexer.hpp"
#include "parser.hpp"


int main() {
    string lexer_rule_file(R"(C:\Users\jgss9\Desktop\VCompiler\lexer\rules\lex_rule.txt)");
    string key_word_file(R"(C:\Users\jgss9\Desktop\VCompiler\lexer\rules\key_words.txt)");
    Lexer lexer(lexer_rule_file,key_word_file);
    string grammar_rule_file(R"(C:\Users\jgss9\Desktop\VCompiler\parser\rules\grammar_rules.txt)");
    string code_file(R"(C:\Users\jgss9\Desktop\VCompiler\lexer\test\test_code.txt)");
    Parser parser(grammar_rule_file);
    list<Token> tokens=lexer.analyze(code_file);
    parser.call(tokens);
}