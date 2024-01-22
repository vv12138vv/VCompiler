#include "parser.hpp"


void Parser::generate_rules(const string &rules_file_name) {
    try {
        ifstream rules_file(rules_file_name);
        if (!rules_file.is_open()) {
            throw runtime_error("Can not open the file!");
        }
        string line;
        while(getline(rules_file,line)){
            Rule rule=Rule::to_rule(static_cast<int>(rules_.size()),line);
            rules_.push_back(std::move(rule));
        }
        rules_file.close();
    } catch (const exception &e) {
        cerr << e.what()<< '\n';
    }
}

Parser::Parser(const string &rules_file_name) {
    generate_rules(rules_file_name);
}


int main(int argc,char* argv[]){
    Parser parser(R"(C:\Users\jgss9\Desktop\classDesign2\Design-2\grammer\test-rule2.txt)");
    for(const auto& rule:parser.rules_){
        rule.print();
    }
    return 0;
}


