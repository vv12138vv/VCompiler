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
    unordered_map<LabelType, unique_ptr<AutoMachine>> auto_machines_;
    bool is_key_word(const string& token) const;
    Lexer() = delete;
    Lexer(const string &rules_file_name,const string& key_words_file_name);
    string preprocessing(const string &file_name);
    list<Token> analyze(const string &file_name);
    static int find_word_end(const string& str,int i);
    void read_key_words(const string& file_name);
    list<Token> other_analyze(const string& file_name);
    static void save_to(const string& file_name,const list<Token>& tokens);
};


string trim(const string& str);

