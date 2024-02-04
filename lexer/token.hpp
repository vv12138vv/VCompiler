#ifndef TOKEN_HPP
#define TOKEN_HPP

#include<iostream>
#include<string>
#include<unordered_map>
#include<unordered_set>

#endif
using namespace std;

enum class TokenType {
    Constant,
    Delimiter,
    Operator,
    Keyword,
    Identifier
};


//extern const unordered_map<TokenType,string> token_type_to_string;
//extern const unordered_map<string,TokenType> string_to_token_type;


class Token {
public:
    size_t line_;
    TokenType type_;
    string value_;
    Token()=delete;
    Token(size_t line, TokenType type, string value) : line_(line), type_(type), value_(std::move(value)) {}
    static unordered_map<TokenType,string> token_type_to_string;
    static unordered_map<string,TokenType> string_to_token_type;
    static Token from_string(const string& str);
    static string to_string(const Token& token);
};