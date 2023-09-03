//
// Created by Pengfei Gao on 2019-09-06.
//

#include <fstream>
#include <iostream>
#include <sstream>
#include <Utils.h>

namespace Utils {
    void debug_info(const std::string& _info) {
        if (std::getenv("DEBUG") != nullptr) {
            std::cout << _info << "\n";
        }
    }

    void writeToJsonFile(std::string fileName, Json::Value root) {
        std::ofstream astJson(fileName);
        if( astJson.is_open() ){
            astJson << root;
            astJson.close();
            std::cout << "json write to " << fileName << std::endl;
        }
    }

    void printProof(const set<set<ThreeAddressNodePtr>>& proof) {
        cout << "proof: {" << endl;
        for(auto ele : proof) {
            cout << "{";
            for(auto ele1 : ele) {
                cout << ele1->prettyPrint5() << ", ";
            }
            cout << "}";
        }
        cout << "}" << endl;
    }

    void generateDDGPng(std::map<ThreeAddressNodePtr, set<ThreeAddressNodePtr>>& ddg) {
        std::stringstream ddgStream;
        std::ofstream output("sl_tmp_ddg_000.gv");
        ddgStream << "digraph CG {\n";
        for (auto it = ddg.begin(); it != ddg.end(); ++it) {
            for (auto it2 = (it->second).begin(); it2 != (it->second).end(); ++it2) {
                ddgStream << "    \"" << it->first->getID() << "\" -> \"" << (*it2)->getID() << "\";\n";
            }
        }
        ddgStream << "}\n";
        output << ddgStream.str();
        output.close();
        system("dot sl_tmp_ddg_000.gv -Tpng -o ddg.png");
        system("rm -f sl_tmp_ddg_000.gv");
    }


    void setIntersection(set<ThreeAddressNodePtr> &lhs, set<ThreeAddressNodePtr> &rhs, set<ThreeAddressNodePtr> &result) {
        std::set_intersection(lhs.begin(), lhs.end(),
                         rhs.begin(), rhs.end(),
                         std::inserter(result, result.end()));
    }

    void setIntersection(set<ValuePtr> &lhs, set<ValuePtr> &rhs, set<ValuePtr> &result) {
        std::set_intersection(lhs.begin(), lhs.end(),
                              rhs.begin(), rhs.end(),
                              std::inserter(result, result.end()));
    }


    void setIntersection(set<string> &lhs, set<string> &rhs, set<string> &result) {
        std::set_intersection(lhs.begin(), lhs.end(),
                              rhs.begin(), rhs.end(),
                              std::inserter(result, result.end()));
    }

    void setUnion(set<ThreeAddressNodePtr> &lhs, set<ThreeAddressNodePtr> &rhs, set<ThreeAddressNodePtr> &result) {
        std::set_union(lhs.begin(), lhs.end(),
                         rhs.begin(), rhs.end(),
                         std::inserter(result, result.end()));

    }

    void setUnion(set<ValuePtr> &lhs, set<ValuePtr> &rhs, set<ValuePtr> &result) {
        std::set_union(lhs.begin(), lhs.end(),
                       rhs.begin(), rhs.end(),
                       std::inserter(result, result.end()));

    }

    void setDifference(set<ThreeAddressNodePtr> &lhs, set<ThreeAddressNodePtr> &rhs, set<ThreeAddressNodePtr> &result) {
        std::set_difference(lhs.begin(), lhs.end(),
                         rhs.begin(), rhs.end(),
                         std::inserter(result, result.end()));

    }

    void setDifference(const set<vector<ThreeAddressNodePtr>> &lhs, const set<vector<ThreeAddressNodePtr>> &rhs, set<vector<ThreeAddressNodePtr>> &result) {
        std::set_difference(lhs.begin(), lhs.end(),
                            rhs.begin(), rhs.end(),
                            std::inserter(result, result.end()));

    }


    void setDifference(set<ValuePtr> &lhs, set<ValuePtr> &rhs, set<ValuePtr> &result) {
        std::set_difference(lhs.begin(), lhs.end(),
                            rhs.begin(), rhs.end(),
                            std::inserter(result, result.end()));
    }

    bool isSubset(vector<ThreeAddressNodePtr>& A, vector<ThreeAddressNodePtr>& B) {
        std::sort(A.begin(), A.end());
        std::sort(B.begin(), B.end());

        return std::includes(A.begin(), A.end(), B.begin(), B.end());
    }

    string getCallSite(const vector<int>& callsites) {
        string result = "";
        for(auto ele : callsites) {
            result += "@" + to_string(ele);
        }
        return result;
    }

    string printSet(const set<ValuePtr>& a) {
        string res = "";
        for(auto ele : a) {
            res += ele->toString() + ",";
        }
        return res;
    }

    string printSet(const set<ThreeAddressNodePtr>& a) {
        string res = "";
        for(auto ele : a) {
            res += ele->getNodeName() + ",";
        }
        return res;
    }


    bool gen_comb_norep_lex_init(vector<int>& array, int n, int k)
    {
        int j; //index

        if(k > n)
            return 0;
        if(k == 0)
            return 0;
        //initialize: vector[0, ..., k - 1] are 0, ..., k - 1
        for(j = 0; j < k; j++)
            array.push_back(j);

        return 1;
    }

    bool gen_comb_norep_lex_next(vector<int>& array, int n, int k)
    {
        int j; //index

        //easy case, increase rightmost element
        if(array[k - 1] < n - 1)
        {
            array[k - 1]++;
            return 1;
        }

        //find rightmost element to increase
        for(j = k - 2; j >= 0; j--)
            if(array[j] < n - k + j)
                break;

        //terminate if vector[0] == n - k
        if(j < 0)
            return 0;

        //increase
        array[j]++;

        //set right-hand elements
        while(j < k - 1)
        {
            array[j + 1] = array[j] + 1;
            j++;
        }

        return 1;
    }


    string getCallSite(const list<string>& a){
        string result = "";
        for(auto ele : a) {
            result += "@";
            result += ele;
        }
        return result;
    }


    bool startWith(string a, string b) {
        if(a == b)
            return true;
        string::size_type pos = a.find("@");
        string first = a.substr(0, pos);
        if(first == b)
            return true;
        else
            return false;
    }




    vector<set<ThreeAddressNodePtr>> getPi(const set<ThreeAddressNodePtr>& nodeset, int i) {
        vector<set<ThreeAddressNodePtr>> result;
        vector<ThreeAddressNodePtr> nodesetV{nodeset.begin(), nodeset.end()};
        vector<int> array;
        bool initSuccess = Utils::gen_comb_norep_lex_init(array, nodeset.size(), i);
        if(initSuccess) {
            set<ThreeAddressNodePtr> temp;
            for(auto ele : array) temp.insert(nodesetV[ele]);
            result.push_back(temp);
            while(gen_comb_norep_lex_next(array, nodeset.size(), i)) {
                set<ThreeAddressNodePtr> temp;
                for(auto ele : array) temp.insert(nodesetV[ele]);
                result.push_back(temp);
            }
        }
        return result;
    }

    void containSubexp(ThreeAddressNodePtr node, ThreeAddressNodePtr target, bool& isContain) {
        if(isContain)
            return;
        if(!node)
            return;
        if(node == target) {
            isContain = true;
            return;
        }
        containSubexp(node->getLhs(), target, isContain);
        containSubexp(node->getRhs(), target, isContain);
    }

    void containSubexpSet(ThreeAddressNodePtr node, const set<ThreeAddressNodePtr>& targets, bool& isContain) {
        if(isContain)
            return;
        if(!node)
            return;
        if(targets.count(node) != 0) {
            isContain = true;
            return;
        }
        containSubexpSet(node->getLhs(), targets, isContain);
        containSubexpSet(node->getRhs(), targets, isContain);
    }

    void printY(const vector<set<ThreeAddressNodePtr>>& Y) {
        int i = 1;
        for(auto ele : Y) {
            cout << "========================================" << i << endl;
            for(auto ele1 : ele) {
                cout << ele1->prettyPrint5() << endl;
            }
            cout << "========================================" << i << endl;
            i++;
        }
    }

    void dealWithYt(vector<set<ThreeAddressNodePtr>>& Yset) {
        // a0, a1, a2, b0, b1, b2，{a0, b0}, {a1, b1}, {a2, b2}
        vector<set<ThreeAddressNodePtr>> needTobeRemoved;
        for(int i = 0; i < Yset.size(); i++) {
            for(int j = 0; j < Yset.size(); j++) {
                if(i == j) continue;
                if(!Yset[j].empty()) {
                    // 如果是一个set
                    if(std::includes(Yset[j].begin(), Yset[j].end(), Yset[i].begin(), Yset[i].end())) {
                        needTobeRemoved.push_back(Yset[i]);
                    }
                }
            }
        }
        for(int i = 0; i < needTobeRemoved.size(); i++)
        {
            auto iter = std::find(Yset.begin(),Yset.end(),needTobeRemoved[i]);
            if(iter != Yset.end())
            {
                Yset.erase(iter);
            }
        }
    }

    vector<string> split(const string line, char delimiter)
    {
        // Variable declarations
        vector<string> tokens;
        string token;

        // Turn string into stream
        stringstream stream(line);

        // Extract tokens from line
        while(getline(stream, token, delimiter)) {
            tokens.push_back(token);
        }

        return tokens;
    }



}