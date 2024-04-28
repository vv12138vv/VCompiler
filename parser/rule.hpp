#ifndef RULE_HPP
#define RULE_HPP

#include<iostream>
#include<string>
#include<unordered_set>
#include<unordered_map>
#include<vector>
#include<list>
#include"token.hpp"
#include"exception.hpp"


using namespace std;

extern const char terminal_left_delimiter ;
extern const char terminal_right_delimiter;
extern const char non_terminal_left_delimiter;
extern const char non_terminal_right_delimiter;
extern const string Nil;
extern const string Front_Search;
extern const string Item_Delimiter;

//二型文法中符号类型
enum class SymbolType {
    Terminal,//对应token
    Non_Terminal,//非终结符
    Nil,//空
    Front,//前向搜索符号
    Error//初始化状态，即错误
};
//符号的抽象
class Symbol {
public:
    struct Hasher {
        size_t operator()(const Symbol &symbol) const {
            return hash<string>()(symbol.content_);
        }
    };
public:
    string content_;
    SymbolType symbol_type_;
    Symbol()=delete;
    Symbol(string content, SymbolType symbol_type);
    Symbol(const Symbol &that)=default;
    Symbol &operator=(const Symbol &that) = default;
    bool operator==(const Symbol &that) const;
};

extern const Symbol NIL;
extern const Symbol FRONT_SEARCH;
extern const Symbol ERROR;

//产生式
class Rule {
public:
    int index_;//规则序号，用于LR1时指定规约规则
    string content_;
    Symbol left_;//左部
    vector<Symbol> right_;//右部

    Rule()=delete;
    Rule(int index, Symbol &&left, vector<Symbol> &&right);
    Rule(const Rule& that)=default;
    Rule& operator=(const Rule& that)=default;
    bool is_nil_rule() const;
    //计算右部的first集
    Symbol get_right_first() const;
    //在字符串和Rule之间进行转换
    static Rule to_rule(int index, const string &rule_string);
    static string to_string(const Rule &rule);
    void print() const;
};
#endif
