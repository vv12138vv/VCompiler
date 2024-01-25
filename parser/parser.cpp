#include "parser.hpp"

Parser::Parser(const string &rules_file_name):rules_file_name_(rules_file_name) {
    init();
}

void Parser::generate_rules(const string &rules_file_name,bool verbose=false) {
    try {
        ifstream rules_file(rules_file_name);
        if (!rules_file.is_open()) {
            throw runtime_error("Can not open the file!");
        }
        string line;
        while(getline(rules_file,line)){
            Rule rule=Rule::to_rule(static_cast<int>(rules_.size()),line);
            rules_.push_back(std::move(rule));
        }
        rules_file.close();
        if(verbose){
            print_rules();
        }
    } catch (const exception &e) {
        cerr << e.what()<< '\n';
    }
}

void Parser::generate_terminals_and_non_terminals(bool verbose=false) {
    try{
        if(rules_.empty()){
            throw Exception("Generate Rules first","");
        }
        for(const auto& rule:rules_){
           non_terminals_.insert(rule.left_);
           for(const auto& sym:rule.right_){
               if(sym.symbol_type_==SymbolType::Ternimal){
                    terminals_.insert(sym);
               }else if(sym.symbol_type_==SymbolType::Non_Terminal){
                    non_terminals_.insert(sym);
               }else{
                   continue;
               }
           }
        }
        if(verbose){
            print_terminals();
            print_non_terminals();
        }
    }catch (const Exception& e){
        cerr<<e.what()<<'\n';
    }

}


void Parser::init() {
    generate_rules(rules_file_name_, true);
    generate_terminals_and_non_terminals(true);
    generate_firsts(true);
}

void Parser::print_terminals() {
    cout<<"=========================terminal========================="<<'\n';
    cout<<"terminals_count: "<<terminals_.size()<<'\n';
    for(const auto& terminal:terminals_){
        cout<<terminal.content_<<'\t';
    }
    cout<<'\n';
}

void Parser::print_non_terminals() {
    cout<<"=========================non_terminal========================="<<'\n';
    cout<<"non_terminals_count: "<<non_terminals_.size()<<'\n';
    for(const auto& non_terminal:non_terminals_){
        cout<<non_terminal.content_<<'\t';
    }
    cout<<'\n';
}

void Parser::print_rules() {
    cout<<"=========================rules========================="<<'\n';
    cout<<"rules_count: "<<rules_.size()<<'\n';
    for(const auto& rule:rules_){
        rule.print();
    }
    cout<<'\n';
}


void Parser::generate_firsts(bool verbose=false) {
    try{
        for(const auto& rule:rules_){//先将所有可以直接获得的空与终结符处理
            Symbol right_first_sym=rule.get_right_first();
            if(rule.left_.symbol_type_!=SymbolType::Non_Terminal){
                throw Exception("The format of rule is wrong:",Rule::to_string(rule));
            }
            if(right_first_sym.symbol_type_==SymbolType::Nil){
                set_can_to_nil(rule.left_);
            }else if(right_first_sym.symbol_type_==SymbolType::Ternimal){
                update_non_terminal_first(rule.left_,right_first_sym);
            }else{}
        }
        while(true){//开始对非终结符进行处理
            bool is_changed=false;
            for(const auto& rule:rules_){
                if(rule.right_[0].symbol_type_!=SymbolType::Non_Terminal){
                    continue;
                }
                Symbol right_first=*non_terminals_.find(rule.right_[0]);
                if(right_first.to_nil_){
                    int i=0;
                    while(rule.right_[i].symbol_type_==SymbolType::Non_Terminal&&i<rule.right_.size()){
                        if(rule.right_[i].to_nil_){
                            bool is_updated= update_non_terminal_first(rule.left_,rule.right_[i]);
                            if(is_updated){
                                is_changed=true;
                            }
                            i+=1;
                        }else{
                            bool is_updated= update_non_terminal_first(rule.left_,rule.right_[i]);
                            if(is_updated){
                                is_changed=true;
                            }
                            break;
                        }
                    }
                    if(i==rule.right_.size()){
                        set_can_to_nil(rule.left_);
                    }else{
                        if(rule.right_[i].symbol_type_==SymbolType::Ternimal){
                            bool is_updated= update_non_terminal_first(rule.left_,rule.right_[i]);
                            if(is_updated){
                                is_changed=true;
                            }
                        }
                    }
                }else{
                    bool is_updated=update_non_terminal_first(rule.left_, right_first);
                    if(is_updated){
                        is_changed=true;
                    }
                }
            }
            if(verbose){
                print_to_nil();
                print_firsts();
            }
            if(!is_changed){
                break;
            }
        }
    }catch (const Exception& e){
        cout<<e.what()<<'\n';
    }
}



bool Parser::update_non_terminal_first(const Symbol &non_terminal, const Symbol &sym) {
    bool is_updated=false;
    try{
        auto it=non_terminals_.find(non_terminal);
        if(it==non_terminals_.end()){
            throw Exception("Can not find the non_terminal: ",non_terminal.content_);
        }
        Symbol non_t=*it;
        non_terminals_.erase(it);
        if(sym.symbol_type_==SymbolType::Nil){
            non_t.to_nil_=true;
        }else if(sym.symbol_type_==SymbolType::Ternimal){
            auto res=non_t.first_.insert(sym);
            if(res.second){
                is_updated=true;
            }
        }else if(sym.symbol_type_==SymbolType::Non_Terminal){
            for(const auto& item:sym.first_){
                if(item.symbol_type_==SymbolType::Nil){
                    continue;
                }
                auto res=non_t.first_.insert(item);
                if(res.second){
                    is_updated=true;
                }
            }
        }
        non_terminals_.insert(non_t);
    }catch (const Exception& e){
        cerr<<e.what()<<'\n';
    }
    return is_updated;
}

void Parser::set_can_to_nil(const Symbol &non_terminal) {
    try{
        auto it=non_terminals_.find(non_terminal);
        if(it==non_terminals_.end()){
            throw Exception("Can not find the non_terminal: ",non_terminal.content_);
        }
        Symbol non_t=*it;
        non_terminals_.erase(it);
        non_t.to_nil_=true;
        non_terminals_.insert(std::move(non_t));
    }catch(const Exception& e){
        cerr<<e.what()<<'\n';
    }
}

void Parser::print_firsts() {
    cout<<"=========================firsts========================="<<'\n';
    for(const auto& non_terminal:non_terminals_){
        cout<<non_terminal.content_<<": ";
        string first="{";
        for(const auto& item:non_terminal.first_){
            first+=item.content_;
            first+=',';
        }
        if(first.back()==','){
            first.pop_back();
        }
        first+='}';
        cout<<first<<'\n';
    }
    cout<<'\n';
}

void Parser::print_to_nil() {
    cout<<"=========================to_nil========================="<<'\n';
    for(const auto& non_terminal:non_terminals_){
        cout<<non_terminal.content_<<": ";
        if(non_terminal.to_nil_){
            cout<<"true\n";
        }else{
            cout<<"false\n";
        }
    }
}


int main(int argc,char* argv[]){
    Parser parser(R"(C:\Users\jgss9\Desktop\classDesign2\Design-2\grammer\test-rule4.txt)");
    return 0;
}


