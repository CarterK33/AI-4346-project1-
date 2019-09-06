//
// Created by apshi on 9/2/2019.
//
// New implementation of backward chaining using C++ objects and more efficient searching


#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using namespace std;


int main(){

    struct VarList {
        vector <string> ifVarList;
        vector <string> thenVarList;
    };

    struct clauseVarInfo {
        vector <string> clauseVarList;
        pair <string, int> cmpList;
    };

    VarList varList;
    clauseVarInfo clausVarInfo;
    vector <string> conclusionList;
    pair <int, string> clauseStack;

    ifstream file;
    string line;
    string ruleStr;

    file.open("KnowledgeBase.txt");
    if (!file) {
        cout << "Unable to open file";
        exit(1); // terminate with error
    }

    while (getline(file, line)) {

        if (!line.empty()) {
            ruleStr.append(line);
        }
        else {
            ruleStr.s
            ruleStr = "";
        }

    }
    file.close();

}
