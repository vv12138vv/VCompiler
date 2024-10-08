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
    //对应的种类
    ElementType element_type_;
    //对应的转移状态 OR 调用的产生式序号
    int index_;
public:
    Element();
    Element(ElementType element_type,int index);
    static string to_string(const Element& element);
};



class Parser{
public:
    //规则文件名
    string rules_file_name_;
    //生成式
    vector<Rule> rules_;
    //终结符集
    unordered_set<Symbol,Symbol::Hasher> terminals_;
    //存储了非终结符及其first集
    unordered_map<Symbol,unordered_set<Symbol,Symbol::Hasher>,Symbol::Hasher> non_terminals_;
    //项目集
    vector<ItemSet> item_sets_;
    //状态转移表
    unordered_map<int,unordered_map<Symbol,int,Symbol::Hasher>> transfer_;
    //action表
    unordered_map<int,unordered_map<Symbol,Element,Symbol::Hasher>> action_;
    //goto表
    unordered_map<int,unordered_map<Symbol,Element,Symbol::Hasher>> goto_;

    Parser()=default;
    explicit Parser(const string& rules_file_name);
    explicit Parser(vector<Rule>&& rules);
    //初始化Parser
//    void init();
    //初始化生成式
    void generate_rules(const string& rules_file_name,bool verbose);
    //初始化终结符和非终结符
    void generate_terminals_and_non_terminals(bool verbose);
    //生成firsts集
    void generate_firsts(bool verbose);
    //生成项目集自动机
    void generate_DFA(bool verbose);
    //生成LR1状态表
    void generate_LR1(bool verbose);
    //加载词法分析生成的token
    list<Token> load_tokens(const string& token_file_name);
    void analyze(const list<Symbol>& input,const unordered_map<string,const Token&>& sym_token_mp,bool verbose);
    void call(const string& token_file_name);
    void call(const list<Token>& tokens);
    //判断某个非终结符能否退出空
    bool can_to_nil(const Symbol& non_terminal);
    //更新first集
    bool update_first_set(const Symbol& non_terminal,const Symbol& sym);
    int is_existed(const ItemSet& item_set);
    vector<int> find_acc_state();
    //将token转symbol
    pair<list<Symbol>,unordered_map<string,const Token&>> tokens_to_syms(const list<Token>& tokens);
    //制作fake_symbol，用于LR1分析
    Symbol make_fake_symbol(const Symbol& origin,const unordered_map<string,const Token&>& sym_token_mp);
    void print_terminals();
    void print_non_terminals();
    void print_rules();
    void print_firsts();
    void print_to_nil();
    void print_item_sets();
    void print_transfer();
    void print_goto_and_action();
};

#endif
