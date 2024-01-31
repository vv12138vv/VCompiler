#ifndef PARSER_HPP
#define PARSER_HPP

#include<iostream>
#include<string>
#include<unordered_set>
#include<vector>
#include<list>
#include<fstream>
#include<unordered_map>

#include"item_set.hpp"
#endif

using namespace std;


enum class ElementType{
    Default,
    Accept,
    Goto,
    Move,
    Reduce
};

class Element{
public:
    ElementType element_type_;
    int index_;
public:
    Element();
    Element(ElementType element_type,int index);
    static string to_string(const Element& elemen);
};



class Parser{
public:
    vector<Rule> rules_;
    unordered_set<Symbol,Symbol::Hasher> terminals_;
    unordered_map<Symbol,unordered_set<Symbol,Symbol::Hasher>,Symbol::Hasher> non_terminals_;
    string rules_file_name_;
    vector<ItemSet> item_sets_;
    unordered_map<int,unordered_map<Symbol,int,Symbol::Hasher>> transfer_;
    unordered_map<int,unordered_map<Symbol,Element,Symbol::Hasher>> action_;
    unordered_map<int,unordered_map<Symbol,Element,Symbol::Hasher>> goto_;

    Parser(const string& rules_file_name);
    void init();

    void generate_rules(const string& rules_file_name,bool verbose);
    void generate_terminals_and_non_terminals(bool verbose);
    void generate_firsts(bool verbose);
    void generate_DFA(bool verbose);
    void generate_LR1(bool verbose);

    bool can_to_nil(const Symbol& non_terminal);
    bool update_first_set(const Symbol& non_terminal,const Symbol& sym);
    int is_existed(const ItemSet& item_set);
    vector<int> find_acc_state();
    void print_terminals();
    void print_non_terminals();
    void print_rules();
    void print_firsts();
    void print_to_nil();
    void print_item_sets();
    void print_transfer();
    void print_goto_and_action();
};




