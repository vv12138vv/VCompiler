#include "analyzer.hpp"

const unordered_set<char> Action::ops{'+', '*', '-', '='};

Action::Action(Symbol left_op, Symbol right_op, Symbol res, ActionType action_type) : left_op_(left_op),
                                                                                      right_op_(right_op),
                                                                                      res_(res),
                                                                                      action_type_(action_type) {

}

vector<Action> Action::to_actions(const string &str) {
    vector<Action> actions;
    auto idx = str.find("print");
    if (idx != string::npos) {
        Symbol op = Analyzer::find_print_op(str);
        actions.emplace_back(op, NIL, NIL, ActionType::Print);
        return actions;
    }
    for (int i = str.size() - 1; i >= 0; i--) {
        if (Action::ops.count(str[i])) {
            Symbol left_op = Analyzer::find_left_op(str, i);
            Symbol right_op = Analyzer::find_right_op(str, i);
            ActionType action_type;
            switch (str[i]) {
                case '+':
                    action_type = ActionType::Add;
                    break;
                case '*':
                    action_type = ActionType::Multiply;
                    break;
                case '=':
                    action_type = ActionType::Assign;
                    break;
            }
            actions.emplace_back(left_op, right_op, NIL, action_type);
        }
    }
    return actions;
}


auto Analyzer::generate_rules(const string &rule_file_name) {
    vector<Rule> rules;
    Semantic senmatics;
    try {
        ifstream rules_file(rule_file_name);
        if (!rules_file.is_open()) {
            throw Exception("Can not open the file:", rule_file_name);
        }
        string line;
        int i = 0;
        while (getline(rules_file, line)) {
            auto it = line.find(" {");
            if (it == string::npos) {
                throw Exception("The format of Rules are wrong", "");
            }
            string rule_part = line.substr(0, it);
            Rule rule = Rule::to_rule(static_cast<int>(rules.size()), rule_part);
            rules.push_back(std::move(rule));
//            string senmatic_part = line.substr(it + 2, line.size() - it - 3);
//            vector<Action> actions = Action::to_actions(senmatic_part);
        }
        rules_file.close();
    } catch (const Exception &e) {
        cerr << e.what() << '\n';
    }
    return pair{rules, senmatics};
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
    cout << content << '\n';
    return {content, SymbolType::Non_Terminal};
}

Symbol Analyzer::find_print_op(const string &str) {
    auto left_brace = str.find('<');
    auto dot_idx = str.find('.');
    string content = str.substr(left_brace + 1, dot_idx - 1 - left_brace);
    cout << content;
    return {content, SymbolType::Non_Terminal};
}


Analyzer::Analyzer(const string &rule_file_name) {
    auto [rules, senmatics] = generate_rules(rule_file_name);
    parser_ = make_unique<Parser>(std::move(rules));
}

//语义分析
void Analyzer::analyze(const list<Symbol>& input,const unordered_map<string,const Token&>& sym_token_mp,bool verbose= false){
    try{
        list<Symbol> str=input;
        Symbol end(FRONT_SEARCH);
        str.push_back(end);

        list<int> state_stack;
        list<Symbol> symbol_stack;
        list<int> semantic_stack;
        list<string> val_stack;
        state_stack.push_back(0);
        symbol_stack.push_back(FRONT_SEARCH);
        semantic_stack.push_back(-1);
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
                    str.pop_front();
                    break;
                }
                case ElementType::Reduce:{
                    const Rule& reduce_rule=parser_->rules_[element.index_];
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






