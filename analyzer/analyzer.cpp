#include "analyzer.hpp"




auto Analyzer::generate_rules(const string &rule_file_name) {
    vector<Rule> rules;
    Senmatic senmatics;
    try{
        ifstream rules_file(rule_file_name);
        if(!rules_file.is_open()){
            throw Exception("Can not open the file:",rule_file_name);
        }
        string line;
        int i=0;
        while(getline(rules_file,line)){
            auto it= line.find(" {");
            if(it==string::npos){
                throw Exception("The format of Rules are wrong","");
            }
            string rule_part=line.substr(0,it);
//            Rule rule=Rule::to_rule(static_cast<int>(rules.size()),rule_part);
//            rules.push_back(std::move(rule));
            string senmatic_part=line.substr(it+2,line.size()-it-1);
            cout<<"rule_part:"<<rule_part<<'\n';
            cout<<"senmatic_part: "<<senmatic_part<<"\n";
        }
        rules_file.close();
    }catch (const Exception& e){
        cerr<<e.what()<<'\n';
    }
    return;
}

Analyzer::Analyzer(const string &rule_file_name) {
    generate_rules(rule_file_name);
}

int main(){
    string rule_file_name(R"(C:\Users\jgss9\Desktop\VCompiler\analyzer\rules\rules.txt)");
    Analyzer analyzer(rule_file_name);
}

