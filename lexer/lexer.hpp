#ifndef LEXER_HPP
#define LEXER_HPP


#include<memory>


#include "auto_machine.hpp"
#include"token.hpp"

#endif






class Lexer {
private:
    string rules_file_name_;
    string key_words_file_name;
    unordered_set<string> key_words_;
public:
    //词法分析自动机
    unordered_map<LabelType, unique_ptr<AutoMachine>> auto_machines_;
    bool is_key_word(const string& token) const;
    Lexer() = delete;
    Lexer(const string &rules_file_name,const string& key_words_file_name);
    //预处理代码文件
    string preprocessing(const string &file_name);
    //分析代码，返回token序列
    list<Token> analyze(const string &file_name);
    //寻找单词的末尾
    static int find_word_end(const string& str,int i);
    void read_key_words(const string& file_name);
    list<Token> other_analyze(const string& file_name);
    //存储token序列
    static void save_to(const string& file_name,const list<Token>& tokens);
};


string trim(const string& str);

