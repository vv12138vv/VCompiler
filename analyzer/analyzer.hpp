#ifndef ANALYZER_HPP
#define ANALYZER_HPP
#include"parser.hpp"

#endif

enum class ActionType {
    Add,
    Assign,
    Subtract,
    Multiply
};

class Action {
public:
    Symbol left_op_;
    Symbol right_op_;
    ActionType action_type_;

    Action() = delete;

    Action(Symbol left_op, Symbol right_op, ActionType action_type) : left_op_(left_op), right_op_(right_op),
                                                                      action_type_(action_type) {

    }
};



class Analyzer {
    using Senmatic =unordered_map<int,vector<Action>>;//每个rule的idx对应的语义映射
private:
    Parser parser;
    Senmatic senmatics;
public:
    Analyzer() = delete;
    Analyzer(const string &rule_file_name);
    auto generate_rules(const string& rule_file_name);
};


