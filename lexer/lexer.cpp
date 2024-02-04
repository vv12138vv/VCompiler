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

Lexer::Lexer(const string &rules_file_name, const string &key_words_file_name) : rules_file_name_(rules_file_name),
                                                                                 key_words_file_name(
                                                                                         key_words_file_name) {
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
        if (len == -1) {
            error_msg = "[Line num:" + to_string(line) + "]: " + part + '\n';
            break;
        } else if (len == 0) {

        } else {
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
        if (len == -1) {
            error_msg = "[Line num:" + to_string(line) + "]: " + part + '\n';
            break;
        } else if (len == 0) {

        } else {
            string token = part.substr(0, len);
            Token new_token(line, TokenType::Delimiter, token);
            tokens.push_back(new_token);
            i = i + len;
            continue;
        }
        len = auto_machines_[LabelType::Operator]->analyze(part);
        if (len == -1) {
            error_msg = "[Line num:" + to_string(line) + "]: " + part + '\n';
            break;
        } else if (len == 0) {

        } else {
            string token = part.substr(0, len);
            Token new_token(line, TokenType::Operator, token);
            tokens.push_back(new_token);
            i = i + len;
            continue;
        }
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
    if (error_msg.empty()) {
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
            output << '(' << token.line_ << ',' << token_type_to_string[token.type_] << ',' << token.value_ << ")\n";
        }
        output.close();
    } catch (const Exception &e) {
        cerr << e.what() << '\n';
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

int main(int args, char *argv[]) {
    if (args < 4) {
        return 0;
    }

    string rules_file_name = argv[1];
    string key_words_file_name = argv[2];
    string code_file_name = argv[3];
    Lexer lexer(rules_file_name, key_words_file_name);
    list<Token> tokens = lexer.analyze(code_file_name);
    Lexer::save_to("./tokens.txt", tokens);
    return 0;
}


