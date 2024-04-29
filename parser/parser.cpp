#include <cassert>
#include "parser.hpp"

Element::Element(ElementType element_type, int index) : element_type_(element_type), index_(index) {

}

Element::Element() {
    element_type_ = ElementType::Error;
    index_ = -1;
}

string Element::to_string(const Element &element) {
    string content;
    switch (element.element_type_) {
        case ElementType::Error:
            content += "\\";
            break;
        case ElementType::Accept:
            content += "acc";
            break;
        case ElementType::Goto:
            content += std::to_string(element.index_);
            break;
        case ElementType::Move:
            content += ('S' + std::to_string(element.index_));
            break;
        case ElementType::Reduce:
            content += ('r' + std::to_string(element.index_));
            break;
    }
    return content;
}

Parser::Parser(const string &rules_file_name) : rules_file_name_(rules_file_name) {
    generate_rules(rules_file_name_, true);
    generate_terminals_and_non_terminals(true);
    generate_firsts(true);
    generate_DFA(true);
    generate_LR1(true);
}

Parser::Parser(vector<Rule> &&rules) : rules_(std::move(rules)) {
    generate_terminals_and_non_terminals(true);
    generate_firsts(true);
    generate_DFA(true);
    generate_LR1(true);
}

//初始化语法分析器
//void Parser::init() {
//    generate_rules(rules_file_name_, true);
//    generate_terminals_and_non_terminals(true);
//    generate_firsts(true);
//    generate_DFA(true);
//    generate_LR1(true);
//}
//根据规则文件生成Rule对象
void Parser::generate_rules(const string &rules_file_name, bool verbose = false) {
    try {
        ifstream rules_file(rules_file_name);
        if (!rules_file.is_open()) {
            throw Exception("Can not open the file:", rules_file_name);
        }
        string line;
        while (getline(rules_file, line)) {
            Rule rule = Rule::to_rule(static_cast<int>(rules_.size()), line);
            rules_.push_back(std::move(rule));
            cout << line << std::endl;
        }
        rules_file.close();
        if (verbose) {
            print_rules();
        }
    } catch (const Exception &e) {
        cerr << e.what() << '\n';
    }
}

//生成终极符与非终结符集
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

//生成first集
void Parser::generate_firsts(bool verbose = false) {
    try {
        while (true) {//开始对非终结符进行处理
            bool is_changed = false;
            for (const auto &rule: rules_) {
                int count = 0;
                for (const auto &sym: rule.right_) {
                    if (sym.symbol_type_ == SymbolType::Terminal || sym.symbol_type_ == SymbolType::Nil) {
                        bool is_updated = update_first_set(rule.left_, sym);
                        if (is_updated) {
                            is_changed = true;
                        }
                        count += 1;
                        break;
                    }
//                    else if (sym.symbol_type_ == SymbolType::Nil) {
//                        bool is_updated = update_first_set(rule.left_, sym);
//                        if (is_updated) {
//                            is_changed = true;
//                        }
//                        count += 1;
//                        break;
//                    }
                    else if (sym.symbol_type_ == SymbolType::Non_Terminal) {
                        bool is_updated = update_first_set(rule.left_, sym);
                        if (is_updated) {
                            is_changed = true;
                        }
                        count += 1;
                        if (!can_to_nil(sym)) {
                            break;
                        }
                        if (count == rule.right_.size() && can_to_nil(sym)) {
                            bool res = update_first_set(rule.left_, NIL);
                            if (res) {
                                is_changed = true;
                            }
                        }
                    }
                }
            }
            if (verbose) {
                print_firsts();
            }
            if (!is_changed) {
                break;
            }
        }
    } catch (const Exception &e) {
        cerr << e.what() << '\n';
    }
}

//将sym插入non_terminal的first集中
bool Parser::update_first_set(const Symbol &non_terminal, const Symbol &sym) {
    bool is_updated = false;
    try {
        auto it = non_terminals_.find(non_terminal);
        if (it == non_terminals_.end()) {
            throw Exception("Can not find the non_terminal: ", non_terminal.content_);
        }
        //若要插入的Sym是Nil,则通过can_to_nil判断是否已插入过，否则则插入。
        if (sym.symbol_type_ == SymbolType::Nil) {
            if (!can_to_nil(non_terminal)) {
                auto res = it->second.insert(NIL);
                if (res.second) {
                    is_updated = true;
                }
            }
        } else if (sym.symbol_type_ == SymbolType::Terminal) {//若插入终结符，则判断一下是否已有，若没有则直接插入
            auto i = it->second.find(sym);
            if (i == it->second.end()) {
                auto res = it->second.insert(sym);
                if (res.second) {
                    is_updated = true;
                }
            }
        } else if (sym.symbol_type_ == SymbolType::Non_Terminal) {//若插入非终结符，则将其first集中除Nil之外的终结符插入
            auto i = non_terminals_.find(sym);
            for (const auto &item: i->second) {
                if (item.symbol_type_ == SymbolType::Nil) {
                    continue;
                }
                auto res = it->second.insert(item);
                if (res.second) {
                    is_updated = true;
                }
            }

        }
    } catch (const Exception &e) {
        cerr << e.what() << '\n';
    }
    return is_updated;
}

//判断非终结符能否推出空
bool Parser::can_to_nil(const Symbol &non_terminal) {
    try {
        auto it = non_terminals_.find(non_terminal);
        if (it == non_terminals_.end()) {//若找不到
            throw Exception("Can not find the non_terminal: ", non_terminal.content_);
        }
        bool res = it->second.count(NIL);
        if (res) {
            return true;
        }
    } catch (const Exception &e) {
        cerr << e.what() << '\n';
    }
    return false;
}

//生成项目集簇
void Parser::generate_DFA(bool verbose = false) {
    //第一个项目
    Item start_item(rules_[0]);
    //前向搜索符
    start_item.insert_fronts(FRONT_SEARCH);
    int start_index = 0;
    ItemSet start_set(start_index, *this);
    start_set.insert(start_item);
    start_set.fill();
    queue<ItemSet> q;//辅助队列
    q.push(start_set);
    item_sets_.push_back(start_set);
    while (!q.empty()) {
        ItemSet now = q.front();
        q.pop();
        unordered_set<Item, Item::Hasher> &items = now.items_;
        unordered_map<Item const *, bool> transferred;//用于标记某个Item是否转移过
        for (const auto &item: items) {
            transferred.insert({&item, false});
        }
        for (const auto &item: items) {
            if (transferred[&item]) {
                continue;
            }
            if (!Item::can_trans(item)) {
                continue;
            }
            ItemSet new_item_set(item_sets_.size(), *this);
            //发生转移的字符
            Symbol next_sym = item.next_sym();
            for (const auto &item1: items) {//遍历项目，找到点后的第一个符号为转移字符的项目
                if (item1.next_sym() == next_sym && Item::can_trans(item1)) {
                    new_item_set.insert(Item::move_pointer(item1));
                    transferred.insert({&item1, true});
                }
            }
            new_item_set.fill();
            int to = is_existed(new_item_set);
            if (to != -1) {
                transfer_[now.state_].insert({next_sym, to});
            } else {
                transfer_[now.state_].insert({next_sym, item_sets_.size()});
                item_sets_.push_back(new_item_set);
                q.push(new_item_set);
            }
        }
    }
    if (verbose) {
        print_item_sets();
        print_transfer();
    }
}

//返回项目集是否已存在，若返回为-1则表示不存在，存在则返回其idx
int Parser::is_existed(const ItemSet &item_set) {
    for (int i = 0; i < item_sets_.size(); i++) {
        if (item_sets_[i] == item_set) {
            assert(i == item_sets_[i].state_);
            return i;
        }
    }
    return -1;
}


void Parser::print_firsts() {
    cout << "=========================firsts=========================" << std::endl;
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
        cout << first << std::endl;
    }
    cout << std::endl;
}

void Parser::print_to_nil() {
    cout << "=========================to_nil=========================" << std::endl;
    for (const auto &item: non_terminals_) {
        cout << item.first.content_ << ": ";
        if (can_to_nil(item.first)) {
            cout << "true\n";
        } else {
            cout << "false\n";
        }
    }
    cout << std::endl;
}

void Parser::print_terminals() {
    cout << "=========================terminal=========================" << std::endl;
    cout << "terminals_count: " << terminals_.size() << std::endl;
    for (const auto &terminal: terminals_) {
        cout << terminal.content_ << '\t';
    }
    cout << std::endl;
}

void Parser::print_non_terminals() {
    cout << "=========================non_terminal=========================" << std::endl;
    cout << "non_terminals_count: " << non_terminals_.size() << std::endl;
    for (const auto &item: non_terminals_) {
        cout << item.first.content_ << std::endl;
    }
    cout << std::endl;
}

void Parser::print_rules() {
    cout << "=========================rules=========================" << std::endl;
    cout << "rules_count: " << rules_.size() << std::endl;
    for (const auto &rule: rules_) {
        rule.print();
    }
    cout << std::endl;
}

void Parser::print_item_sets() {
    cout << "=========================item_sets=========================" << std::endl;
    cout << "item_sets_count: " << item_sets_.size() << '\n' << std::endl;
    for (const auto &item_set: item_sets_) {
        cout << ItemSet::to_string(item_set);
        cout << std::endl;
    }
}

void Parser::print_transfer() {
    cout << "=========================transfer=========================" << std::endl;
    cout << "item_sets_count: " << item_sets_.size() << '\n' << std::endl;
    for (const auto &item: transfer_) {
        cout << "state: " << item.first << std::endl;
        for (const auto &sym: item.second) {
            cout << sym.first.content_ << ":" << sym.second << std::endl;
        }
        cout << std::endl;
    }
    cout << std::endl;
}

//获得可接受的状态idxs
vector<int> Parser::find_acc_state() {
    vector<int> res;
    //终结item S->S·,{#}
    Item end_item(rules_[0]);
    end_item.pointer_ = end_item.right_.size();
    end_item.fronts_.insert(FRONT_SEARCH);

    for (const auto &item_set: item_sets_) {
        for (const auto &item: item_set.items_) {
            if (item == end_item) {
                res.push_back(item_set.state_);
            }
        }
    }
    return res;
}

//生成LR1转移表
void Parser::generate_LR1(bool verbose = false) {
    //初始化goto表与action表
    for (const auto &item_set: item_sets_) {//对于每个状态，初始化其为error
        unordered_map<Symbol, Element, Symbol::Hasher> action_content;
        for (const auto &terminal: terminals_) {
            action_content.insert({terminal, Element(ElementType::Error, -1)});
        }
        action_.insert({item_set.state_, std::move(action_content)});
        unordered_map<Symbol, Element, Symbol::Hasher> goto_content;
        for (const auto &non_terminal: non_terminals_) {
            goto_content.insert({non_terminal.first, Element(ElementType::Error, -1)});
        }
        goto_.insert({item_set.state_, std::move(goto_content)});
    }
    //对于每个项目集中的每个项目
    for (auto &item_set: item_sets_) {
        for (auto &item: item_set.items_) {
            if (item.is_movable()) {//移进项目
                Symbol trans_sym = item.next_sym();
                int to_state = transfer_[item_set.state_][trans_sym];
                //指定转移状态
                action_[item_set.state_][trans_sym] = Element(ElementType::Move, to_state);
            } else if (item.is_reducible()) {//规约项目
                //对于每一个前向搜索符
                for (const auto &front: item.fronts_) {
                    //指定规约式子序号
                    action_[item_set.state_][front] = Element(ElementType::Reduce, item.index_);
                }
            } else {//todo some fault
                Symbol trans_sym = item.next_sym();
                try {
                    if (trans_sym.symbol_type_ != SymbolType::Non_Terminal) {
                        throw Exception("Goto trans symbol must be non_terminal", "");
                    }
                    int to = transfer_[item_set.state_][trans_sym];
                    goto_[item_set.state_][trans_sym] = Element(ElementType::Goto, to);
                } catch (const Exception &e) {
                    cerr << e.what() << '\n';
                }
            }
        }
    }
    vector<int> acc_state = find_acc_state();
    for (int state: acc_state) {
        action_[state][FRONT_SEARCH] = Element(ElementType::Accept, -1);
    }
    if (verbose) {
        print_goto_and_action();
    }
}

void Parser::print_goto_and_action() {
    cout << "=========================action and goto=========================" << std::endl;
    cout << "row count: " << action_.size() << std::endl;
    string col_head;
    col_head += "state\t";
    for (const auto &terminal: terminals_) {
        col_head += (terminal.content_ + '\t');
    }
    col_head += Front_Search + '\t';
    for (const auto &non_terminal: non_terminals_) {
        if (non_terminal.first.content_.back() == '\'') {
            continue;
        }
        col_head += (non_terminal.first.content_ + '\t');
    }
    cout << col_head << std::endl;
    for (int i = 0; i < action_.size(); i++) {
        string row_content;
        row_content += to_string(i) + '\t';
        for (const auto &terminal: terminals_) {
            row_content += Element::to_string(action_[i][terminal]) + '\t';
        }
        row_content += Element::to_string(action_[i][FRONT_SEARCH]) + '\t';
        for (const auto &non_terminal: non_terminals_) {
            if (non_terminal.first.content_.back() == '\'') {
                continue;
            }
            row_content += Element::to_string(goto_[i][non_terminal.first]) + '\t';
        }
        cout << row_content << std::endl;
    }
    cout << std::endl;
}

Symbol Parser::make_fake_symbol(const Symbol &origin, const unordered_map<string, const Token &> &sym_token_mp) {
    Symbol fake_symbol = origin;
    auto p = sym_token_mp.find(fake_symbol.content_);
    if (p == sym_token_mp.end()) {
        return fake_symbol;
    }
    TokenType type = p->second.type_;
    switch (type) {
        case TokenType::Constant:
            fake_symbol.content_ = "[constant]";
            break;
        case TokenType::Operator:
            if (!terminals_.count(fake_symbol)) {
                fake_symbol.content_ = "[operator]";
            }
            break;
        case TokenType::Identifier:
            fake_symbol.content_ = "[identifier]";
            break;
        case TokenType::Delimiter:
            break;
        case TokenType::Keyword:
            break;
    }
    return fake_symbol;
}


void Parser::analyze(const list<Symbol> &input, const unordered_map<string, const Token &> &sym_token_mp,
                     bool verbose = false) {//todo lr1分析
    //初始化输入串
    try {
        list<Symbol> str = input;
        Symbol end(FRONT_SEARCH);
        str.push_back(end);
        //初始化状态栈与符号栈,为了方便展示内容，不使用std::stack,使用std::list，其中尾部为栈顶，头部为栈底
        list<int> state_stack;
        list<Symbol> symbol_stack;
        state_stack.push_back(0);
        symbol_stack.push_back(FRONT_SEARCH);
        int step = 0;
        bool is_acc = false;
        bool is_error = false;
        while (true) {
            int cur_state = state_stack.back();
            Symbol top_sym = str.front();
            Element element;
            if (top_sym.symbol_type_ == SymbolType::Terminal || top_sym.symbol_type_ == SymbolType::Front) {
                Symbol fake_sym = make_fake_symbol(top_sym, sym_token_mp);
                element = action_[cur_state][fake_sym];
            } else if (top_sym.symbol_type_ == SymbolType::Non_Terminal) {
                element = goto_[cur_state][top_sym];
            }
            switch (element.element_type_) {
                case ElementType::Move: {
                    state_stack.push_back(element.index_);
                    symbol_stack.push_back(top_sym);
                    str.pop_front();
                    break;
                }
                case ElementType::Reduce: {
                    const Rule &reduce_rule = rules_[element.index_];
                    int pop_size = static_cast<int>(reduce_rule.right_.size());
                    if (reduce_rule.is_nil_rule()) {
                        pop_size = 0;
                    }
                    for (int i = 0; i < pop_size; i++) {
                        state_stack.pop_back();
                        symbol_stack.pop_back();
                    }
                    //为了统一处理，将规约的左部放入符号串头部
                    str.push_front(reduce_rule.left_);
                    break;
                }
                case ElementType::Accept: {
                    if (str.size() != 1) {
                        throw Exception("Acc Error", "");
                    }
                    is_acc = true;
                    break;
                }
                case ElementType::Goto: {
                    symbol_stack.push_back(top_sym);
                    state_stack.push_back(element.index_);
                    str.pop_front();
                    break;
                }
                case ElementType::Error:
                    is_error = true;
                    break;
            }
            step += 1;
            if (verbose) {
                if (step == 1) {
                    cout << "step\t\tstate_stack\t\tsymbol_stack\t\tstr\t\toperation\n";
                }
                auto print_analyze = [&]() {
                    string content;
                    content += to_string(step) + "\t\t";
                    //打印状态栈
                    for (const auto &state: state_stack) {
                        content += to_string(state);
                    }
                    content += "\t\t";
                    //打印符号栈
                    for (const auto &sym: symbol_stack) {
                        content += sym.content_;
                    }
                    content += "\t\t";
                    for (const auto &sym: str) {
                        content += sym.content_;
                    }
                    content += "\t\t";
                    content += Element::to_string(element);
                    cout << content << std::endl;
                };
                print_analyze();
            }
            if (is_acc || is_error) {
                break;
            }
        }
    } catch (const Exception &e) {
        cerr << e.what() << '\n';
    }
}

//根据token文件加载tokes
list<Token> Parser::load_tokens(const string &token_file_name) {
    list<Token> tokens;
    try {
        ifstream token_file(token_file_name);
        if (!token_file.is_open()) {
            throw Exception("Can not open the file: ", token_file_name);
        }
        string line;
        getline(token_file, line);
        while (getline(token_file, line)) {
            tokens.emplace_back(Token::from_string(line));
        }
    } catch (const Exception &e) {
        cerr << e.what() << '\n';
    }
    return tokens;
}

pair<list<Symbol>, unordered_map<string, const Token &>> Parser::tokens_to_syms(const list<Token> &tokens) {
    list<Symbol> syms;
    unordered_map<string, const Token &> sym_token_mp;
    for (const Token &token: tokens) {
        string content;
        SymbolType symbol_type = SymbolType::Terminal;
        content = token.value_;
        syms.emplace_back(content, symbol_type);
        sym_token_mp.insert({content, token});
    }
    return {syms, sym_token_mp};
}

void Parser::call(const string &token_file_name) {
    list<Token> tokens = std::move(load_tokens(token_file_name));
    auto [syms, sym_token_mp] = std::move(tokens_to_syms(tokens));
    analyze(syms, sym_token_mp, true);
}

void Parser::call(const list<Token> &tokens) {
    auto [syms, sym_token_mp] = std::move(tokens_to_syms(tokens));
    analyze(syms, sym_token_mp, true);
}



