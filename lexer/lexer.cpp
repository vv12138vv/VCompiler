#include "lexer.hpp"

string Lexer::preprocessing(const string& file_name){
    string code;
    size_t line=1;
    string processed_code;
    try{
        ifstream code_file(file_name);
        if(!code_file.is_open()){
            throw runtime_error("Can not open the file!");
        }
        while(code_file.peek()!=EOF){//读取代码文件
            code.push_back(static_cast<char>(code_file.peek()));
            code_file.ignore();
        }
        size_t i=0;
        while(i<=code.length()-1){
            if(code[i]=='/'&&code[i+1]=='/'){//单行注释
                while(code[i]!='\n'){
                    i+=1;
                }
            }else{
                processed_code.push_back(code[i]);
                if(code[i]=='\n'){
                    line+=1;
                }
                i+=1;
            }

        }
        code_file.close();
    }catch (const exception& e){
        cerr<<e.what()<<'\n';
    }
    while(code.find('\t')!=string::npos){
        code.erase(code.find('\t'),1);
    }
    return processed_code;
}

Lexer::Lexer(const string &rules_file_name):rules_file_name_(rules_file_name) {
    auto_machines_.insert({LabelType::Operator, make_unique<AutoMachine>(rules_file_name,LabelType::Operator)});
    auto_machines_.insert({LabelType::Delimiter, make_unique<AutoMachine>(rules_file_name,LabelType::Delimiter)});
    auto_machines_.insert({LabelType::Identifier, make_unique<AutoMachine>(rules_file_name,LabelType::Identifier)});
    auto_machines_.insert({LabelType::Scientific, make_unique<AutoMachine>(rules_file_name,LabelType::Scientific)});
}

//list<Token> Lexer::analyze(const string &file_name) {
//    list<Token> tokens;
//    string code= preprocessing(file_name);
//    size_t line=1;
//    istringstream code_stream(code);
//    while(code_stream.peek()!=EOF){
//        int ch=code_stream.peek();
//        if(ch=='\n'){
//            line+=1;
//        }
//        string part;
//        code_stream>>part;
//        cout<<part<<'\n';
//        int len=-1;
//        len=auto_machines_[LabelType::Identifier]->analyze(part);
//        if(len!=-1&&len!=0){
//            string token=part.substr(0,len);
//            if(is_key_word(token)){
//                Token new_token(line,TokenType::Keyword,token);
//                tokens.push_back(new_token);
//            }else{
//                Token new_token(line,TokenType::Identifier,token);
//            }
////            code_stream.ignore(len-1);
//            continue;
//        }
//        len=auto_machines_[LabelType::Delimiter]->analyze(part);
//        if(len!=-1&&len!=0){
//            string token=part.substr(0,len);
//            Token new_token(line,TokenType::Delimiter,token);
//            tokens.push_back(new_token);
////            code_stream.ignore(len-1);
//            continue;
//        }
//        len=auto_machines_[LabelType::Operator]->analyze(part);
//        if(len!=-1&&len!=0){
//            string token=part.substr(0,len);
//            Token new_token(line,TokenType::Operator,token);
//            tokens.push_back(new_token);
////            code_stream.ignore(len-1);
//            continue;
//        }
//        len=auto_machines_[LabelType::Scientific]->analyze(part);
//        if(len!=-1&&len!=0) {
//            string token = part.substr(0, len);
//            Token new_token(line, TokenType::Constant, token);
//            tokens.push_back(new_token);
////            code_stream.ignore(len-1);
//            continue;
//        }
//    }
//    return tokens;
//}

list<Token> Lexer::analyze(const std::string &file_name) {
    list<Token> tokens;
    string code= std::move(preprocessing(file_name));
    size_t line=1;
    int i=0;
    while(code[i]!=0){
        char ch=static_cast<char>(code[i]);
        if(ch=='\n'){
            line+=1;
            i+=1;
            continue;
        }
        if(ch==' '){
            i+=1;
            continue;
        }
        int j= find_word_end(code,i);
        string part=code.substr(i,j-i+1);
        int len=-1;
        len=auto_machines_[LabelType::Identifier]->analyze(part);
        if(len!=-1&&len!=0){
            string token=part.substr(0,len);
            if(is_key_word(token)){
                Token new_token(line,TokenType::Keyword,token);
                tokens.push_back(new_token);
            }else{
                Token new_token(line,TokenType::Identifier,token);
                tokens.push_back(new_token);
            }
            i=i+len;
            continue;
        }
        len=auto_machines_[LabelType::Delimiter]->analyze(part);
        if(len!=-1&&len!=0){
            string token=part.substr(0,len);
            Token new_token(line,TokenType::Delimiter,token);
            tokens.push_back(new_token);
            i=i+len;
            continue;
        }
        len=auto_machines_[LabelType::Operator]->analyze(part);
        if(len!=-1&&len!=0){
            string token=part.substr(0,len);
            Token new_token(line,TokenType::Operator,token);
            tokens.push_back(new_token);
            i=i+len;
            continue;
        }
        len=auto_machines_[LabelType::Scientific]->analyze(part);//todo 科学计数法的识别有问题
        if(len!=-1&&len!=0) {
            string token = part.substr(0, len);
            Token new_token(line, TokenType::Constant, token);
            tokens.push_back(new_token);
            i=i+len;
            continue;
        }
    }
    return tokens;
}

bool Lexer::is_key_word(const string &token) const {
    if(key_words.count(token)==0){
        return false;
    }
    return true;
}

int Lexer::find_word_end(const string &str, int i) {
    int j=i;
    int n=static_cast<int>(str.size());
    while(str[j]!=' '&&j<n){
        j+=1;
    }
    j-=1;
    return j;
}

int main(int args,char* argv[]){
    Lexer lexer(R"(C:\Users\jgss9\Desktop\VCompiler\lexer\lex-rule.txt)");
//    lexer.auto_machines_[LabelType::Identifier]->print_content();
//    lexer.auto_machines_[LabelType::Scientific]->print_content();
    list<Token> tokens=lexer.analyze(R"(C:\Users\jgss9\Desktop\VCompiler\lexer\test_code.txt)");
    cout<<"tokens_count:"<<tokens.size()<<'\n';
    for(const auto& token:tokens){
        cout<<'('<<token.line_<<','<<token_type_to_string[token.type_]<<','<<token.value_<<")\n";
    }

    return 0;
}
