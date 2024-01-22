#include"rule.hpp"


Rule::Rule(int index, Symbol&& left, vector<Symbol>&& right)
        : index_(index), left_(std::forward<Symbol>(left)), right_(std::forward<vector<Symbol>>(right))
{
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
        Symbol non_terminal_sym(non_terminal,SymbolType::Non_Terminal);
        vector<Symbol> right_syms;
        int j=0;
        while(j<right.size()){
            if(right[j]==non_terminal_left_delimiter){
                int k=j+1;
                while(right[k]!=non_terminal_right_delimiter){
                    k+=1;
                }
                string content=right.substr(j+1,k-j-1);
                Symbol non_termial_sym(content,SymbolType::Non_Terminal);
                right_syms.push_back(std::move(non_termial_sym));
                j=k+1;
                continue;
            }
            if(right[j]==terminal_left_delimiter){
                int k=j+1;
                while(right[k]!=terminal_left_delimiter){
                    k+=1;
                }
                string content=right.substr(j+1,k-j-1);
                if(content.length()==1&&content[0]==Nil){
                    Symbol terminal_sym(content,SymbolType::Nil);
                    right_syms.push_back(std::move(terminal_sym));
                }else{
                    Symbol terminal_sym(content,SymbolType::Ternimal);
                    right_syms.push_back(std::move(terminal_sym));
                }
                j=k+1;
                continue;
            }
        }
        Rule rule(index,std::move(non_terminal_sym),std::move(right_syms));
        return rule;
    } catch (const Exception &e) {
        cerr << e.what() << '\n';
    }
}

void Rule::print() const {
    cout<<index_<<": "<<left_.content_<<"->";
    for(const auto& sym:right_){
        cout<<sym.content_;
    }
    cout<<'\n';
}

Symbol::Symbol(string content, SymbolType symbol_type) : content_(std::move(content)),
                                                         symbol_type_(symbol_type) {
    to_nil_=false;
}

