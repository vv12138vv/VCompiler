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
const string Nil = "@";

enum class SymbolType {
    Ternimal,
    Non_Terminal,
    Nil,
    Front,
};

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

class Rule {
public:
    int index_;
    string content_;
    Symbol left_;
    vector<Symbol> right_;

    Rule()=delete;
    Rule(int index, Symbol &&left, vector<Symbol> &&right);
    Rule(const Rule& that)=default;
    Rule& operator=(const Rule& that)=default;

    static Rule to_rule(int index, const string &rule_string);
    Symbol get_right_first() const;
    static string to_string(const Rule &rule);
    void print() const;
};