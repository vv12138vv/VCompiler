#ifndef PARSER_HPP
#define PARSER_HPP

#include<iostream>
#include<string>
#include<unordered_set>
#include<vector>
#include<list>
#include<fstream>
#include<unordered_map>
#include<stack>
#include<tuple>

#include"item_set.hpp"
#include"../lexer/token.hpp"

#endif

using namespace std;

//LR表中的元素类型
enum class ElementType{
    Error,
    Accept,
    Goto,
    Move,
    Reduce
};

//LR表中的元素
class Element{
public:
    ElementType element_type_;
    int index_;//对应的转移状态 / 调用的产生式序号
public:
    Element();
    Element(ElementType element_type,int index);
    static string to_string(const Element& element);
};



class Parser{
public:
    vector<Rule> rules_;
    unordered_set<Symbol,Symbol::Hasher> terminals_;
    //存储了非终结符及其first集
    unordered_map<Symbol,unordered_set<Symbol,Symbol::Hasher>,Symbol::Hasher> non_terminals_;
    string rules_file_name_;

    vector<ItemSet> item_sets_;
    //状态转移表
    unordered_map<int,unordered_map<Symbol,int,Symbol::Hasher>> transfer_;
    //action表
    unordered_map<int,unordered_map<Symbol,Element,Symbol::Hasher>> action_;
    //goto表
    unordered_map<int,unordered_map<Symbol,Element,Symbol::Hasher>> goto_;

    explicit Parser(const string& rules_file_name);
    void init();

    void generate_rules(const string& rules_file_name,bool verbose);
    void generate_terminals_and_non_terminals(bool verbose);
    void generate_firsts(bool verbose);
    void generate_DFA(bool verbose);
    void generate_LR1(bool verbose);
    list<Token> load_tokens(const string& token_file_name);
    void analyze(const list<Symbol>& input,bool verbose);
    void call(const string& token_file_name);
    bool can_to_nil(const Symbol& non_terminal);
    bool update_first_set(const Symbol& non_terminal,const Symbol& sym);
    int is_existed(const ItemSet& item_set);
    vector<int> find_acc_state();
    tuple<list<Symbol>,unordered_map<Symbol*,Token>> tokens_to_syms(const list<Token>& tokens);
    void print_terminals();
    void print_non_terminals();
    void print_rules();
    void print_firsts();
    void print_to_nil();
    void print_item_sets();
    void print_transfer();
    void print_goto_and_action();
};




