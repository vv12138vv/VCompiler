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
//        while (code_file.peek() != EOF) {//读取代码文件
//            code.push_back(static_cast<char>(code_file.peek()));
//            code_file.ignore();
//        }
        stringstream buff;
        buff<<code_file.rdbuf();
        code=std::move(buff.str());
        processed_code=remove_comment(code);
        code_file.close();
    } catch (const exception &e) {
        cerr << e.what() << '\n';
    }
    while (code.find('\t') != string::npos) {
        code.erase(code.find('\t'), 1);
    }
    return processed_code;
}

string Lexer::remove_comment(const std::string &code) {
    string processed_code;
    string line;
    istringstream code_stream(code);
    while(getline(code_stream,line)){
        auto comment_pos=line.find("//");
        if(comment_pos!=string::npos){
            processed_code+=line.substr(0,comment_pos);
        }else{
            processed_code+=line;
        }
        processed_code+='\n';
    }
    return processed_code;
}

Lexer::Lexer(const string &rules_file_name, const string &key_words_file_name) : rules_file_name_(rules_file_name),
                                                                                 key_words_file_name(
                                                                                         key_words_file_name) {
    //添加四种自动机
    auto_machines_.insert({LabelType::Operator, make_unique<AutoMachine>(rules_file_name, LabelType::Operator)});
    auto_machines_.insert({LabelType::Delimiter, make_unique<AutoMachine>(rules_file_name, LabelType::Delimiter)});
    auto_machines_.insert({LabelType::Identifier, make_unique<AutoMachine>(rules_file_name, LabelType::Identifier)});
    auto_machines_.insert({LabelType::Scientific, make_unique<AutoMachine>(rules_file_name, LabelType::Scientific)});
//    auto_machines_[LabelType::Operator]->print_content();
//    auto_machines_[LabelType::Delimiter]->print_content();
//    auto_machines_[LabelType::Identifier]->print_content();
//    auto_machines_[LabelType::Scientific]->print_content();

    //读取关键字
    read_key_words(key_words_file_name);
}


list<Token> Lexer::analyze(const std::string &file_name) {//todo 这里可能有错误处理
    list<Token> tokens;
    string code = std::move(preprocessing(file_name));
    size_t line = 1;
    int i = 0;
    string error_msg;
    while (i<=code.size()-1) {
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
        //切分出一个可能的token
        int j = find_word_end(code, i);
        string part = code.substr(i, j - i + 1);
        int len = -1;
        //首先使用Identifier状态机去检测
        len = auto_machines_[LabelType::Identifier]->analyze(part);
        if (len == -1) {//接受错误，即无法处于终止态
            error_msg = "[Line num:" + to_string(line) + "]: " + part + '\n';
            break;
        } else if (len == 0) {//无法接收

        } else {//根据可接受的最大长度切出Token
            string token = part.substr(0, len);
            if (is_key_word(token)) {//若该Identifier是Keyword
                tokens.emplace_back(line,TokenType::Keyword,token);
            } else {//若该Identifer不是Keyword
                tokens.emplace_back(line,TokenType::Identifier,token);
            }
            i = i + len;
            continue;
        }

        //使用界符自动机去检测
        len = auto_machines_[LabelType::Delimiter]->analyze(part);
        if (len == -1) {//接受错误，即无法处于接收态
            error_msg = "[Line num:" + to_string(line) + "]: " + part + '\n';
            break;
        } else if (len == 0) {

        } else {
            string token = part.substr(0, len);
            tokens.emplace_back(line,TokenType::Delimiter,token);
            i = i + len;
            continue;
        }
        //运算符自动机去检测
        len = auto_machines_[LabelType::Operator]->analyze(part);
        if (len == -1) {
            error_msg = "[Line num:" + to_string(line) + "]: " + part + '\n';
            break;
        } else if (len == 0) {

        } else {
            string token = part.substr(0, len);
            tokens.emplace_back(line,TokenType::Operator,token);
            i = i + len;
            continue;
        }
        //调用数字自动机去检测
        len = auto_machines_[LabelType::Scientific]->analyze(part);
        if (len == -1) {
            error_msg = "[Line num:" + to_string(line) + "]: " + part + '\n';
            break;
        } else if (len == 0) {

        } else {
            string token = part.substr(0, len);
            Token new_token(line, TokenType::Constant, token);
            tokens.push_back(new_token);
            i = i + len;
            continue;
        }
    }
    if (!error_msg.empty()) {
        cerr << error_msg << '\n';
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
    try {
        ifstream key_words_file(file_name);
        if (!key_words_file.is_open()) {
            throw runtime_error("Can not open the file!");
        }
        string line;
        while (getline(key_words_file, line)) {
            key_words_.insert(trim(line));
        }
        key_words_file.close();
    } catch (const exception &e) {
        cerr << e.what() << '\n';
    }
}

void Lexer::save_to(const string &file_name, const list<Token> &tokens) {
    try {
        ofstream output(file_name);
        if (!output.is_open()) {
            throw Exception("Create file failed: ", file_name);
        }
        output << "tokens_count:" << tokens.size() << '\n';
        for (const auto &token: tokens) {
            output << '(' << token.line_ << ',' << Token::token_type_to_string[token.type_] << ',' << token.value_ << ")\n";
        }
        output.close();
    } catch (const Exception &e) {
        cerr << e.what() << '\n';
    }
    cout<<"save tokens file in "<<file_name<<" finished!\n";
}

void Lexer::print_tokens(const list<Token> &tokens) {
    for(const Token& token:tokens){
        cout<<Token::to_string(token)<<'\n';
    }
}


string trim(const string &str) {
    size_t first = str.find_first_not_of(' ');
    if (first == string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, last - first + 1);
}




