#include"lexer.hpp"
#include"cmdline.h"





//int main(){
//    string rule_file_name(R"(C:\Users\jgss9\Desktop\VCompiler\lexer\rules\lex_rule.txt)");
//    string key_words_file_name(R"(C:\Users\jgss9\Desktop\VCompiler\lexer\rules\key_words.txt)");
//    string code_file_name(R"(C:\Users\jgss9\Desktop\VCompiler\analyzer\test\test_code_1.txt)");
//    Lexer lexer(rule_file_name,key_words_file_name);
//    list<Token> tokens=lexer.analyze(code_file_name);
//    Lexer::save_to("./tokens.txt",tokens);
//    Lexer::print_tokens(tokens);
//    return 0;
//}


int main(int argc,char* argv[]){
    cmdline::parser cmd_parser;
    cmd_parser.add<string>("rule_file_name",'r',"lexer rule file name", true,"");
    cmd_parser.add<string>("key_words_file_name",'k',"key words file name",true,"");
    cmd_parser.add<string>("code_file_name",'c',"code file name",true,"");
    cmd_parser.add<string>("tokens_save_path",'s',"tokens save path",false,"");
    cmd_parser.parse_check(argc,argv);

    string rule_file_name=cmd_parser.get<string>("rule_file_name");
    string key_words_file_name=cmd_parser.get<string>("key_words_file_name");
    string code_file_name=cmd_parser.get<string>("code_file_name");
    string tokens_save_path;
    if(cmd_parser.exist("tokens_save_path")){
        tokens_save_path=cmd_parser.get<string>("tokens_save_path");
    }
    Lexer lexer(rule_file_name,key_words_file_name);
    list<Token> tokens=lexer.analyze(code_file_name);
    Lexer::print_tokens(tokens);
    if(!tokens_save_path.empty()){
        Lexer::save_to(tokens_save_path,tokens);
    }
    return 0;
}