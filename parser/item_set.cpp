#include"parser.hpp"


Item::Item(const Rule &rule) : Rule(rule) {
    pointer_ = 0;
}


bool Item::operator==(const Item &that) const {
    return that.content_ == content_ && that.pointer_ == pointer_ && that.fronts_ == fronts_;
}

bool Item::insert_fronts(const Symbol &sym) {
    bool is_inserted = false;
    try {
        if (sym.symbol_type_ == SymbolType::Front) {
            auto res = fronts_.insert(sym);
            if (res.second) {
                is_inserted = true;
            }
        } else if (sym.symbol_type_ == SymbolType::Terminal) {
            auto res = fronts_.insert(sym);
            if (res.second) {
                is_inserted = true;
            }
        } else {
            throw Exception("Wrong argument: ", sym.content_);
        }
    } catch (const Exception &e) {
        cerr << e.what() << '\n';
    }
    return is_inserted;
}

bool Item::insert_fronts(const unordered_set<Symbol,Symbol::Hasher>& syms) {
    bool is_inserted=false;
    for(const auto& sym:syms){
        auto res=fronts_.insert(sym);
        if(res.second){
            is_inserted=true;
        }
    }
    return is_inserted;
}

Symbol Item::next_sym() const {
    if (pointer_ == right_.size()) {
        return ERROR;
    }
    return right_[pointer_];
}

string Item::to_string(const Item &item) {
    string content;
    content = item.left_.content_ + "->";
    for(int i=0;i<item.right_.size();i++){
        if(i==item.pointer_){
            content+=Item_Delimieter;
        }
        content+=item.right_[i].content_;
    }
    if(item.pointer_==item.right_.size()){
        content+=Item_Delimieter;
    }
    content+=",{";
    for(const auto& sym:item.fronts_){
        content+=(sym.content_+',');
    }
    if(content.back()==','){
        content.pop_back();
    }
    content+="}";
    return content;
}

bool Item::can_trans(const Item &item) {
    if(item.right_.empty()){
        return false;
    }
    if(item.is_reducible()){
        return false;
    }
    return true;
}

Item Item::move_pointer(const Item &item) {
    Item new_item(item);
    try{
        new_item.pointer_+=1;
        if(new_item.pointer_>new_item.right_.size()){
            throw Exception("The pointer can not larger than size","");
        }
        return new_item;
    }catch(const Exception& e){
        cerr<<e.what()<<'\n';
    }
}

bool Item::is_reducible() const {
    try{
        if(pointer_> right_.size()){
            throw Exception("Pointer can not larger than right","");
        }
        if(pointer_==right_.size()){
            return true;
        }
    }catch(const Exception& e){
        cerr<<e.what()<<'\n';
    }
    return false;
}

bool Item::is_movable() const {
    try{
        if(pointer_> right_.size()){
            throw Exception("Pointer can not larger than right","");
        }
        if(pointer_==right_.size()){
            return false;
        }
    }catch(const Exception& e){
        cerr<<e.what()<<'\n';
    }
    return next_sym().symbol_type_==SymbolType::Terminal;
}

unordered_set<Symbol, Symbol::Hasher> ItemSet::get_firsts(const Rule &rule, int start, int end) {
    unordered_set<Symbol, Symbol::Hasher> first_set;
    try {
        if (start < 0 || end >= rule.right_.size()) {
            throw Exception("Wrong argument: ", "out of range");
        }
        int i = start;
        while (i <= end) {
            Symbol sym=rule.right_[i];
            if (sym.symbol_type_ == SymbolType::Terminal) {
                first_set.insert(sym);
                break;
            } else if (sym.symbol_type_ == SymbolType::Non_Terminal) {
                for(const auto& symbol:parser_.non_terminals_.find(sym)->second){
                    first_set.insert(symbol);
                }
                if(!can_be_nil(sym)){
                    break;
                }
                i+=1;
            }
        }
    } catch (const Exception &e) {
        cerr<<e.what()<<'\n';
    }
    return first_set;
}


ItemSet::ItemSet(int state, const Parser &parser) : state_(state), parser_(parser) {}

ItemSet::ItemSet(const Parser &parser):state_(-1),parser_(parser) {}

bool ItemSet::insert(const Item &item) {
    bool is_inserted = false;
    auto it = items_.find(item);
    if (it == items_.end()) {
        auto res = items_.insert(Item(item));
        if (res.second) {
            is_inserted = true;
        }
    }
    return is_inserted;
}




void ItemSet::fill() {
    list<Item> item_list(items_.begin(),items_.end());
    while(true){
        bool is_changed=false;
        for (const auto &it: item_list) {
            Symbol next_sym = it.next_sym();
            if (next_sym.symbol_type_ != SymbolType::Non_Terminal) {
                continue;
            }
            auto firsts= get_firsts(it,it.pointer_+1,it.right_.size()-1);
            for (const auto &rule: parser_.rules_) {
                if (rule.left_ == next_sym) {
                    Item new_item(rule);
                    //插入first后部进入front集
                    if(firsts.count(NIL)||firsts.empty()){
                        new_item.insert_fronts(firsts);
                        new_item.insert_fronts(it.fronts_);
                    }else{
                        new_item.insert_fronts(firsts);
                    }
                    auto res=items_.insert(new_item);
                    if(res.second){
                        is_changed=true;
                        item_list.emplace_back(new_item);
                    }
                }
            }
        }
        if(!is_changed){
            break;
        }
    }
}



bool ItemSet::can_be_nil(const Symbol &non_terminal) {
    try{
        auto it=parser_.non_terminals_.find(non_terminal);
        if(it==parser_.non_terminals_.end()){//若找不到
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


string ItemSet::to_string(const ItemSet &item_set) {
    string content;
    content+="state: ";
    content+= std::to_string(item_set.state_);
    content+='\n';
    for(const auto& item:item_set.items_){
        content+= Item::to_string(item);
        content+='\n';
    }
    return content;
}

bool ItemSet::operator==(const ItemSet &that) const {
    return items_==that.items_;
}



