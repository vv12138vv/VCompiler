#ifndef ANALYZER_HPP
#define ANALYZER_HPP
#include"parser.hpp"
#include<memory>
#endif

enum class ActionType {
    Add,
    Init,//区分赋值和初始化
    Assign,
    Multiply,
    Subtract,
    Bracket,
    Print,
};




class Analyzer {
    using Semantic =unordered_map<int,ActionType>;//每个rule的idx对应的语义映射
private:
    std::unique_ptr<Parser> parser_;
    Semantic semantics_;
public:
    Analyzer() = delete;
    Analyzer(const string &rule_file_name);
    //读取语法规则
    auto generate_rules(const string& rule_file_name);
    //读取语义规则
    auto generate_actions(const string& rule_file_name);
    //生成语法和语义的映射
    auto generate_semantics(const vector<ActionType>& actions);

    //查找左操作数
    Symbol find_left_op(const string& str,int i);
    //查找右操作数
    Symbol find_right_op(const string& str,int i);

    Symbol find_print_op(const string& str);
    ActionType to_action(const string &str);
    void analyze(const list<Symbol>& syms,const unordered_map<string,const Token&>& sym_token_mp,bool verbose);
    void call(const string& token_file_name);
};


