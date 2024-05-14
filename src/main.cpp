#include "lexer.hpp"
#include "parser.hpp"
#include "cmdline.h"
#include <unistd.h>

//int main() {
//    string lexer_rule_file(R"(C:\Users\jgss9\Desktop\VCompiler\lexer\rules\lex_rule.txt)");
//    string key_word_file(R"(C:\Users\jgss9\Desktop\VCompiler\lexer\rules\key_words.txt)");
//    Lexer lexer(lexer_rule_file,key_word_file);
//    string grammar_rule_file(R"(C:\Users\jgss9\Desktop\VCompiler\parser\rules\grammar_rules.txt)");
//    string code_file(R"(C:\Users\jgss9\Desktop\VCompiler\lexer\test\test_code.txt)");
//    Parser parser(grammar_rule_file);
//    list<Token> tokens=lexer.analyze(code_file);
//    parser.call(tokens);
//}


int main(int argc,char *argv[]){
    cmdline::parser cmd_parser;
    cmd_parser.add<string>("lexer_rules_file",'l',"lexer rules file name",false,"../rules/lex_rule.txt");
    cmd_parser.add<string>("key_words_file",'k',"keywords file name",false,"../rules/key_words.txt");
    cmd_parser.add<string>("grammar_rules_file",'g',"grammar rules file name", false,"../rules/grammar_rules.txt");
    cmd_parser.add<string>("code_file",'c',"code file name",false,"../code/sql_code.txt");
    cmd_parser.add<string>("tokens_save_file",'t',"tokens file save path",false,"");
    cmd_parser.parse_check(argc,argv);
    string lexer_rules_file=cmd_parser.get<string>("lexer_rules_file");
    string key_words_file=cmd_parser.get<string>("key_words_file");
    string grammar_rules_file=cmd_parser.get<string>("grammar_rules_file");
    string code_file=cmd_parser.get<string>("code_file");
    string tokens_save_file;
    if(cmd_parser.exist("tokens_save_file")){
        tokens_save_file=cmd_parser.get<string>("tokens_save_file");
    }
    Lexer lexer(lexer_rules_file,key_words_file);
    Parser parser(grammar_rules_file);
    list<Token> tokens=lexer.analyze(code_file);
    if(!tokens_save_file.empty()){
        Lexer::save_to(tokens_save_file,tokens);
    }
    parser.call(tokens);

    return 0;
}