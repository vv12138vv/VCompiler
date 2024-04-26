#ifndef RULE_HPP
#define RULE_HPP

#include<iostream>
#include<string>
#include<unordered_set>
#include<vector>
#include<list>
#include"exception.hpp"
#endif

using namespace std;

const char terminal_left_delimiter = '\'';
const char terminal_right_delimiter = '\'';
const char non_terminal_left_delimiter = '\"';
const char non_terminal_right_delimiter = '\"';
const string Nil="@";
const string Front_Search="#";
const string Item_Delimieter="\xA1\xA4";

//二型文法中符号类型
enum class SymbolType {
    Terminal,//对应token
    Non_Terminal,//非终结符
    Nil,//空
    Front,//前向搜索符号
    Error
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

const Symbol NIL(Nil,SymbolType::Nil);
const Symbol FRONT_SEARCH(Front_Search,SymbolType::Front);
const Symbol ERROR("",SymbolType::Error);

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