#include"lexer.hpp"

//读取生成文件
vector<string> AutoMachine::read_rules(std::string file_name, std::string label) {
    try{
        ifstream rules_file(file_name);
        if(!rules_file.is_open()){
            throw runtime_error("Can not open the file!");
        }
        vector<string> rules_string;
        string line;
        getline(rules_file,line);
        while(line!=label){//连续读取直至目标行
            getline(rules_file,line);
        }
        while(rules_file.peek()!=EOF){
            getline(rules_file,line);
            rules_string.emplace_back(line);
            if(rules_file.peek()=='['){
                break;
            }
        }
        rules_file.close();
        return rules_string;
    }catch(const exception& e){
        cerr<<e.what()<<'\n';
    }
}

AutoMachine::AutoMachine(string rules_file_name) {

}


