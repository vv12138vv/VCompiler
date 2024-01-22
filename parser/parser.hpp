#ifndef PARSER_HPP
#define PARSER_HPP

#include<iostream>
#include<string>
#include<unordered_set>
#include<vector>
#include<list>
#include<fstream>

#include"rule.hpp"
#endif

using namespace std;



class Item: public Rule{
public:
    int pointer;
    unordered_set<Symbol,Symbol::Hasher> fronts_;
    bool is_reducible_;
    bool is_movable_;
};

class ItemSet{
public:
    int state;
    unordered_set<Item> items_;
};

class Parser{
public:
    vector<Rule> rules_;
    unordered_set<Symbol,Symbol::Hasher> terminals_;
    unordered_set<Symbol,Symbol::Hasher> non_terminals_;


    void generate_rules(const string& rules_file_name);
    Parser(const string& rules_file_name);
};




