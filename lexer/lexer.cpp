#include "lexer.hpp"

string Lexer::preprocessing(const string &file_name) {
    string code;
    size_t line = 1;
    string processed_code;
    try {
        ifstream code_file(file_name);
        if (!code_file.is_open()) {
            throw runtime_error("Can not open the file!");
        }
        while (code_file.peek() != EOF) {//读取代码文件
            code.push_back(static_cast<char>(code_file.peek()));
            code_file.ignore();
        }
        size_t i = 0;
        while (i <= code.length() - 1) {
            if (code[i] == '/' && code[i + 1] == '/') {//单行注释
                while (code[i] != '\n') {
                    i += 1;
                }
            } else {
                processed_code.push_back(code[i]);
                if (code[i] == '\n') {
                    line += 1;
                }
                i += 1;
            }

        }
        code_file.close();
    } catch (const exception &e) {
        cerr << e.what() << '\n';
    }
    while (code.find('\t') != string::npos) {
        code.erase(code.find('\t'), 1);
    }
    return processed_code;
}

Lexer::Lexer(const string &rules_file_name,const string&key_words_file_name) : rules_file_name_(rules_file_name),key_words_file_name(key_words_file_name) {
    auto_machines_.insert({LabelType::Operator, make_unique<AutoMachine>(rules_file_name, LabelType::Operator)});
    auto_machines_.insert({LabelType::Delimiter, make_unique<AutoMachine>(rules_file_name, LabelType::Delimiter)});
    auto_machines_.insert({LabelType::Identifier, make_unique<AutoMachine>(rules_file_name, LabelType::Identifier)});
    auto_machines_.insert({LabelType::Scientific, make_unique<AutoMachine>(rules_file_name, LabelType::Scientific)});
    read_key_words(key_words_file_name);
}


list<Token> Lexer::analyze(const std::string &file_name) {//todo 这里可能有错误处理
    list<Token> tokens;
    string code = std::move(preprocessing(file_name));
    size_t line = 1;
    int i = 0;
    string error_msg;
    while (code[i] != 0) {
        char ch = static_cast<char>(code[i]);
        if (ch == '\n') {
            line += 1;
            i += 1;
            continue;
        }
        if (ch == ' ') {
            i += 1;
            continue;
        }
        int j = find_word_end(code, i);
        string part = code.substr(i, j - i + 1);
        int len = -1;
        len = auto_machines_[LabelType::Identifier]->analyze(part);
        if(len==-1){
            error_msg="[Line num:"+ to_string(line)+"]: "+part+'\n';
            break;
        }else if(len==0){

        }else{
            string token = part.substr(0, len);
            if (is_key_word(token)) {
                Token new_token(line, TokenType::Keyword, token);
                tokens.push_back(new_token);
            } else {
                Token new_token(line, TokenType::Identifier, token);
                tokens.push_back(new_token);
            }
            i = i + len;
            continue;
        }
        len = auto_machines_[LabelType::Delimiter]->analyze(part);
        if(len==-1){
            error_msg="[Line num:"+ to_string(line)+"]: "+part+'\n';
            break;
        }else if(len==0){

        }else{
            string token = part.substr(0, len);
            Token new_token(line, TokenType::Delimiter, token);
            tokens.push_back(new_token);
            i = i + len;
            continue;
        }
        len = auto_machines_[LabelType::Operator]->analyze(part);
        if(len==-1){
            error_msg="[Line num:"+ to_string(line)+"]: "+part+'\n';
            break;
        }else if(len==0){

        }else{
            string token = part.substr(0, len);
            Token new_token(line, TokenType::Operator, token);
            tokens.push_back(new_token);
            i = i + len;
            continue;
        }
        len = auto_machines_[LabelType::Scientific]->analyze(part);
        if(len==-1){
            error_msg="[Line num:"+ to_string(line)+"]: "+part+'\n';
            break;
        }else if(len==0){

        }else{
            string token = part.substr(0, len);
            Token new_token(line, TokenType::Constant, token);
            tokens.push_back(new_token);
            i = i + len;
            continue;
        }
    }
    if(error_msg.empty()){
        cerr<<error_msg<<'\n';
    }
    return tokens;
}

bool Lexer::is_key_word(const string &token) const {
    if (key_words_.count(token) == 0) {
        return false;
    }
    return true;
}

int Lexer::find_word_end(const string &str, int i) {
    int j = i;
    int n = static_cast<int>(str.size());
    while (str[j] != ' ' && j < n) {
        j += 1;
    }
    j -= 1;
    return j;
}

void Lexer::read_key_words(const string &file_name) {
    try{
        ifstream key_words_file(file_name);
        if(!key_words_file.is_open()){
            throw runtime_error("Can not open the file!");
        }
        string line;
        while(getline(key_words_file,line)){
            key_words_.insert(trim(line));
        }
        key_words_file.close();
    }catch (const exception& e){
        cerr<<e.what()<<'\n';
    }
}

//list<Token> Lexer::other_analyze(const string &file_name) {
//    list<Token> tokens;
//    string code= preprocessing(file_name);
//    int line=1;
//    istringstream is(code);
//    string error_msg;
//    bool is_error=false;
//    while(is.peek()!=EOF){
//        if(is_error){
//            break;
//        }
//        if(is.peek()=='\n'){
//            line++;
//        }
//        string sub;
//        is>>sub;
//        int pos=0,len=1;
//        while(pos<sub.length()){
//            char ch=sub[pos+len-1];
//            int state=auto_machines_[LabelType::Identifier]->move_to(ch,0);
//            int last_state=-1;
//            if(state!=-1){
//                while(state!=-1){
//                    len+=1;
//                    ch=sub[pos+len-1];
//                    last_state=state;
//                    state=auto_machines_[LabelType::Identifier]->move_to(ch,state);
//                }
//                string current_identified=sub.substr(pos,len-1);
//                if(!auto_machines_[LabelType::Identifier]->dfa_end_states_.count(last_state)){
//                    error_msg="[Line num:"+to_string(line)+"]: "+"invalid identifier: "+current_identified+'\n';
//                    is_error=true;
//                    break;
//                }
//                if(is_key_word(current_identified)){
//                    Token new_token(line,TokenType::Keyword,current_identified);
//                    tokens.push_back(new_token);
//                }else{
//                    Token new_token(line,TokenType::Identifier,current_identified);
//                    tokens.push_back(new_token);
//                }
//                pos=pos+len-1;
//                len=1;
//                continue;
//            }else{
//                state=auto_machines_[LabelType::Delimiter]->move_to(ch,0);
//                if(state!=-1){
//                    while(state!=-1){
//                        len+=1;
//                        ch=sub[pos+len-1];
//                        last_state=state;
//                        state=auto_machines_[LabelType::Delimiter]->move_to(ch,state);
//                    }
//                    string current_identified=sub.substr(pos,len-1);
//                    if(!auto_machines_[LabelType::Delimiter]->dfa_end_states_.count(last_state)){
//                        error_msg="[Line num:"+to_string(line)+"]: "+"invalid delimiter: "+current_identified+'\n';
//                        is_error=true;
//                        break;
//                    }else{
//                        Token new_token(line,TokenType::Delimiter,current_identified);
//                        tokens.push_back(new_token);
//                        pos=pos+len-1;
//                        len=1;
//                        break;
//                    }
//                }else{
//                    state=auto_machines_[LabelType::Scientific]->move_to(ch,0);
//                    if(state!=-1){
//                        while(state!=-1){
//                            len+=1;
//                            ch=sub[pos+len-1];
//                            last_state=state;
//                            state=auto_machines_[LabelType::Scientific]->move_to(ch,state);
//                        }
//                        string current_identified=sub.substr(pos,len-1);
//                        if(!auto_machines_[LabelType::Scientific]->dfa_end_states_.count(last_state)){
//                            error_msg="[Line num:"+to_string(line)+"]: "+"invalid scientific: "+current_identified+'\n';
//                            is_error=true;
//                            break;
//                        }
//                        Token new_token(line,TokenType::Constant,current_identified);
//                        tokens.push_back(new_token);
//                        pos=pos+len-1;
//                        len=1;
//                        continue;
//                    }else{
//                        state=auto_machines_[LabelType::Operator]->move_to(ch,0);
//                        if(state!=-1){
//                            while(state!=-1){
//                                len+=1;
//                                ch=sub[pos+len-1];
//                                last_state=state;
//                                state=auto_machines_[LabelType::Operator]->move_to(ch,state);
//                            }
//                            string current_identified=sub.substr(pos,len-1);
//                            if(!auto_machines_[LabelType::Operator]->dfa_end_states_.count(last_state)){
//                                error_msg="[Line num:"+to_string(line)+"]: "+"invalid operator: "+current_identified+'\n';
//                                is_error=true;
//                                break;
//                            }
//                            Token new_token(line,TokenType::Operator,current_identified);
//                            tokens.push_back(new_token);
//                            pos=pos+len-1;
//                            len=1;
//                            continue;
//                        }else{
//                            error_msg="[Line num:"+to_string(line)+"]: "+"error tokens"+'\n';
//                            is_error=true;
//                            break;
//                        }
//                    }
//                }
//            }
//        }
//    }
//    cout<<error_msg<<'\n';
//    return tokens;
//}

string trim(const string& str){
    size_t first=str.find_first_not_of(' ');
    if(first==string::npos){
        return "";
    }
    size_t last=str.find_last_not_of(' ');
    return str.substr(first,last-first+1);
}

int main(int args, char *argv[]) {
    if(args<4){
        return 0;
    }

    string rules_file_name=argv[1];
    string key_words_file_name=argv[2];
    string code_file_name=argv[3];
    Lexer lexer(rules_file_name,key_words_file_name);
    list<Token> tokens = lexer.analyze(code_file_name);
    cout << "tokens_count:" << tokens.size() << '\n';
    for (const auto &token: tokens) {
        cout << '(' << token.line_ << ',' << token_type_to_string[token.type_] << ',' << token.value_ << ")\n";
    }
    return 0;
}
