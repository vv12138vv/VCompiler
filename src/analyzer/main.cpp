#include"analyzer.hpp"
#include"lexer.hpp"
#include"cmdline.h"

int main(int argc,char *argv[]){
    cmdline::parser cmd_parser;
    cmd_parser.add<string>("lexer_rules_file",'l',"lexer rules file name",true,"");
    cmd_parser.add<string>("key_words_file",'k',"keywords file name",true,"");
    cmd_parser.add<string>("grammar_rules_file",'g',"grammar rules file name", true,"");
    cmd_parser.add<string>("code_file",'c',"code file name",true,"");
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
    Analyzer analyzer(grammar_rules_file);
    list<Token> tokens=lexer.analyze(code_file);
    if(!tokens_save_file.empty()){
        Lexer::save_to(tokens_save_file,tokens);
    }
    analyzer.call(tokens);
    return 0;
}
//
//int main(){
//    string rule_file_name(R"(C:\Users\jgss9\Desktop\VCompiler\src\analyzer\rules\rules.txt)");
//    Analyzer analyzer(rule_file_name);
//    string token_file_name(R"(C:\Users\jgss9\Desktop\VCompiler\cmake-build-debug\lexer\tokens.txt)");
//    auto forms=analyzer.call(token_file_name);
//    Analyzer::save_to("./forms.txt",forms);
//    return 0;
//}