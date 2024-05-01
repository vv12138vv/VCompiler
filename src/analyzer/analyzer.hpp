#ifndef ANALYZER_HPP
#define ANALYZER_HPP

#include"parser.hpp"
#include<memory>


//对应的语义栈操作类型
enum class ActionType {
    Add,
    Init,//区分赋值和初始化
    Assign,
    Multiply,
    Subtract,
    Bracket,
    Print,
};

//四元式
class Form {
public:
    string op_;
    string op1_;
    string op2_;
    string res_;
    Form(const string &op, const string &op1, const string &op2, const string &res) : op_(op), op1_(op1), op2_(op2),
                                                                                      res_(res) {}
    static string to_string(const Form& form);
};

//语义分析器
class Analyzer {
    using Semantic = unordered_map<int, ActionType>;//每个rule的idx对应的语义映射
private:
    std::unique_ptr<Parser> parser_;
    Semantic semantics_;
public:
    Analyzer() = delete;
    Analyzer(const string &rule_file_name);
    //读取语法规则
    auto generate_rules(const string &rule_file_name);
    //读取语义规则
    auto generate_actions(const string &rule_file_name);
    //生成语法和语义的映射
    auto generate_semantics(const vector<ActionType> &actions);
    //将属性文法转为具体的语义动作
    ActionType to_action(const string &str);
    //对给定tokens流进行分析
    vector<Form> analyze(const list<Symbol> &syms, const unordered_map<string, const Token &> &sym_token_mp, bool verbose);
    vector<Form> call(const string &token_file_name);
    vector<Form> call(const list<Token>& tokens);
    //保存生产的四元式
    static void save_to(const string& file_name,const vector<Form>& forms);
    //打印四元式
    static void print_forms(const vector<Form>& forms);
};

#endif

