#include "analyzer.hpp"

const unordered_set<char> ops{'+','*','='};

ActionType Analyzer::to_action(const string &str) {
    auto idx = str.find("print");
    if (idx != string::npos) {
        return ActionType::Print;
    }
    idx=str.find('+');
    if(idx!=string::npos){
        return ActionType::Add;
    }
    idx=str.find('*');
    if(idx!=string::npos){
        return ActionType::Multiply;
    }
    return ActionType::Assign;
}


auto Analyzer::generate_rules(const string &rule_file_name) {
    vector<Rule> rules;
    try {
        ifstream rules_file(rule_file_name);
        if (!rules_file.is_open()) {
            throw Exception("Can not open the file:", rule_file_name);
        }
        string line;
        while (getline(rules_file, line)) {
            auto it = line.find(" {");
            if (it == string::npos) {
                throw Exception("The format of Rules are wrong", "");
            }
            string rule_part = line.substr(0, it);
            Rule rule = Rule::to_rule(static_cast<int>(rules.size()), rule_part);
            rules.push_back(std::move(rule));
        }
        rules_file.close();
    } catch (const Exception &e) {
        cerr << e.what() << '\n';
    }
    return rules;
}


auto Analyzer::generate_actions(const string &rule_file_name) {
    vector<ActionType> semantics;
    try{
        ifstream rules_file(rule_file_name);
        if (!rules_file.is_open()) {
            throw Exception("Can not open the file:", rule_file_name);
        }
        string line;
        while (getline(rules_file, line)) {
            auto it = line.find(" {");
            if (it == string::npos) {
                throw Exception("The format of Rules are wrong", "");
            }
            string senmatic_part = line.substr(it + 2, line.size() - it - 3);
            ActionType action = to_action(senmatic_part);
            semantics.push_back(action);
        }
        rules_file.close();
    }catch(const Exception& e){
        cerr << e.what() << '\n';
    }
    return semantics;
}


auto Analyzer::generate_semantics(const vector<ActionType>& actions) {
    assert(parser_->rules_.size()==actions.size());
    for(int i=0;i<parser_->rules_.size();i++){
        semantics_.insert({i,actions[i]});
    }
}



Symbol Analyzer::find_left_op(const string &str, int i) {
    int j = i - 1;
    int dot_idx = j;
    while (j >= 0 && str[j] != '<') {
        if (str[j] == '.') {
            dot_idx = j;
        }
        j -= 1;
    }
    string content = str.substr(j + 1, dot_idx - j - 1);

    return {content, SymbolType::Non_Terminal};
}

Symbol Analyzer::find_right_op(const string &str, int i) {
    int j = i + 1;
    int dot_idx = j;
    int left_brace = j;
    while (j < str.size()) {
        if (str[j] == '.') {
            dot_idx = j;
            break;
        } else if (str[j] == '<') {
            left_brace = j;
        }
        j += 1;
    }
    string content = str.substr(left_brace + 1, dot_idx - 1 - left_brace);
    SymbolType symbol_type;
    if(parser_->non_terminals_.count(Symbol(content,SymbolType::Non_Terminal))){
        return {content, SymbolType::Non_Terminal};
    }
    return {content,SymbolType::Terminal};
}




//Symbol Analyzer::find_print_op(const string &str) {
//    auto left_brace = str.find('<');
//    auto dot_idx = str.find('.');
//    string content = str.substr(left_brace + 1, dot_idx - 1 - left_brace);
//    cout << content;
//    return {content, SymbolType::Non_Terminal};
//}


Analyzer::Analyzer(const string &rule_file_name) {
    auto rules = generate_rules(rule_file_name);
    parser_ = make_unique<Parser>(std::move(rules));
    auto actions= generate_actions(rule_file_name);
    generate_semantics(actions);
}

//语义分析
void Analyzer::analyze(const list<Symbol>& input,const unordered_map<string,const Token&>& sym_token_mp,bool verbose= false){
    try{
        list<Symbol> str=input;
        Symbol end(FRONT_SEARCH);
        str.push_back(end);

        list<int> state_stack;
        list<Symbol> symbol_stack;
        vector<int> semantic_stack;
        list<string> val_stack;
        state_stack.push_back(0);
        symbol_stack.push_back(FRONT_SEARCH);
        semantic_stack.push_back(INT_MAX);
        val_stack.emplace_back("_");
        int step=0;
        bool is_acc=false;
        bool is_error=false;
        while(true){
            int cur_state=state_stack.back();
            Symbol top_sym=str.front();
            Element element;
            if (top_sym.symbol_type_ == SymbolType::Terminal||top_sym.symbol_type_==SymbolType::Front) {
                Symbol fake_sym=parser_->make_fake_symbol(top_sym,sym_token_mp);
                element=parser_->action_[cur_state][fake_sym];
            }else if (top_sym.symbol_type_ == SymbolType::Non_Terminal) {
                element = parser_->goto_[cur_state][top_sym];
            }
            switch(element.element_type_){
                case ElementType::Move:{
                    state_stack.push_back(element.index_);
                    symbol_stack.push_back(top_sym);
                    auto it=sym_token_mp.find(top_sym.content_);
                    if(it->second.type_==TokenType::Constant){
                        int num= stoi(top_sym.content_);
                        semantic_stack.push_back(num);
                    }else{
                        semantic_stack.push_back(INT_MAX);
                    }
                    str.pop_front();
                    break;
                }
                case ElementType::Reduce:{
                    const Rule& reduce_rule=parser_->rules_[element.index_];
                    auto it=semantics_.find(reduce_rule.index_);
                    if(it==semantics_.end()){
                        throw Exception("Can not find semantics rule: ","");
                    }
                    auto action=it->second;
                    switch (action) {
                        case ActionType::Assign:{
                            //assign也可简化为不更改语义栈
                            break;
                        }
                        case ActionType::Init:{
                            //init实际可以简化为不更改语义栈
                            break;
                        }
                        case ActionType::Add:{
                            semantic_stack[semantic_stack.size()-1-2]+=semantic_stack[semantic_stack.size()-1];
                            for(int i=0;i<2;i++){
                                semantic_stack.pop_back();
                            }
                            break;
                        }
                        case ActionType::Multiply:{
                            semantic_stack[semantic_stack.size()-1-2]*=semantic_stack[semantic_stack.size()-1];
                            for(int i=0;i<2;i++){
                                semantic_stack.pop_back();
                            }
                            break;
                        }
                        case ActionType::Print:{
                            cout<<"Print action: "<<semantic_stack.back()<<'\n';
                            break;
                        }
                    }
                    int pop_size=static_cast<int>(reduce_rule.right_.size());
                    if(reduce_rule.is_nil_rule()){
                        pop_size=0;
                    }
                    for(int i=0;i<pop_size;i++){
                        state_stack.pop_back();
                        symbol_stack.pop_back();
                    }
                    str.push_front(reduce_rule.left_);
                    break;
                }
                case ElementType::Goto:{
                    state_stack.push_back(element.index_);
                    symbol_stack.push_back(top_sym);
                    str.pop_front();
                    break;
                }
                case ElementType::Accept:{
                    if(str.size()!=1){
                        throw Exception("Acc Error", "");
                    }
                    is_acc=true;
                    break;
                }
                case ElementType::Error:{
                    is_error=true;
                    break;
                }
            }
            step+=1;
            if(verbose){
                auto print_analyze=[&](){
                    string content;
                    content+= to_string(step)+"\t\t";
                    //打印状态栈
                    for(const auto& state:state_stack){
                        content+= to_string(state)+',';
                    }
                    content+="\t\t";
                    //打印符号栈
                    for(const auto& sym:symbol_stack){
                        content+=sym.content_;
                    }
                    content+="\t\t";
                    //打印语义栈
                    for(int num:semantic_stack){
                        if(num==INT_MAX){
                            content+="_";
                        }else{
                            content+= to_string(num);
                        }
                    }
                    content+="\t\t";
                    //打印符号串
                    for(const auto& sym:str){
                        content+=sym.content_;
                    }
                    content+="\t\t";
                    content+=Element::to_string(element);
                    cout<<content<<'\n';
                };
                print_analyze();
            }
            if(is_acc||is_error){
                break;
            }
        }

    }catch (const Exception& e){
        cerr << e.what() << '\n';
    }
}

void Analyzer::call(const std::string &token_file_name) {
    list<Token> tokens=std::move(parser_->load_tokens(token_file_name));
    auto [syms,sym_token_mp]=std::move(parser_->tokens_to_syms(tokens));
    analyze(syms,sym_token_mp,true);
}








