#ifndef LEXER_HPP
#define LEXER_HPP

#include "auto_machine.hpp"
#include<memory>

#endif

enum class TokenType {
    Constant,
    Delimiter,
    Operator,
    Keyword,
    Identifier
};
unordered_map<TokenType,string> token_type_to_string={
        {TokenType::Constant,"constant"},
        {TokenType::Delimiter,"delimiter"},
        {TokenType::Operator,"operator"},
        {TokenType::Keyword,"keyword"},
        {TokenType::Identifier,"identifier"}
};

class Token {
public:
    size_t line_;
    TokenType type_;
    string value_;
    Token(size_t line, TokenType type, string value) : line_(line), type_(type), value_(std::move(value)) {}
};




class Lexer {
private:
    string rules_file_name_;
public:
    unordered_map<LabelType, unique_ptr<AutoMachine>> auto_machines_;

    unordered_set<string> key_words = {
            "i32", "i64", "f32", "return",
            "let", "fn", "if", "while", "break","else","continue"
    };

    bool is_key_word(const string& token) const;
    Lexer() = delete;

    Lexer(const string &rules_file_name);

    string preprocessing(const string &file_name);
    list<Token> analyze(const string &file_name);
    int find_word_end(const string& str,int i);
};


