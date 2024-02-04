#include"token.hpp"

unordered_map<TokenType,string> Token::token_type_to_string={
        {TokenType::Constant,"constant"},
        {TokenType::Delimiter,"delimiter"},
        {TokenType::Operator,"operator"},
        {TokenType::Keyword,"keyword"},
        {TokenType::Identifier,"identifier"}
};

unordered_map<string,TokenType> Token::string_to_token_type={
        {"constant",TokenType::Constant},
        {"delimiter",TokenType::Delimiter},
        {"operator",TokenType::Operator},
        {"keyword",TokenType::Keyword},
        {"identifier",TokenType::Identifier},
};

Token Token::from_string(const string &str) {
    int i = 0;
    while (str[i] != '(') {
        i += 1;
    }
    i+=1;
    int j = i;
    while (str[j] != ',') {
        j += 1;
    };
    string s=str.substr(i,j-i);
    size_t line = stoi(s);
    j += 1;
    i = j;
    while (str[i] != ',') {
        i += 1;
    }
    s=str.substr(j,i-j);
    TokenType token_type = string_to_token_type[s];
    i += 1;
    j = i;
    while (str[j] != ')') {
        j += 1;
    }
    s=str.substr(i,j-i);
    return Token(line, token_type, s);
}

string Token::to_string(const Token &token) {
    string res;
    res += '(';
    res += std::to_string(token.line_) + ',';
    res += token_type_to_string[token.type_] + ',';
    res += token.value_;
    res += ')';
    return res;
}