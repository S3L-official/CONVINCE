//
// Created by Pengfei Gao on 2019-09-06.
//

#ifndef TESTBISON_UTILS_H
#define TESTBISON_UTILS_H

#include "string"
#include "json/json.h"
#include "IRCodeGen.h"
#include "algorithm"
#include "Value.h"
#include "list"

#ifdef DEBUG
#define DBG printf
#else
#define DBG
#endif

namespace Utils{

    void debug_info(const std::string& _info);
    void writeToJsonFile(std::string fileName, Json::Value root);

    void generateDDGPng(std::map<ThreeAddressNodePtr, set<ThreeAddressNodePtr>>& ddg);


    void setIntersection(set<ThreeAddressNodePtr> &lhs, set<ThreeAddressNodePtr> &rhs, set<ThreeAddressNodePtr> &result);

    void setIntersection(set<ValuePtr> &lhs, set<ValuePtr> &rhs, set<ValuePtr> &result);


    void setIntersection(set<string> &lhs, set<string> &rhs, set<string> &result);

    void setUnion(set<ThreeAddressNodePtr> &lhs, set<ThreeAddressNodePtr> &rhs, set<ThreeAddressNodePtr> &result);

    void setUnion(set<ValuePtr> &lhs, set<ValuePtr> &rhs, set<ValuePtr> &result);

    void setDifference(set<ThreeAddressNodePtr> &lhs, set<ThreeAddressNodePtr> &rhs, set<ThreeAddressNodePtr> &result);

    void setDifference(const set<vector<ThreeAddressNodePtr>> &lhs, const set<vector<ThreeAddressNodePtr>> &rhs, set<vector<ThreeAddressNodePtr>> &result);

    void setDifference(set<ValuePtr> &lhs, set<ValuePtr> &rhs, set<ValuePtr> &result);

    bool isSubset(vector<ThreeAddressNodePtr>& A, vector<ThreeAddressNodePtr>& B);

    string getCallSite(const vector<int>& callsites);

    string printSet(const set<ValuePtr>& a);

    bool gen_comb_norep_lex_init(vector<int>& array, int n, int k);

    bool gen_comb_norep_lex_next(vector<int>& array, int n, int k);


    string printSet(const set<ThreeAddressNodePtr>& a);

    string getCallSite(const list<string>&);

    bool startWith(string a, string b);


    void printProof(const set<set<ThreeAddressNodePtr>>& proof);

    vector<set<ThreeAddressNodePtr>> getPi(const set<ThreeAddressNodePtr>&, int i);

    void printY(const vector<set<ThreeAddressNodePtr>>& Y);


    void containSubexp(ThreeAddressNodePtr node, ThreeAddressNodePtr target, bool& isContain);

    void containSubexpSet(ThreeAddressNodePtr node, const set<ThreeAddressNodePtr>& targets, bool& isContain);


    void dealWithYt(vector<set<ThreeAddressNodePtr>>& Yset);

    vector<string> split(const string line, char delimiter);
}

#endif //TESTBISON_UTILS_H
