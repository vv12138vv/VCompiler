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
    string rules_file_name_;

    Parser(const string& rules_file_name);
    void init();
    void generate_rules(const string& rules_file_name,bool verbos);
    void generate_terminals_and_non_terminals(bool verbose);
    void generate_firsts(bool verbose);

    void set_can_to_nil(const Symbol& non_terminal);
    bool update_non_terminal_first(const Symbol& non_terminal,const Symbol& sym);
    void print_terminals();
    void print_non_terminals();
    void print_rules();
    void print_firsts();
    void print_to_nil();
};




