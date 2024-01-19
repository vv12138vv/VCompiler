#include"lexer.hpp"

#include <utility>

//读取生成文件
list<string> AutoMachine::read_rules(const std::string& rules_file_name, const std::string& label) {
    list<string> rules_string;
    try {
        ifstream rules_file(rules_file_name);
        if (!rules_file.is_open()) {
            throw runtime_error("Can not open the file!");
        }
        string line;
        getline(rules_file, line);
        while (line != label) {//连续读取直至目标行
            getline(rules_file, line);
        }
        while (rules_file.peek() != EOF) {
            getline(rules_file, line);
            if (line.length() != 4 && line.length() != 5) {//判断是否符合三型文法
                throw runtime_error("The format of rules are error");
            }
            rules_string.emplace_back(line);
            if (rules_file.peek() == '[') {
                break;
            }
        }
        rules_file.close();
    } catch (const exception &e) {
        cerr << e.what() << '\n';
    }
    return rules_string;
}

AutoMachine::AutoMachine(string rules_file_name, LabelType label_type) : rules_file_name_(std::move(rules_file_name)),
                                                                         label_type_(label_type) {
    init_rules();
}

void AutoMachine::init_rules() {
    list<string> rules_string=std::move(read_rules(rules_file_name_,label_mp[label_type_]));
    generate_rules(rules_string);
#ifdef DEBUG
    for(const auto& rule: rules_){
        cout<<rule.first<<':'<<rule.second<<'\n';
    }
#endif
    unordered_set<char> terminals;
    unordered_set<char> non_terminals;

}

void AutoMachine::generate_rules(const list<string> &rules_string) {
    try {
        for (const string &rule_string: rules_string) {
            size_t index = rule_string.find("->");
            if (index == string::npos) {
                throw runtime_error("The format of rules are error");
            }
            string left=rule_string.substr(0,index);
            string right=rule_string.substr(index+2,rule_string.length()-index-2);
            Rule rule{left,right};
            rules_.emplace_back(rule);
        }
    } catch (const exception &e) {
        cerr << e.what() << '\n';
    }
}


int main(int argc, char *argv[]) {
    AutoMachine autoMachine(argv[1], LabelType::Delimiter);
    return 0;
}