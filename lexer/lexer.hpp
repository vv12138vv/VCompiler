#include<iostream>
#include<vector>
#include<list>
#include<string>
#include<unordered_map>
#include<unordered_set>

#include<fstream>
//using
using namespace std;

//表示空
const char Nil='@';

class DFAState {
    //状态记录
    vector<int> states_;
    //可以转移到的状态
    unordered_map<char, int> to_;
};

typedef unordered_map<int, DFAState> DFA;
typedef unordered_map<int, unordered_map<char, vector<int>>> NFA;
typedef pair<string,string> Rule;

enum class TokenType {
    Constant,
    Delimiter,
    Operand,
    Keyword,
    Identifier
};

enum class LabelType {
    Scientific,
    Delimiter,
    Operator,
    Identifier,
};
unordered_map<LabelType, string> label_mp{
    {LabelType::Delimiter, "[delimiter]"},
    {LabelType::Scientific,"[scientific]"},
    {LabelType::Operator,"[operator]"},
    {LabelType::Identifier,"[identifier]"}
};

class Token {
public:
    size_t line_;
    TokenType type_;
    string value_;
    Token(size_t line, TokenType type, string value) : line_(line), type_(type), value_(std::move(value)) {}
};


class AutoMachine {
private:
    string rules_file_name_;
    LabelType label_type_;

    list<Rule> rules_;
    vector<char> alphabets_;
    NFA nfa_;
    DFA dfa_;

public:
    static list<string> read_rules(const string& rules_file_name, const string& label);

    void generate_rules(const list<string>& rules_string);
    void init_rules();

    AutoMachine(string rules_file_name,LabelType label_type);
};