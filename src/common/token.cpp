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

//Token Token::from_string(const string &str) {
//    string s=str.substr(1,str.size()-2);
//    cout<<s<<'\n';
//    s+=',';
//    vector<string> parts=Token::split(s,',');
//    size_t line= stoi(parts[0]);
//    TokenType type=string_to_token_type[parts[1]];
//    string value=parts[2];
//    return Token(line,type,value);
//}

Token Token::from_string(const string &str) {
    int i=1;
    int j=i;
    while(str[j]!=','){
        j+=1;
    }
    string line=str.substr(i,j-i);
    i=j+1;
    j=i;
    while(str[j]!=','){
        j+=1;
    }
    string type=str.substr(i,j-i);
    i=j+1;
    j=i;
    while(j!=str.size()-1){
        j+=1;
    }
    string content=str.substr(i,j-i);
    size_t line_num=stoi(line);
    TokenType token_type=Token::string_to_token_type[type];
    return {line_num,token_type,content};
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

