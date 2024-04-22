#ifndef ITEM_SET_HPP
#define ITEM_SET_HPP


#include"rule.hpp"
#include"queue"
#endif

using namespace std;

class Parser;

//项目集中每条项目
class Item : public Rule {
public:
    struct Hasher {
        size_t operator()(const Item &item) const {
            return hash<string>()(item.content_) ^ hash<int>()(item.pointer_);
        }
    };

public:
    int pointer_;//点前的符号个数
    unordered_set<Symbol, Symbol::Hasher> fronts_;//前向搜索符集合

    Item() = delete;
    Item(const Item& that)=default;
    explicit Item(const Rule &rule);
    bool operator==(const Item& that) const;
    bool insert_fronts(const Symbol &sym);
    bool insert_fronts(const unordered_set<Symbol, Symbol::Hasher>& syms);
    Symbol next_sym() const;
    bool is_reducible() const;
    bool is_movable() const;
    static Item move_pointer(const Item& item);
    static string to_string(const Item& item);
    static bool can_trans(const Item& item);
    //返回指定范围内的first集
};

//项目集
class ItemSet {
public:
    int state_;
    const Parser& parser_;
    unordered_set<Item, Item::Hasher> items_;


    ItemSet()=delete;
    explicit ItemSet(const Parser& parser);
    ItemSet(int state,const Parser& parser);
    ItemSet(const ItemSet& that)=default;
    //获取指定范围内的first集合
    unordered_set<Symbol,Symbol::Hasher> get_firsts(const Rule& rule, int start, int end);

    bool operator==(const ItemSet& that) const;

    bool insert(const Item& item);
    bool can_be_nil(const Symbol& non_terminal);
    void fill();
    static string to_string(const ItemSet& item_set);
};
