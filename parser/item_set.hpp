#ifndef ITEM_SET_HPP
#define ITEM_SET_HPP


#include"rule.hpp"
#include<cassert>
#include"queue"

#endif

using namespace std;

class Parser;

//项目集中每条项目
class Item : public Rule {
public:
    //Hash函数，为了使用unordered_set/unoredered_map
    struct Hasher {
        size_t operator()(const Item &item) const {
            return hash<string>()(item.content_) ^ hash<int>()(item.pointer_);
        }
    };

public:
    //点前的符号个数
    int pointer_;
    //前向搜索符集合
    unordered_set<Symbol, Symbol::Hasher> fronts_;

    Item() = delete;
    Item(const Item& that)=default;
    explicit Item(const Rule &rule);
    bool operator==(const Item& that) const;
    //填入前向搜素符
    bool insert_fronts(const Symbol &sym);
    bool insert_fronts(const unordered_set<Symbol, Symbol::Hasher>& syms);
    //返回·后的第一个Symbol
    Symbol next_sym() const;
    //判断该项目是否可规约
    bool is_reducible() const;
    //判断该项目是否可移进
    bool is_movable() const;
    //移动·
    static Item move_pointer(const Item& item);
    static string to_string(const Item& item);
    static bool can_trans(const Item& item);
};

//项目集
class ItemSet {
public:
    //该项目集对于的idx
    int state_;
    //指向所属Parser的引用，为了方便访问Parser中的一些信息
    const Parser& parser_;
    //属于该ItemSet的项目集
    unordered_set<Item, Item::Hasher> items_;

    ItemSet()=delete;
    explicit ItemSet(const Parser& parser);
    ItemSet(int state,const Parser& parser);
    ItemSet(const ItemSet& that)=default;
    //获取指定范围内的first集合
    unordered_set<Symbol,Symbol::Hasher> get_firsts(const Rule& rule, int start, int end);

    bool operator==(const ItemSet& that) const;
    //插入项目集
    bool insert(const Item& item);
    //判断某个非终结符能否推出nil
    bool can_be_nil(const Symbol& non_terminal);
    //填充该项目集，即根据已有的项目将新项目填入
    void fill();
    static string to_string(const ItemSet& item_set);
};
