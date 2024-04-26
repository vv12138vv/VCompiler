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
    string s=str.substr(1,str.size()-2);
    cout<<s<<'\n';
    s+=',';
    vector<string> parts=Token::split(s,',');
    size_t line= stoi(parts[0]);
    TokenType type=string_to_token_type[parts[1]];
    string value=parts[2];
    return Token(line,type,value);
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

vector<string> Token::split(const string &str,char delimiter) {
    vector<string> res;
    string part;
    istringstream part_stream(str);
    while(getline(part_stream,part,delimiter)){
        res.push_back(part);
    }
    return res;
}

