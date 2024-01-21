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
    unordered_map<LabelType, unique_ptr<AutoMachine>> auto_machines_;
public:

    unordered_set<string> key_words = {
            "i32", "i64", "f32", "return",
            "let", "fn", "if", "while", "break","else","continue"
    };

    bool is_key_word(const string& token);
    Lexer() = delete;

    Lexer(const string &rules_file_name);

    string preprocessing(const string &file_name);

    list<Token> analyze(const string &file_name);
};


