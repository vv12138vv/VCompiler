#include"auto_machine.hpp"

#include <utility>

const char Nil_char='@';
//读取生成文件
list<string> AutoMachine::read_rules(const string &rules_file_name, const std::string &label) {
    list<string> rules_string;
    try {
        ifstream rules_file(rules_file_name);
        if (!rules_file.is_open()) {
            throw runtime_error("Can not open the file!");
        }
        string line;
        getline(rules_file, line);
        int line_num=1;
        while (line != label) {//连续读取直至目标行
            getline(rules_file, line);
            line_num+=1;
        }
        while (rules_file.peek() != EOF) {
            getline(rules_file, line);
            line_num+=1;
            if (line.length() != 4 && line.length() != 5) {//判断是否符合三型文法
                cerr<<"error line_num:"<<line_num<<'\n';
                cerr<<"error line content"<<line<<'\n';
                throw runtime_error("The format of rules are error");
            }
            rules_string.emplace_back(line);
            if (rules_file.peek() == '[') {
                break;
            }
        }
        rules_file.close();
    } catch (const exception &e) {
        cerr << e.what()<< '\n';
    }
    return rules_string;
}

AutoMachine::AutoMachine(const string& rules_file_name, LabelType label_type) : rules_file_name_(rules_file_name),
                                                                         label_type_(label_type) {
    init();
}

void AutoMachine::init() {
    list<string> rules_string = std::move(read_rules(rules_file_name_, label_mp[label_type_]));
    //初始化规则
    generate_rules(rules_string);
    //初始化终结符与非终结符表
    generate_terminals_and_non_terminals();

    //初始化字母表
    alphabets_ = unordered_set<char>(terminals_.begin(), terminals_.end());
    //添加空
    alphabets_.insert(Nil_char);

    //对非终结符进行索引
    size_t index = 1;
    for (const auto &c: non_terminals_) {
        non_terminals_index_.insert({c, index});
        index += 1;
    }
    //添加终态
    nfa_state_count_ = static_cast<int>(non_terminals_.size()) + 1;
    //初始非终结符索引
    nfa_start_index_ = non_terminals_index_[rules_.front().first[0]];
    nfa_end_states_.insert(static_cast<int>(nfa_state_count_));
    int end = *(nfa_end_states_.begin());
    //构造nfa状态转移表
    make_nfa(end);
    make_dfa();
    dfa_end_states_=find_end_dfa_state();

}
//根据字符串序列生成rules
void AutoMachine::generate_rules(const list<string> &rules_string) {
    try {
        for (const string &rule_string: rules_string) {
            size_t index = rule_string.find("->");
            if (index == string::npos) {
                throw runtime_error("The format of rules are error");
            }
            string left = rule_string.substr(0, index);
            string right = rule_string.substr(index + 2, rule_string.length() - index - 2);
            Rule_ rule{left, right};
            rules_.emplace_back(rule);
        }
    } catch (const exception &e) {
        cerr << e.what() << '\n';
    }
}

//求解NFA，end为终止状态
void AutoMachine::make_nfa(int end) {
    //对于每个非终结符，求出他的转移
    for (char non_terminal: non_terminals_) {
        unordered_map<char, unordered_set<int>> move;
        unordered_set<char> has_edge;

        for (const auto &rule: rules_) {
            if (rule.first[0] != non_terminal) {
                continue;
            }
            //根据自动机的种类进行生成
            switch (label_type_) {
                case LabelType::Identifier:
                    if (rule.second.length() == 1) {
                        char ch = rule.second[0];
                        if (ch == 'a') {
                            for (const auto &letter: upper_case_letters) {
                                has_edge.insert(letter);
                                move[letter].insert(end);
                            }
                            for (const auto &letter: lower_case_letters) {
                                has_edge.insert(letter);
                                move[letter].insert(end);
                            }
                        }else if(ch=='d'){
                            for(char i='0';i<='9';i++){
                                has_edge.insert(i);
                                move[i].insert(end);
                            }
                        } else {
                            has_edge.insert(ch);
                            move[ch].insert(end);
                        }
                    } else if (rule.second.length() == 2) {
                        char ch = rule.second[0];
                        int to_non_terminal = non_terminals_index_[rule.second[1]];
                        if (ch == 'a') {
                            for (const auto &letter: upper_case_letters) {
                                has_edge.insert(letter);
                                move[letter].insert(to_non_terminal);
                            }
                            for (const auto &letter: lower_case_letters) {
                                has_edge.insert(letter);
                                move[letter].insert(to_non_terminal);
                            }
                        }else if(ch=='d'){
                            for(char i='0';i<='9';i++){
                                has_edge.insert(i);
                                move[i].insert(to_non_terminal);
                            }
                        } else {
                            has_edge.insert(ch);
                            move[ch].insert(to_non_terminal);
                        }
                    }
                    break;
                case LabelType::Delimiter:
                    if (rule.second.length() == 1) {
                        char ch = rule.second[0];
                        has_edge.insert(ch);
                        move[ch].insert(end);
                    }
                    break;
                case LabelType::Scientific:
                    if (rule.second.length() == 1) {
                        char ch = rule.second[0];
                        if (ch == 'd') {
                            for (char i = '0'; i <= '9'; i++) {
                                has_edge.insert(i);
                                move[i].insert(end);
                            }
                        }else{
                            has_edge.insert(ch);
                            move[ch].insert(end);
                        }
                    } else if (rule.second.length() == 2) {
                        char ch = rule.second[0];
                        int to_non_terminal = non_terminals_index_[rule.second[1]];
                        if (ch == 'd') {
                            for (char i = '0'; i <= '9'; i++) {
                                has_edge.insert(i);
                                move[i].insert(to_non_terminal);
                            }
                        } else {
                            has_edge.insert(ch);
                            move[ch].insert(to_non_terminal);
                        }
                    }
                    break;
                case LabelType::Operator:
                    if (rule.second.length() == 1) {
                        char ch = rule.second[0];
                        has_edge.insert(ch);
                        move[ch].insert(end);
                    } else if (rule.second.length() == 2) {
                        char ch = rule.second[0];
                        int to_non_terminal = non_terminals_index_[rule.second[1]];
                        has_edge.insert(ch);
                        move[ch].insert(to_non_terminal);
                    }
                    break;
            }
        }
        //对于不能发生转移的终结符
        for (char alphabet: alphabets_) {
            if (!has_edge.count(alphabet)) {
                move[alphabet] = unordered_set<int>();
            }
        }
        nfa_[non_terminals_index_[non_terminal]] = move;
    }
    unordered_map<char, unordered_set<int>> end_move;
    for (char alphabet: alphabets_) {
        end_move[alphabet] = unordered_set<int>();
    }
    nfa_[end] = end_move;
}

void AutoMachine::make_dfa() {
    //初始化初态
    dfa_state_count_=0;
    //初始状态空闭包
    unordered_set<int> start_states;
    start_states.insert(nfa_start_index_);
    unordered_set<int> e_closure= get_e_closure(start_states);
    DFAState start_state(false,std::move(e_closure));
    dfa_[dfa_state_count_++]=std::move(start_state);

    int next_state=0;
    while(next_state<dfa_state_count_){
        int now=next_state;
        dfa_[now].is_existed_=true;
        for(char alphabet:alphabets_){
            unordered_set<int> move_set= move(dfa_[now],alphabet);
            unordered_set<int> move_e_closure= get_e_closure(move_set);
            int index=is_existed_state(move_e_closure);
            if(index!=-1){//DFA状态已存在的情况，直接指向。
                dfa_[now].to_[alphabet]=index;
            }else{//状态不存在则新建状态
                if(move_e_closure.empty()){
                    dfa_[now].to_[alphabet]=-1;
                }else{
                    DFAState new_state(false,std::move(move_e_closure));
                    dfa_[dfa_state_count_]=new_state;
                    dfa_[now].to_[alphabet]=dfa_state_count_;
                    dfa_state_count_+=1;
                }
            }
        }
        next_state+=1;
    }
}
//求解状态的e闭包
unordered_set<int> AutoMachine::get_e_closure(const unordered_set<int> &t) {
    unordered_set<int> e_closure;
    //用于广搜
    queue<int> help;
    for(int i:t){
        help.push(i);
    }
    for(int i:t){
        e_closure.insert(i);
    }
    while(!help.empty()){
        int now=help.front();
        help.pop();
        unordered_set<int> e_goto=nfa_[now][Nil_char];
        for(int state:e_goto){
            if(!e_closure.count(state)){
                e_closure.insert(state);
                help.push(state);
            }
        }
    }
    return e_closure;
}
//求解状态集合I的ch弧转换
unordered_set<int> AutoMachine::move(const DFAState& dfa_state,char ch){
    unordered_set<int> move_result;
    for(auto state:dfa_state.states_){
        unordered_set<int> result=nfa_[state][ch];
        for(int move_to:result){
            move_result.insert(move_to);
        }
    }
    return move_result;
}
//判断是否是已存在的状态
int AutoMachine::is_existed_state(const unordered_set<int> &states) {
    for(int i=0;i<dfa_.size();i++){
        if(states==dfa_[i].states_){
            return i;
        }
    }
    return -1;
}

void AutoMachine::print_dfa() {
    cout<<"=================DFA==================\n";
    cout<<"rule_type:"<<label_mp[label_type_]<<'\n';
    cout<<"start_state:"<<0<<'\n';
    string end_states;
    for(int state:dfa_end_states_){
        end_states+=static_cast<char>(state+'0');
        end_states+=',';
    }
    if(end_states.back()==','){
        end_states.pop_back();
    }
    cout<<"end_state:{"<<end_states<<"}\n";
    cout<<"state:\t";
    for(char alphabet:alphabets_){
        if(alphabet!=Nil_char){
            cout<<alphabet<<'\t';
        }
    }
    cout<<'\n';
    for(const auto& item:dfa_){
        cout<<item.first<<'\t';
        for(char alphabet:alphabets_){
            if(alphabet==Nil_char){
                continue;
            }
            if(dfa_[item.first].to_[alphabet]!=-1){
                cout<<'{'<<dfa_[item.first].to_[alphabet]<<'}'<<'\t';
            }else{
                cout<<"{}"<<'\t';
            }
        }
        cout<<'\n';
    }
    cout<<'\n';
}

void AutoMachine::print_nfa() {
    cout<<"=================NFA==================\n";
    cout<<"rule_type:"<<label_mp[label_type_]<<'\n';
    cout<<"state_count:"<<nfa_state_count_<<'\n';
    cout<<"start_index:"<<nfa_start_index_<<'\t'<<"end_index:"<<nfa_state_count_<<'\n';
    cout<<"state:\t";
    for(char alphabet:alphabets_){
        cout<<alphabet<<'\t';
    }
    cout<<'\n';
    for(auto& item:nfa_){
        cout<<item.first<<'\t';
        for(char alphabet:alphabets_){
            auto& set=item.second[alphabet];
            string show;
            show+='{';
            for(int state:set){
                show+=static_cast<char>('0'+state);
                show+=',';
            }
            if(show.back()==','){
                show.pop_back();
            }
            show+='}';
            cout<<show<<'\t';
        }
        cout<<'\n';
    }
    cout<<'\n';
}

void AutoMachine::print_alphabets() {
    cout<<"=================alphabets==================\n";
    for(char alphabet:alphabets_){
        cout<<alphabet<<'\t';
    }
    cout<<'\n';
}

void AutoMachine::generate_terminals_and_non_terminals() {
    for (const auto &rule: rules_) {
        //左部为非终结符
        non_terminals_.insert(rule.first[0]);
        //右部
        const string &right = rule.second;
        //定义右部处理方式
        switch (label_type_) {
            case LabelType::Identifier:
                if(right.length()==1){
                    if(right[0]=='a'){
                        for(char letter:upper_case_letters){
                            terminals_.insert(letter);
                        }
                        for(char letter:lower_case_letters){
                            terminals_.insert(letter);
                        }
                    }else if(right[0]=='d'){
                        for(char ch='0';ch<='9';ch++){
                            terminals_.insert(ch);
                        }
                    }else if(right[0]==Nil_char) {

                    }else{
                        terminals_.insert(right[0]);
                    }
                }else if(right.length()==2){
                    if(right[0]=='a'){
                        for(char letter:upper_case_letters){
                            terminals_.insert(letter);
                        }
                        for(char letter:lower_case_letters){
                            terminals_.insert(letter);
                        }
                    }else if(right[0]=='d'){
                        for(char ch='0';ch<='9';ch++){
                            terminals_.insert(ch);
                        }
                    }else if(right[0]==Nil_char){

                    }else{
                        terminals_.insert(right[0]);
                    }
                    non_terminals_.insert(right[1]);
                }
                break;
            case LabelType::Delimiter:
                if(right.length()==1){
                    terminals_.insert(right[0]);
                }else if(right.length()==2){
                    //不存在
                }
                break;
            case LabelType::Operator:
                if(right.length()==1){
                    terminals_.insert(right[0]);
                }else if(right.length()==2){
                    terminals_.insert(right[0]);
                    non_terminals_.insert(right[1]);
                }
                break;
            case LabelType::Scientific:
                if(right.length()==1){
                    if(right[0]=='d'){
                        for(char ch='0';ch<='9';ch++){
                            terminals_.insert(ch);
                        }
                    }else if(right[0]==Nil_char){

                    }else{
                        terminals_.insert(right[0]);
                    }
                }else if(right.length()==2){
                    if(right[0]=='d'){
                        for(char ch='0';ch<='9';ch++){
                            terminals_.insert(ch);
                        }
                    }else if(right[0]==Nil_char){

                    }else{
                        terminals_.insert(right[0]);
                    }
                    non_terminals_.insert(right[1]);
                }
                break;
        }
    }
}

unordered_set<int> AutoMachine::find_end_dfa_state() {
    unordered_set<int> end_states;
    for(auto& item:dfa_){
        for(auto& state:nfa_end_states_){
            if(item.second.states_.find(state)!=item.second.states_.end()){
                end_states.insert(item.first);
            }
        }
    }
    return end_states;
}

void AutoMachine::print_terminals() {
    cout<<"=======================terminals========================\n";
    cout<<"rule_type:"<<label_mp[label_type_]<<'\n';
    cout<<"terminals_count:"<<terminals_.size()<<'\n';
    for(char ch:terminals_){
        cout<<ch<<'\t';
    }
    cout<<'\n';
}

void AutoMachine::print_non_terminals() {
    cout<<"=======================non_terminals========================\n";
    cout<<"rule_type:"<<label_mp[label_type_]<<'\n';
    cout<<"non_terminals_count:"<<non_terminals_.size()<<'\n';
    for(const auto& item:non_terminals_index_){
        cout<<item.first<<':'<<item.second<<'\t';
    }
    cout<<'\n';
}

void AutoMachine::print_content() {
    cout<<"=======================AutoMachine========================\n";
    cout<<"rule_type:"<<label_mp[label_type_]<<'\n';
    print_terminals();
    print_non_terminals();
    print_nfa();
    print_dfa();
    cout<<'\n';
}

//返回可转移到的下一状态
int AutoMachine::move_to(char ch, int cur_state) {
    if(!terminals_.count(ch)){
        return -1;
    }
    if(dfa_[cur_state].to_[ch]!=-1){
        return dfa_[cur_state].to_[ch];
    }
    return -1;
}

//返回自动机可接受的数量，0则表示一个字符都无法接收，-1表示出现错误，其余整数为可接受的自动机可接受的数量
int AutoMachine::analyze(const std::string &part) {
    int len=0;
    char ch=static_cast<char>(part[0]);
    int state= move_to(ch,0);
    int pre_state=0;
    if(state==-1){
        return 0;
    }
    while(state!=-1){
        len+=1;
        ch=static_cast<char>(part[len]);
        pre_state=state;
        state= move_to(ch,state);
    }
    if(!dfa_end_states_.count(pre_state)){
        return -1;
    }
    return len;
}


