#include"parser.hpp"
#include"cmdline.h"

//int main(int argc, char *argv[]) {
////    std::string file_path(R"(C:\Users\jgss9\Desktop\VCompiler\parser\test\test_7.txt)");
//    std::string file_path(R"(C:\Users\jgss9\Desktop\VCompiler\parser\rules\grammar_rules.txt)");
//    Parser parser(file_path);
//    parser.call(R"(C:\Users\jgss9\Desktop\VCompiler\cmake-build-debug\lexer\tokens.txt)");
//    return 0;
//}

int main(int argc,char* argv[]){
    cmdline::parser cmd_parser;
    cmd_parser.add<string>("grammar_rule_file",'r',"grammar rules file",true,"");
    cmd_parser.add<string>("tokens_file",'t',"tokens file name", true,"");
    cmd_parser.parse_check(argc,argv);
    string grammar_rule_file=cmd_parser.get<string>("grammar_rule_file");
    string tokens_file=cmd_parser.get<string>("tokens_file");
    Parser parser(grammar_rule_file);
    parser.call(tokens_file);
    return 0;
}