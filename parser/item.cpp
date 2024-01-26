#include"item.hpp"

Item::Item(const Rule &rule) : Rule(rule) {
    pointer_=0;
    is_reducible_=false;
    is_movable_=false;
}

ItemSet::ItemSet(int state):state_(state) {}
bool Item::insert_fronts(const Symbol &sym) {
    bool is_inserted=false;
    if(sym.symbol_type_==SymbolType::Front){
        auto res=fronts_.insert(sym);
        if(res.second){
            is_inserted= true;
        }
    }else if(sym.symbol_type_==SymbolType::Ternimal){
        auto res=fronts_.insert(sym);
        if(res.second){
            is_inserted=true;
        }
    }else if(sym.symbol_type_==SymbolType::Non_Terminal){
    }
    return is_inserted;
}

