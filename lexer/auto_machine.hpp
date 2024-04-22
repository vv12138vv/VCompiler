#ifndef AUTO_MACHINE_HPP
#define AUTO_MACHINE_HPP

#include<iostream>
#include<vector>
#include<list>
#include<string>
#include<unordered_map>
#include<unordered_set>
#include<algorithm>
#include<fstream>
#include<queue>
#include<iomanip>

#include"../common/exception.hpp"

#endif
using namespace std;

const char Nil = '@';

//DFA中的每个状态
class DFAState {
public:
    //标识在进行NFA转DFA时，是否已经考虑过
    bool is_existed_;
    //该DFA对于原NFA的状态有哪些
    unordered_set<int> states_;
    //可以转移到的状态
    unordered_map<char, int> to_;
public:
    DFAState(bool is_existed, unordered_set<int> states) : is_existed_(is_existed), states_(std::move(states)) {
    }

    DFAState() {}

    DFAState(const DFAState &that) {
        this->is_existed_ = that.is_existed_;
        this->states_ = that.states_;
        this->to_ = that.to_;
    }

    DFAState &operator=(const DFAState &that) = default;

    DFAState &operator=(DFAState &&that) noexcept {
        this->is_existed_ = that.is_existed_;
        this->states_ = std::move(that.states_);
        this->to_ = std::move(that.to_);
        return *this;
    }
};


using DFA = unordered_map<int, DFAState>;
using NFAState=unordered_map<char,unordered_set<int>>;
using NFA = unordered_map<int, NFAState>;
using Rule_ = pair<string, string>;

//文件输入中不同类别产生式的Tag
enum class LabelType {
    Scientific,
    Delimiter,
    Operator,
    Identifier,
};

static unordered_map<LabelType, string> label_mp{
        {LabelType::Delimiter,  "[delimiter]"},
        {LabelType::Scientific, "[scientific]"},
        {LabelType::Operator,   "[operator]"},
        {LabelType::Identifier, "[identifier]"}
};


const vector<char> upper_case_letters = {
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
        'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'
};
const vector<char> lower_case_letters = {
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
        'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'
};

class AutoMachine {
private:
    //规则文件路径
    string rules_file_name_;
    //自动机类型
    LabelType label_type_;
    //规则
    list<Rule_> rules_;
    //字母表
    unordered_set<char> alphabets_;
    //终结符集合
    unordered_set<char> terminals_;
    //非终结符集合
    unordered_set<char> non_terminals_;
    //非终结符id映射
    unordered_map<char, int> non_terminals_index_;
    //nfa的结束状态
    unordered_set<int> nfa_end_states_;
    //nfa状态总数
    int nfa_state_count_;
    //dfa状态总数
    int dfa_state_count_;
    //nfa初始状态
    int nfa_start_index_;
    //nfa状态表
    NFA nfa_;
    //dfa状态表
    DFA dfa_;
    //初始化一个自动机
    void init();
public:
    //dfa的结束状态
    unordered_set<int> dfa_end_states_;
    //读取规则文件
    static list<string> read_rules(const string &rules_file_name, const string &label);
    //生成rules_的内容
    void generate_rules(const list<string> &rules_string);
    //生成终结符与非终结符
    void generate_terminals_and_non_terminals();
    //生成nfa
    void make_nfa(int end);
    //根据nfa生成dfa
    void make_dfa();
    //判断某个DFAState是否已存在
    int is_existed_state(const unordered_set<int> &states);
    //求解某个DFA状态可以转移到的状态
    unordered_set<int> move(const DFAState &dfa_state, char ch);
    //子集法闭包
    unordered_set<int> get_e_closure(const unordered_set<int> &t);
    AutoMachine(const string &rules_file_name, LabelType label_type);
    //返回DFA可转移到的下一状态
    int move_to(char ch, int cur_state);
    //对输出串进行自动机分析
    int analyze(const string &part);
    unordered_set<int> find_end_dfa_state();

    //以下是一些输出函数
    void print_dfa();
    void print_nfa();
    void print_alphabets();
    void print_terminals();
    void print_non_terminals();
    void print_content();
};
