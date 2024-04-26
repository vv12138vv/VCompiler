#include"rule.hpp"


Rule::Rule(int index, Symbol &&left, vector<Symbol> &&right)
        : index_(index), left_(std::forward<Symbol>(left)), right_(std::forward<vector<Symbol>>(right)) {
    content_ = to_string(*this);
}

Rule Rule::to_rule(int index, const string &rule_string) {
    try {
        size_t i = rule_string.find("->");
        if (index == string::npos) {
            throw Exception("The format of rule is error:", rule_string);
        }
        string left = rule_string.substr(0, i);
        string right = rule_string.substr(i + 2, rule_string.length() - i - 2);
        string non_terminal = left.substr(1, left.size() - 2);
        Symbol non_terminal_sym(non_terminal, SymbolType::Non_Terminal);
        vector<Symbol> right_syms;
        int j = 0;
        while (j < right.size()) {
            if (right[j] == non_terminal_left_delimiter) {
                int k = j + 1;
                while (right[k] != non_terminal_right_delimiter) {
                    k += 1;
                }
                string content = right.substr(j + 1, k - j - 1);
                Symbol non_termial_sym(content, SymbolType::Non_Terminal);
                right_syms.push_back(std::move(non_termial_sym));
                j = k + 1;
                continue;
            }
            if (right[j] == terminal_left_delimiter) {
                int k = j + 1;
                while (right[k] != terminal_left_delimiter) {
                    k += 1;
                }
                string content = right.substr(j + 1, k - j - 1);
                if (content.length() == 1 && content == Nil) {
                    Symbol terminal_sym(content, SymbolType::Nil);
                    right_syms.push_back(std::move(terminal_sym));
                } else {
                    Symbol terminal_sym(content, SymbolType::Terminal);
                    right_syms.push_back(std::move(terminal_sym));
                }
                j = k + 1;
                continue;
            }
        }
        Rule rule(index, std::move(non_terminal_sym), std::move(right_syms));
        return rule;
    } catch (const Exception &e) {
        cerr << e.what() << '\n';
    }
}

void Rule::print() const {
    string rule_string = std::move(to_string(*this));
    cout << index_ << ": " << rule_string;
    cout << '\n';
}

bool Rule::is_nil_rule() const {
    return right_.size()==1&&right_[0]==NIL;
}

string Rule::to_string(const Rule &rule) {
    string res;
    res = rule.left_.content_ + "->";
    for (const auto &sym: rule.right_) {
        res += sym.content_;
    }
    return res;
}

Symbol Rule::get_right_first() const {
    Symbol right_first = right_[0];
    return right_first;
}

Symbol::Symbol(string content, SymbolType symbol_type) : content_(std::move(content)),
                                                         symbol_type_(symbol_type) {
}

bool Symbol::operator==(const Symbol &that) const {
    return content_ == that.content_ && symbol_type_ == that.symbol_type_;
}

Symbol Symbol::make_fake_symbol(const Symbol &origin, const unordered_map<string, const Token &> &sym_token_mp) {
    Symbol fake_symbol=origin;
    auto p=sym_token_mp.find(fake_symbol.content_);
    if(p==sym_token_mp.end()){
        return fake_symbol;
    }
    TokenType type=p->second.type_;
    switch(type){
        case TokenType::Constant:
            fake_symbol.content_="[constant]";
            break;
        case TokenType::Operator:
            if(fake_symbol.content_!="->"&&fake_symbol.content_!="="){
                fake_symbol.content_="[operator]";
            }
            break;
        case TokenType::Identifier:
            fake_symbol.content_="[identifier]";
            break;
        case TokenType::Delimiter:
            break;
        case TokenType::Keyword:
            break;
    }
    return fake_symbol;
}


