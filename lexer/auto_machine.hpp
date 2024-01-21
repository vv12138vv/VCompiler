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

#endif
using namespace std;
//using

//表示空
const char Nil = '@';
class DFAState {
public:
    bool is_existed_{};
    //集合内容
    unordered_set<int> states_;
    //可以转移到的状态
    unordered_map<char, int> to_;
public:
    DFAState(bool is_existed,unordered_set<int> states):is_existed_(is_existed),states_(std::move(states)){
    }
    DFAState(){}
    DFAState(const DFAState& that){
        this->is_existed_=that.is_existed_;
        this->states_=that.states_;
        this->to_=that.to_;
    }
    DFAState& operator=(const DFAState& that)= default;
    DFAState& operator=(DFAState&& that) noexcept {
        this->is_existed_=that.is_existed_;
        this->states_=std::move(that.states_);
        this->to_=std::move(that.to_);
        return *this;
    }
};

typedef unordered_map<int, DFAState> DFA;
typedef unordered_map<int, unordered_map<char, unordered_set<int>>> NFA;
typedef pair<string, string> Rule;


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
    list<Rule> rules_;
    //字母表
    unordered_set<char> alphabets_;
    //终结符集合
    unordered_set<char> terminals_;
    //非终结符集合
    unordered_set<char> non_terminals_;
    //非终结符id映射
    unordered_map<char,int> non_terminals_index_;

    unordered_set<int> nfa_end_states_;
    unordered_set<int> dfa_end_states_;
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

public:
    static list<string> read_rules(const string &rules_file_name, const string &label);

    void generate_rules(const list<string> &rules_string);
    void generate_terminals_and_non_terminals();
    void init();
    void make_nfa(int end);
    void make_dfa();
    int is_existed_state(const unordered_set<int>& states);
    unordered_set<int> move(const DFAState& dfa_state,char ch);
    unordered_set<int> get_e_closure(const unordered_set<int>& t);
    AutoMachine(const string& rules_file_name, LabelType label_type);
    void print_dfa();
    void print_nfa();
    void print_alphabets();
    void print_terminals();
    void print_non_terminals();
    void print_content();
    int move_to(char ch,int cur_state);
    int analyze(const string& part);
    unordered_set<int> find_end_dfa_state();
};
