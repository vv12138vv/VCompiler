#include<iostream>
#include<unordered_map>
#include<vector>
#include<string>
#include<fstream>
//using
using namespace std;

class DFAState {
    //状态记录
    vector<int> states;
    //可以转移到的状态
    unordered_map<char, int> to;
};


typedef unordered_map<int, DFAState> DFA;
typedef unordered_map<int, unordered_map<char, vector<int>>> NFA;

enum TokenType{
    Constant,
    Delimiter,
    Operand,
    Keyword,
    Identifier
};

class AutoMachine{
private:
    vector<string> rules;
public:
    vector<string> read_rules(string file_name,string label);
    AutoMachine(string rules_file_name);
};