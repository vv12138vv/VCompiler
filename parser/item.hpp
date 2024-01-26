#ifndef ITEM_HPP
#define ITEM_HPP


#include"rule.hpp"

#endif

using namespace std;

class Item : public Rule {
public:
    struct Hasher {
        size_t operator()(const Item &item) const {
            return hash<string>()(item.content_) ^ hash<int>()(item.pointer_);
        }
    };

public:
    int pointer_;//点前的符号个数
    bool is_reducible_;//可规约？
    bool is_movable_;//可移入？
    unordered_set<Symbol, Symbol::Hasher> fronts_;//前向搜索符集合

    Item() = delete;

    explicit Item(const Rule &rule);

    bool insert_fronts(const Symbol &sym);
};

class ItemSet {
public:
    int state_;
    unordered_set<Item, Item::Hasher> items_;
    ItemSet()=default;
    explicit ItemSet(int state);
};
