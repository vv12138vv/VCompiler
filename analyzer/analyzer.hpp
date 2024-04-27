#ifndef ANALYZER_HPP
#define ANALYZER_HPP
#include"parser.hpp"
#include<memory>
#endif

enum class ActionType {
    Add,
    Assign,
    Subtract,
    Multiply,
    Print
};


//从简单开始，每个符号只有一个属性即val

class Action {
public:
    static const unordered_set<char> ops;
    Symbol left_op_;
    Symbol right_op_;
    Symbol res_;
    ActionType action_type_;

    Action() = delete;

    Action(Symbol left_op, Symbol right_op, Symbol res_,ActionType action_type);
    static vector<Action> to_actions(const string& str);
};


class Analyzer {
    using Semantic =unordered_map<int,vector<Action>>;//每个rule的idx对应的语义映射
private:
    std::unique_ptr<Parser> parser_;
    Semantic semantics_;
public:
    Analyzer() = delete;
    Analyzer(const string &rule_file_name);
    auto generate_rules(const string& rule_file_name);
    //查找左操作数
    static Symbol find_left_op(const string& str,int i);
    //查找右操作数
    static Symbol find_right_op(const string& str,int i);

    static Symbol find_print_op(const string& str);

    void analyze(const list<Symbol>& syms,const unordered_map<string,const Token&>& sym_token_mp,bool verbose);
    void call(const string& token_file_name);
};


