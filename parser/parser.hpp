#ifndef PARSER_HPP
#define PARSER_HPP

#include<iostream>
#include<string>
#include<unordered_set>
#include<vector>
#include<list>
#include<fstream>
#include<unordered_map>

#include"item.hpp"
#endif

using namespace std;

const char FRONT_SEARCH='#';


class Parser{
public:
    vector<Rule> rules_;
//    unordered_set<Symbol,Symbol::Hasher> terminals_;
//    unordered_set<Symbol,Symbol::Hasher> non_terminals_;
    unordered_set<Symbol,Symbol::Hasher> terminals_;
    unordered_map<Symbol,unordered_set<Symbol,Symbol::Hasher>,Symbol::Hasher> non_terminals_;
    string rules_file_name_;
    list<ItemSet> item_sets_;

    Parser(const string& rules_file_name);
    void init();

    void generate_rules(const string& rules_file_name,bool verbos);
    void generate_terminals_and_non_terminals(bool verbose);
    void generate_firsts(bool verbose);
    void generate_LR1(bool verbose);

    bool can_to_nil(const Symbol& non_terminal);
    bool update_first_set(const Symbol& non_terminal,const Symbol& sym);
    bool update_non_terminal_first(const Symbol& non_terminal,const Symbol& sym);
    void print_terminals();
    void print_non_terminals();
    void print_rules();
    void print_firsts();
    void print_to_nil();
};




