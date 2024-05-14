#ifndef TOKEN_HPP
#define TOKEN_HPP

#include<iostream>
#include<string>
#include<unordered_map>
#include<unordered_set>
#include<vector>
#include<sstream>

using namespace std;

//枚举用于标识Token的类型
enum class TokenType {
    Constant,
    Delimiter,
    Operator,
    Keyword,
    Identifier
};

//Token类
class Token {
public:
    //标识Token行号
    size_t line_;
    //标识Token类型
    TokenType type_;
    //标识Token值
    string value_;
    Token()=delete;
    Token(size_t line, TokenType type, string value) : line_(line), type_(type), value_(std::move(value)) {}
    //一些字符串与TokenType进行转换的函数
    static unordered_map<TokenType,string> token_type_to_string;
    static unordered_map<string,TokenType> string_to_token_type;
    //由string转Token
    static Token from_string(const string& str);
    //由Token转string
    static string to_string(const Token& token);
    static vector<string> split(const string& str,char delimiter);
};

#endif
