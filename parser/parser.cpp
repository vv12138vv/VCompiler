#include "parser.hpp"

Parser::Parser(const string &rules_file_name) : rules_file_name_(rules_file_name) {
    init();
}

void Parser::init() {
    generate_rules(rules_file_name_, true);
    generate_terminals_and_non_terminals(true);
    generate_firsts(true);
    generate_DFA(true);
}

void Parser::generate_rules(const string &rules_file_name, bool verbose = false) {
    try {
        ifstream rules_file(rules_file_name);
        if (!rules_file.is_open()) {
            throw runtime_error("Can not open the file!");
        }
        string line;
        while (getline(rules_file, line)) {
            Rule rule = Rule::to_rule(static_cast<int>(rules_.size()), line);
            rules_.push_back(std::move(rule));
        }
        rules_file.close();
        if (verbose) {
            print_rules();
        }
    } catch (const exception &e) {
        cerr << e.what() << '\n';
    }
}

void Parser::generate_terminals_and_non_terminals(bool verbose = false) {
    try {
        if (rules_.empty()) {
            throw Exception("Generate Rules first", "");
        }
        for (const auto &rule: rules_) {
            if (!non_terminals_.count(rule.left_)) {//生成式左侧非终结符插入
                unordered_set<Symbol, Symbol::Hasher> first_set;
                non_terminals_.insert({rule.left_, std::move(first_set)});
            }
            for (const auto &sym: rule.right_) {//生成式右部插入
                if (sym.symbol_type_ == SymbolType::Terminal) {
                    if (!terminals_.count(sym)) {
                        terminals_.insert(sym);
                    }
                } else if (sym.symbol_type_ == SymbolType::Non_Terminal) {
                    if (!non_terminals_.count(sym)) {
                        unordered_set<Symbol, Symbol::Hasher> first_set;
                        non_terminals_.insert({sym, std::move(first_set)});
                    }
                } else {
                    continue;
                }
            }
        }
        if (verbose) {
            print_terminals();
            print_non_terminals();
        }
    } catch (const Exception &e) {
        cerr << e.what() << '\n';
    }
}

void Parser::generate_firsts(bool verbose = false) {
    try {
        while (true) {//开始对非终结符进行处理
            bool is_changed = false;
            for (const auto &rule: rules_) {
                int count=0;
                for(const auto& sym:rule.right_){
                    if(sym.symbol_type_==SymbolType::Terminal){
                        bool is_updated=update_first_set(rule.left_,sym);
                        if(is_updated){
                            is_changed=true;
                        }
                        count+=1;
                        break;
                    }else if(sym.symbol_type_==SymbolType::Nil){
                        bool is_updated= update_first_set(rule.left_,sym);
                        if(is_updated){
                            is_changed= true;
                        }
                        count+=1;
                        break;
                    }else if(sym.symbol_type_==SymbolType::Non_Terminal){
                        bool is_updated= update_first_set(rule.left_,sym);
                        if(is_updated){
                            is_changed=true;
                        }
                        count+=1;
                        if(!can_to_nil(sym)){
                            break;
                        }
                        if(count==rule.right_.size()&& can_to_nil(sym)){
                            bool res=update_first_set(rule.left_,NIL);
                            if(res){
                                is_changed=true;
                            }
                        }
                    }
                }
            }
            if(verbose){
                print_firsts();
            }
            if(!is_changed){
                break;
            }
        }
    } catch (const Exception &e) {
        cout << e.what() << '\n';
    }
}


bool Parser::update_first_set(const Symbol& non_terminal,const Symbol& sym){
    bool is_updated = false;
    try {
        auto it = non_terminals_.find(non_terminal);
        if (it == non_terminals_.end()) {
            throw Exception("Can not find the non_terminal: ", non_terminal.content_);
        }
        if (sym.symbol_type_ == SymbolType::Nil) {
            if(!can_to_nil(non_terminal)){
                auto res=it->second.insert(NIL);
                if(res.second){
                    is_updated=true;
                }
            }
        } else if (sym.symbol_type_ == SymbolType::Terminal) {
            auto i=it->second.find(sym);
            if(i==it->second.end()){
                auto res=it->second.insert(sym);
                if(res.second){
                    is_updated=true;
                }
            }
        } else if (sym.symbol_type_ == SymbolType::Non_Terminal) {
            auto i=non_terminals_.find(sym);
            for(const auto& item:i->second){
                if(item.symbol_type_==SymbolType::Nil){
                    continue;
                }
                auto res=it->second.insert(item);
                if(res.second){
                    is_updated=true;
                }
            }

        }
    } catch (const Exception &e) {
        cerr << e.what() << '\n';
    }
    return is_updated;

}

bool Parser::can_to_nil(const Symbol &non_terminal) {
    try{
        auto it=non_terminals_.find(non_terminal);
        if(it==non_terminals_.end()){//若找不到
            throw Exception("Can not find the non_terminal: ",non_terminal.content_);
        }
        bool res=it->second.count(NIL);
        if(res){
            return true;
        }
    }catch(const Exception& e){
        cerr<<e.what()<<'\n';
    }
    return false;
}

void Parser::generate_DFA(bool verbose = false) {
    //第一个项目
    Item start_item(rules_[0]);
    //前向搜索符
    start_item.insert_fronts(FRONT_SEARCH);
    int start_index=0;
    ItemSet start_set(start_index,*this);
    start_set.insert(start_item);
    start_set.fill();
    queue<ItemSet> q;//辅助队列
    q.push(start_set);
    item_sets_.push_back(start_set);
    while(!q.empty()){
        ItemSet now=q.front();
        q.pop();
        unordered_set<Item,Item::Hasher>& items=now.items_;
        unordered_map<Item const*,bool> transferred;
        for(const auto& item:items){
            transferred.insert({&item, false});
        }
        for(const auto& item:items){
            if(transferred[&item]){
                continue;
            }
            if(!Item::can_trans(item)){
                continue;
            }
            ItemSet new_item_set(item_sets_.size(),*this);
            Symbol next_sym=item.next_sym();
            for(const auto& item1:items){
                if(item1.next_sym()== next_sym&&Item::can_trans(item1)){
                    new_item_set.insert(Item::move_pointer(item1));
                    transferred.insert({&item1, true});
                }
            }
            new_item_set.fill();
            int to= is_existed(new_item_set);
            if(to!=-1){
                transfer_[now.state_].insert({next_sym,to});
            }else{
                item_sets_.push_back(new_item_set);
                q.push(new_item_set);
                transfer_[now.state_].insert({next_sym,item_sets_.size()});
            }
        }
    }
    if(verbose){
        print_item_sets();
    }
}


int Parser::is_existed(const ItemSet &item_set) {
    for(int i=0;i<item_sets_.size();i++){
        if(item_sets_[i]==item_set){
            return i;
        }
    }
    return -1;
}


void Parser::print_firsts() {
    cout << "=========================firsts=========================" << '\n';
    for (const auto &non_terminal: non_terminals_) {
        cout << non_terminal.first.content_ << ": ";
        string first = "{";
        for (const auto &item: non_terminal.second) {
            first += item.content_;
            first += ',';
        }
        if (first.back() == ',') {
            first.pop_back();
        }
        first += '}';
        cout << first << '\n';
    }
    cout << '\n';
}

void Parser::print_to_nil() {
    cout << "=========================to_nil=========================" << '\n';
    for (const auto &item: non_terminals_) {
        cout << item.first.content_ << ": ";
        if (can_to_nil(item.first)) {
            cout << "true\n";
        } else {
            cout << "false\n";
        }
    }
}

void Parser::print_terminals() {
    cout << "=========================terminal=========================" << '\n';
    cout << "terminals_count: " << terminals_.size() << '\n';
    for (const auto &terminal: terminals_) {
        cout << terminal.content_ << '\t';
    }
    cout << '\n';
}

void Parser::print_non_terminals() {
    cout << "=========================non_terminal=========================" << '\n';
    cout << "non_terminals_count: " << non_terminals_.size() << '\n';
    for (const auto &item: non_terminals_) {
        cout << item.first.content_ << '\t';
    }
    cout << '\n';
}

void Parser::print_rules() {
    cout << "=========================rules=========================" << '\n';
    cout << "rules_count: " << rules_.size() << '\n';
    for (const auto &rule: rules_) {
        rule.print();
    }
    cout << '\n';
}

void Parser::print_item_sets() {
    cout << "=========================item_sets=========================" << '\n';
    cout<<"item_sets_count: "<<item_sets_.size()<<"\n\n";
    for(const auto& item_set:item_sets_){
        cout<<ItemSet::to_string(item_set);
        cout<<'\n';
    }
}



int main(int argc, char *argv[]) {
    Parser parser(R"(C:\Users\jgss9\Desktop\VCompiler\parser\test\test_4.txt)");
    return 0;
}


