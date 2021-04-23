//
// Created by apshi on 9/2/2019.
//
// New implementation of backward chaining using C++ objects and more efficient searching


#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <fstream>
#include <algorithm>
#include "clausevarinfo.h"
#include "rulevarinfo.h"

using namespace std;

inline pair<int, pair<int, string>> getVarInfo(RuleVarInfo&, string&);
inline int getClauseIndex(int&, const int&);
inline int conditionToInt(string&);
inline bool isNumeric(const string&);
inline string toUpper(string&);
inline void print(ClauseVarInfo&, RuleVarInfo&);
inline void extractRules(vector <string>&);
inline void fillDataStructs(vector <string>&, ClauseVarInfo&, RuleVarInfo&, const int&);

int main(){

    const int NUM_OF_CLAUSES = 4; // Highest number of if clauses in rule base
    vector <string> ruleString; // Contains extracted data from text file rules
    ClauseVarInfo cvi; // Contains all info regarding clause variables
    RuleVarInfo rvi; // Contains all info regarding rules
    deque <pair<string,string>*> cs; // Conclusion stack

    extractRules(ruleString);
    fillDataStructs(ruleString, cvi, rvi, NUM_OF_CLAUSES);

    string searchTerm;
    cout << "\nWhat would you like to know?\n";
    cin >> searchTerm;
    toUpper(searchTerm);

    int idx = 0; // Keeps track of index in range-based loops
    int clauseIdx = 0; // First clause related to current rule
    bool contains = false; // True - Element found ; False - Element not found

/*Check if value from user is inside of a rule and add to stack if found*/
    for(auto &p : rvi.cl) {
        if (searchTerm == p.first) {
            clauseIdx = getClauseIndex(idx, NUM_OF_CLAUSES);
            cs.emplace_front(&p);
            contains = true;
            break;
        } ++idx;
    } if(!contains){
        cout << "I could not find this conclusion. Try again.";
        exit(1);
      }

    string clauseName = cvi.cvl[clauseIdx]; // Index for current rule clause variables in cvl
    int stackPtr = 0; // Index for current element in stack being processed

    idx = 0;
    pair<int, pair<int, string>> varInfo; // Contains data for current clause var in relation to var lists
    int varIdx; // Index for var in the list it was found in
    bool instantiated;   // True - not instantiated; False - instantiated
    string list; // IF - in IF var list; THEN - in THEN var list
    string *varValue; // Points to value for given variable found

    // Clause comparison strings and values
    string clauseValue;
    float clauseValueNum;
    string condition;
    int conditionNum;
    string compareValue;
    float compareValueNum;
    string answer;
    string numMessage = "(Enter numeric value)"; // If value entered by user will be a string
    string strMessage = "(YES/NO)"; // If value entered by user will be a float
    bool cont; // Flag to break out of while
    int currRule; // Current rule number

/*Search lists for containing clause var and collect data when found*/
    while(stackPtr > -1) {
        for (int j = 0; j < NUM_OF_CLAUSES; ++j) {

            clauseName = cvi.cvl[clauseIdx + j];

            if (clauseName.empty() || j ==3) break; // No more clauses to process

            cont = true;
            varInfo = getVarInfo(rvi, clauseName);
            varInfo.first == 0 ? list = "IF" : list = "THEN";
            !varInfo.second.second.empty() ? instantiated = true : instantiated = false;
            varIdx = varInfo.second.first;
            varValue = &rvi.ivl[varIdx].second;

/*Test for list and NI status*/
            if (varInfo.first != -1) {
                if (list == "IF" && !instantiated) {
                    cout << "Please enter value for " << clauseName << ": \n";
                    cin >> *varValue;
                    toUpper(*varValue);
                } else if (list == "THEN" && !instantiated) {

 /*Find the rule for clause variable in conclusion list and add to the stack*/
                    idx = 0;
                    for (auto &p : rvi.cl) {
                        if (clauseName == p.first) {
                            clauseIdx = getClauseIndex(idx, NUM_OF_CLAUSES);
                            ++stackPtr;
                            cs.emplace_back(&p);
                            j = -1;
                            break;
                        } ++idx;
                    }
                }
            } else exit(3); // Var not found error
        }

/*See if all clause conditions are met for current conclusion on stack*/
        for(int k=0;k<NUM_OF_CLAUSES;++k) {

            answer = "";
            clauseName = cvi.cvl[clauseIdx + k];

            if(clauseName.empty()) {
                if(stackPtr < 0) break;
                idx = 0;
                for (auto &p : rvi.cl) {
                    if (cs[stackPtr]->first == p.first) break;
                    ++idx;
                } break;
            }


            varInfo = getVarInfo(rvi, clauseName);
            clauseValue = varInfo.second.second;
            condition = cvi.cndOp[clauseIdx + k];
            conditionNum = conditionToInt(condition);
            compareValue = cvi.cmpVal[clauseIdx + k];

            varInfo = getVarInfo(rvi, cs[stackPtr]->first);
            varIdx = varInfo.second.first;
            if(varInfo.first == 0) {
                varValue = &rvi.ivl[varIdx].second;
            } else
                varValue = &rvi.tvl[varIdx].second;

            if(!isNumeric(compareValue)){
                if(clauseValue == compareValue){

/*Set variable value to the value stored if all clauses are true*/
                    if(k == 3 || cvi.cvl[clauseIdx + k + 1].empty()){
                        *varValue = cs[stackPtr]->second;
                        answer = *varValue;
                        break;
                    }
                } else
                    break;
            } else {
                clauseValueNum = stof(clauseValue);
                compareValueNum = stof(compareValue);
                conditionNum = conditionToInt(condition);

                switch(conditionNum){
                    case 0:
                        break;
                    case 1:
                        if(clauseValueNum < compareValueNum) {

/*Set variable value to the value stored if all clauses are true*/
                            if (k == 3 || cvi.cvl[clauseIdx + k + 1].empty()) {
                                *varValue = cs[stackPtr]->second;
                                answer = *varValue;
                            }
                        } else {
                            cont = false;
                        }
                        break;
                    case 2:
                        break;
                    case 3:
                        break;
                    case 4:
                        if(clauseValueNum >= compareValueNum) {

 /*Set variable value to the value stored if all clauses are true*/
                            if (k == 3 || cvi.cvl[clauseIdx + k + 1].empty()) {
                                *varValue = cs[stackPtr]->second;
                                answer = *varValue;
                            }
                        } else {
                            cont = false;
                        }
                        break;
                }
                if(!answer.empty() || !cont)
                    break;
            }
        }

        if(!answer.empty() && stackPtr<=0) {
            cout << searchTerm << " = " << answer;
            exit(4);
        } else if(stackPtr<0){
            break;
        }

/*Go to next rule that has the same conclusion as the current rule. Replace on stack if found.
 * If none, pop the stack*/
        currRule = ((clauseIdx/NUM_OF_CLAUSES)+1)*10;
        idx = 0;
        for (auto &p : rvi.cl) {
            if (cs[stackPtr]->first == p.first && ((idx+1)*10) > currRule) {
                clauseIdx = getClauseIndex(idx, NUM_OF_CLAUSES);
                cs[stackPtr] = &p;
                break;
            }
            ++idx;
        }
        if(idx != rvi.cl.size()) {
            continue;
        }

        cs.pop_back();
        --stackPtr;

        clauseName = cs[stackPtr]->first;
/*Reset clause index*/
        idx = 0;
        for (auto &p : rvi.cl) {
            if (clauseName == p.first && idx >= clauseIdx) {
                clauseIdx = getClauseIndex(idx, NUM_OF_CLAUSES);
                break;
            } ++idx;
        }
    }
}

/* Searches and returns the appropriate list info for a given list variable
 *
 * rvi - the rule variable info lists
 * s - string variable value to be searched
 *
 * Pair returned - First value indicates list returned from (0 for if, 1 for then)
 * Second value is a pair containing the index where the variable was found
 * and then the value for that variable
 *
 */
inline pair<int, pair<int, string>> getVarInfo(RuleVarInfo& rvi, string& s){
/*Check IF var list first*/
    int i = 0;
    for(auto &p : rvi.ivl){
        if(s == p.first) {
            return make_pair(0, make_pair(i, p.second));
        }
        ++i;
    }
/*Must be inside of THEN var list*/
    i = 0;
    for(auto &p : rvi.tvl){
        if(s == p.first) {
            return make_pair(1, make_pair(i, p.second));
        }
        ++i;
    }

    return make_pair(-1, make_pair(i, "")); // Not inside either - Error
}

/* Returns the clause index for a given rule number
 *
 * i - index of rule
 * cn - maximum number of clauses
 *
 */
inline int getClauseIndex(int&i, const int&cn){
    int ruleNum = (i+1)*10;
    return cn * (ruleNum / 10 - 1 );
}

/* Returns an int value that relates to the condition operator given
 *
 * s -  String to be manipulated
 *
 */
inline int conditionToInt(string &s) {
    if(s == "=")
        return 0;
    if(s == "<")
        return 1;
    if(s == "<=")
        return 2;
    if(s == ">")
        return 3;
    if(s == ">=")
        return 4;
    return 5;
};

/* Returns true if a digit char is found, false otherwise
 *
 * s -  String to be tested
 *
 */
inline bool isNumeric(const string &s){
    for(auto & c : s)
        if(isdigit(c))
            return true;
    return false;
}

/* Changes all lowercase chars in a string to uppercase
 *
 * s -  String to be manipulated
 *
 */
inline string toUpper(string &s){
    for(auto & c : s)
        c = toupper(c);
    return s;
}

/* Pull data from rule base file and put into a vector of strings
 *
 * rs -  Rule string with all rule info as strings
 *
 */
inline void extractRules(vector <string> &rs){

    ifstream f; // file
    string w; // word

    /* Collect rules info from knowledge base*/
    f.open("KnowledgeBase.txt");
    if (!f) {
        cout << "Unable to open file";
        exit(1);
    }
    while (f >> w) {
        if (w != "\n") {
            if(w.substr(0,7) == "RULE: "){
                rs.emplace_back(" ");
            }
            rs.emplace_back(w);
        }
    }
    f.close();
}

/* Fill all data structures with info extracted from the rule base file
 *
 * rs - Rule string with all rule info as strings
 * cvi - Clause variable info container
 * rvi - Rule variable info container
 * cn - Highest number of if clauses in rules
 *
 */
inline void fillDataStructs(vector <string> &rs, ClauseVarInfo &cvi, RuleVarInfo &rvi, const int &cn) {
    int clsCtr = 0; // Current clause
    int ruleCtr = 1; // Current rule
    bool contains; // True - Element found ; False - Element not found
    string clauseName; // Name of clause
    string clauseOp; // Conditional operator for clause
    string clauseCompare; // Comparison value for clause

    for (int i = 0; i < rs.size(); ++i) {

        contains = false;
        clauseName = rs[i+1];
        clauseOp = rs[i+2];
        clauseCompare = rs[i+3];

/*Process IF statement variables*/
        if (rs[i] == "IF" || rs[i] == "AND") {
            cvi.cvl.emplace_back(clauseName);
            cvi.cndOp.emplace_back(clauseOp);
            cvi.cmpVal.emplace_back(clauseCompare);
            ++clsCtr;
/*If current var not inside of IF var list, then add new item to it*/
            for (auto &p : rvi.ivl) {
                if (p.first == clauseName) {
                    contains = true;
                    break;
                }
            }
            if(!contains){
                rvi.ivl.emplace_back(make_pair(rs[i + 1], ""));
            }
        }
/*Process THEN statement variables*/
        if (rs[i] == "THEN") {
            for (int j = 0; j < cn - clsCtr; ++j) {
                cvi.cvl.emplace_back("");
                cvi.cndOp.emplace_back("");
                cvi.cmpVal.emplace_back("");
            }
            clsCtr = 0;
            rvi.cl.emplace_back(make_pair(clauseName, clauseCompare));
            ++ruleCtr;
/*If current var not inside of THEN var list, then add new item to it*/
            for (auto &p : rvi.tvl) {
                if (p.first == rs[i + 1]) {
                    contains = true;
                    break;
                }
            }
            if(!contains){
                rvi.tvl.emplace_back(make_pair(rs[i + 1], ""));
            }
        }
    }
/*Remove any IF clause vars that are also THEN vars from the IF var list*/
    for(int j=0;j<rvi.ivl.size();++j){
        for(int k=0;k<rvi.tvl.size();++k) {
            if (rvi.ivl[j].first == rvi.tvl[k].first){
                rvi.ivl.erase(rvi.ivl.begin() + j);
                break;
            }
        }
    }
}

/* Print all info for data structures
 *
 * cvi - Clause variable info container
 * rvi - Rule variable info container
 *
 */
inline void print(ClauseVarInfo &cvi, RuleVarInfo &rvi) {
    cout << "\n\nCLAUSE VARIABLE INFO LIST\n---------------------------\n";
    for (int i = 0; i < cvi.cvl.size(); ++i) {
        if (cvi.cvl[i] == "") {
            cout << "index: " << i << " |";
        } else {
            cout << "index: " << i;
            cout << " | clause: " << cvi.cvl[i];
            cout << " | condition: " << cvi.cndOp[i];
            cout << " | comp val: " << cvi.cmpVal[i];
        }
        cout << "\n----------------------------------------------------------------\n";
    }
    cout << "\n\nIF VAR LIST\n-----------------\n";
    for(auto &p : rvi.ivl){
        cout << p.first << " " << p.second << " \n";
    }
    cout << "\n\nTHEN VAR LIST\n-----------------\n";
    for(auto &p : rvi.tvl){
        cout << p.first << " " << p.second << " \n";
    }
    cout << "\n\nCONCLUSION LIST\n-----------------\n";
    for(int j=0;j<rvi.cl.size();++j){
        cout << ((j+1)*10) << " " << rvi.cl[j].first << " " << rvi.cl[j].second << " \n";
    }

}
