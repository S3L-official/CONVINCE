//
// Created by Pengfei Gao on 2019-09-05.
//

#ifndef TESTBISON_IRCODEGEN_H
#define TESTBISON_IRCODEGEN_H

#include <string>
#include <vector>
#include <stack>
#include <set>
#include "ASTNode.h"
#include "ThreeAddressNode.h"
using namespace std;

typedef pair<string, string> domStringPari;



class IRCodeGenBlock {
public:
    std::map<string, ThreeAddressNodePtr> locals;
    std::map<string, int> localValues;
    std::map<string, vector<ThreeAddressNodePtr>> types;
};

class Function {
public:
    string name;
    vector<ThreeAddressNodePtr> paramenters;
    vector<ThreeAddressNodePtr> instructions;
    vector<ThreeAddressNodePtr> returns;
};

class IRCodeGenContext {
private:
//    vector<ThreeAddressNodePtr> insructions; // input of verifier

    // used to store syntax sugure z = x[1:3] & y[1:3] and the assignment whose return value is array
    stack<vector<ThreeAddressNodePtr>> arrayAssignmentStack; // used for sytax sugure z = x[1:3] & y[1:3]

    // used to store instructions for each function
    map<string, vector<ThreeAddressNodePtr>> instructionsForFunction;
    // used to construct data dependency graph
    map<string, map<string, ThreeAddressNodePtr>> functionNameVariableNameInstruction;
    // used to store parameters for each function
    map<string, vector<ThreeAddressNodePtr>> functionParameters;
    // used to store return values for each function
    map<string, vector<ThreeAddressNodePtr>> functionReturn;
    // used to store variable type for each function
    map<string, map<string, vector<string>>> functionVariableToType;
    // function name
    vector<string> functionNames;
    // index i,j should have the concrete value
    map<string, int> ForStatementEnvironment;
    // definition set
    set<ThreeAddressNodePtr> definitions;

    vector<ThreeAddressNodePtr> DDG;

    vector<IRCodeGenBlock*> blockStack;

    vector<Function*> functionStack;



public:
    IRCodeGenContext(){}

    stack<vector<string>> ArrayAssignmentNameStack;
    map<string, vector<int>> arrayBounds;
    bool contextOfArrayIndex = 0;
    void addArrayRangeAssignment(vector<ThreeAddressNodePtr>);

    vector<ThreeAddressNodePtr> popArrayRangeAssignment();

    ThreeAddressNodePtr declareVariable(string type, string name);

    void generateCode(ASTNode::NBlock& root);

    int evaluateExpr(ThreeAddressNodePtr expr, IRCodeGenContext& context);

    bool evaluateCond(ThreeAddressNodePtr expr, IRCodeGenContext& context);

    void pushBlock() {
        IRCodeGenBlock* codeGenBlock = new IRCodeGenBlock();
        blockStack.push_back(codeGenBlock);
    }

    void popBlock() {
        IRCodeGenBlock* codeGenBlock = blockStack.back();
        blockStack.pop_back();
        delete codeGenBlock;
    }

    void setSymbolValueViaDecl(string name, ThreeAddressNodePtr nodeptr) {
        blockStack.back()->locals[name] = nodeptr;
    }

    void setSymbolValueViaAss(string name, ThreeAddressNodePtr nodeptr) {
        bool flag = false;
        for(auto it=blockStack.rbegin(); it!=blockStack.rend(); it++){
            if( (*it)->locals.find(name) != (*it)->locals.end() ){
                flag = true;
                (*it)->locals[name] = nodeptr;
                break;
            }
        }
        assert(flag && "can not find variable");
    }

    Function* getCurrentFunction() {
        return functionStack.back();
    }

    void pushFunction(Function* fun) {
        functionStack.push_back(fun);
    }

    ThreeAddressNodePtr getSymbolValue(string name) {
        for(auto it=blockStack.rbegin(); it!=blockStack.rend(); it++){
            if( (*it)->locals.find(name) != (*it)->locals.end() ){
                return (*it)->locals[name];
            }
        }
        return nullptr;
    }

    vector<ThreeAddressNodePtr> getSymbolValues(vector<string>& names) {
        vector<ThreeAddressNodePtr> res;
        for(string name : names) {
            res.push_back(getSymbolValue(name));
        }
        return res;
    }

    vector<string> getSymbolNameStartedWith(string name) {
        vector<string> res;
        for(auto it = blockStack.rbegin(); it != blockStack.rend(); it++) {
            bool flag = false;
            map<string, ThreeAddressNodePtr>& locals = (*it)->locals;
            for(auto it = locals.begin(); it != locals.end(); it++) {
                if(it->first.find(name) != string::npos) {
                    flag = true;
                    res.push_back(it->first);
                }
            }
            if(flag)
                break;
        }
        return res;
    }

    void setSymbolConcreteValue(string name, int value) {
        blockStack.back()->localValues[name] = value;
    }

    void setSymbolType(string name, ThreeAddressNodePtr nodeptr) {
        blockStack.back()->types[name].push_back(nodeptr);
    }

    vector<ThreeAddressNodePtr> getSymbolType(string name) {
        for(auto it=blockStack.rbegin(); it!=blockStack.rend(); it++){
            if( (*it)->types.find(name) != (*it)->types.end() ){
                return (*it)->types[name];
            }
        }
        vector<ThreeAddressNodePtr> empty;
        return empty;
    }

    Function* getFunction(string name) {
        for(Function* func : functionStack) {
            if(func->name == name)
                return func;
        }
        assert(false);
    }

    int getSymbolConcreteValue(string name) {
        for(auto it=blockStack.rbegin(); it!=blockStack.rend(); it++){
            if( (*it)->localValues.find(name) != (*it)->localValues.end() ){
                return (*it)->localValues[name];
            }
        }
        return -1;
        return blockStack.back()->localValues[name];
    }

    void addOneInstruction(ThreeAddressNodePtr a) {
//        instructionsForFunction[functionNames.back()].push_back(a);
//        a->prettyPrint();
    }

    int getValueFromEnvironment(string name) {
        return ForStatementEnvironment[name];
    }

    void setValueForEnvironment(string name, int value) {
        ForStatementEnvironment[name] = value;
    }

    void removeLatestInstruction() {
        ThreeAddressNodePtr a = instructionsForFunction[functionNames.back()].back();
        cout << "remove: ";
        a->prettyPrint();
        instructionsForFunction[functionNames.back()].pop_back();
    }

    ThreeAddressNodePtr getLatestInstruction() {
        return instructionsForFunction[functionNames.back()].back();
    }

    void ToSSA(string functionName);

    void prettyPrint(string functionName);

    // used to evaluate the index value, used for forstatement
    static int evaluate(ThreeAddressNodePtr expr, IRCodeGenContext context);

    vector<ThreeAddressNodePtr> &getFunctionInstructions(string functionName) {
        return instructionsForFunction[functionName];
    }

    void addFunctionInstructionDDG(string functionName, string nodeName, ThreeAddressNodePtr node) {
        functionNameVariableNameInstruction[functionName][nodeName] = node;
    }

    ThreeAddressNodePtr getThreeAddressNodePtrDDG(string functionName, string nodeName) {
        if(functionNameVariableNameInstruction[functionName].count(nodeName) != 0) {
            cout << "already have definition of ThreeAddressNode: " << nodeName << endl;
            return functionNameVariableNameInstruction[functionName][nodeName];
        } else {
            ThreeAddressNodePtr re = make_shared<ThreeAddressNode>(nodeName,
                                                                   nullptr, nullptr, ASTNode::Operator::NULLOP,
                                                                   NodeType::UNKNOWN);
            this->addFunctionInstructionDDG(this->getCurrentFunctionName(), nodeName, re);
            if(nodeName != "newrandom")
                this->addOneInstruction(re);
            return re;
        }
    }

    string getCurrentFunctionName() {
        return functionNames.back();
    }

    void addFunction(string name) {
        functionNames.push_back(name);
    }

    void addFunctionParameters(ThreeAddressNodePtr parameterName) {
        string functionName = getCurrentFunctionName();
        functionParameters[functionName].push_back(parameterName);
    }

    void addFunctionReturns(ThreeAddressNodePtr returnVariable) {
        string functionName = getCurrentFunctionName();
        functionReturn[functionName].push_back(returnVariable);
    }

    const vector<ThreeAddressNodePtr> &getFunctionReturns(string functionName) {
        return functionReturn[functionName];
    }

    const vector<ThreeAddressNodePtr> &getFunctionParameters(string functionName){
        return functionParameters[functionName];
    }

    vector<ThreeAddressNodePtr> copyInstructionSet(string functionName, int callSite,
            vector<ThreeAddressNodePtr>& parameters, vector<ThreeAddressNodePtr>& returnVariables);

    void addVariableToType(string id, vector<string> type) {
        string functionName = getCurrentFunctionName();
        functionVariableToType[functionName][id] = type;
    }

    const vector<string> getTypeOfVariable(string name) {
        string functionName = getCurrentFunctionName();
        return functionVariableToType[functionName][name];
    }

    void addDefinition(ThreeAddressNodePtr ptr) {
        definitions.insert(ptr);
    }

    bool hasDefinition(ThreeAddressNodePtr ptr) {
        if(definitions.count(ptr) != 0)
            return true;
        return false;
    }

    static Json::Value jsonGen(ThreeAddressNodePtr ptr) {
        Json::Value root;
        string type = "(";
        if(ptr->getNodeType() == NodeType::PRIVATE)
            type += "private";
        else if(ptr->getNodeType() == NodeType::INTERNAL)
            type += "internal";
        else if(ptr->getNodeType() == NodeType::RANDOM)
            type += "random";
        else if(ptr->getNodeType() == NodeType::UNKNOWN)
            type += "unknown";
        else if(ptr->getNodeType() == NodeType::CONSTANT)
            type += "constant";
        type += ")";
        root["name"] = ptr->getNodeName() + type;
        if(ptr->getLhs() != nullptr)
            root["children"].append(jsonGen(ptr->getLhs()));
        if(ptr->getRhs() != nullptr)
            root["children"].append(jsonGen(ptr->getRhs()));
        return root;
    }

    static void fillInParents(ThreeAddressNodePtr node, set<ThreeAddressNodePtr>& visited) {
        if(visited.find(node) == visited.end()) {
            visited.insert(node);
            if(node->getLhs() == nullptr && node->getRhs() == nullptr) {
                return;
            }
            if(node->getLhs() != nullptr) {
                node->getLhs()->addParents(node);
                fillInParents(node->getLhs(), visited);
            }
            if(node->getRhs() != nullptr) {
                node->getRhs()->addParents(node);
                fillInParents(node->getRhs(), visited);
            }


        }
    }

    static void setNumberOfChild(ThreeAddressNodePtr node, map<ThreeAddressNodePtr, int>& visited) {
        /**
        if(visited.count(node) == 0) {
            if(node->getLhs() == nullptr && node->getRhs() == nullptr) {
                node->setNumberOfChild(1);
                visited[node] = 1;
                return;
            }
            if(node->getLhs() != nullptr) {
                setNumberOfChild(node->getLhs(), visited);
            }
            if(node->getRhs() != nullptr) {
                setNumberOfChild(node->getRhs(), visited);
            }

            if(node->getRhs() != nullptr) {
                node->setNumberOfChild(node->getLhs()->getNumberOfChild() + node->getRhs()->getNumberOfChild());
                visited[node] = node->getLhs()->getNumberOfChild() + node->getRhs()->getNumberOfChild();
            } else {
                node->setNumberOfChild(node->getLhs()->getNumberOfChild());
                visited[node] = node->getLhs()->getNumberOfChild();
            }

        }
         **/
         if(visited.count(node) != 0)
            return;
        if(node->getLhs() == nullptr && node->getRhs() == nullptr) {
            node->setNumberOfChild(1);
            visited[node] = 1;
        } else {
            if(node->getRhs() != nullptr) {
                setNumberOfChild(node->getLhs(), visited);
                setNumberOfChild(node->getRhs(), visited);
                int number = node->getLhs()->getNumberOfChild() + node->getRhs()->getNumberOfChild();
                if(number > 1000)
                    cout << "1" << endl;
                node->setNumberOfChild(number);
            } else {
                setNumberOfChild(node->getLhs(), visited);
                int number = node->getLhs()->getNumberOfChild();
                node->setNumberOfChild(number);
            }
            visited[node] = 1;
        }
    }

//    static void fillInParents(ThreeAddressNodePtr ptr) {
//        if(ptr == nullptr)
//            return;
//        if(ptr->getLhs() != nullptr) {
//            ptr->getLhs()->addParents(ptr);
//        }
//
//        if(ptr->getRhs() != nullptr) {
//            ptr->getRhs()->addParents(ptr);
//        }
//        fillInParents(ptr->getLhs());
//        fillInParents(ptr->getRhs());
//    }

    void removeInsFromInstructionSet(string basicString, ThreeAddressNodePtr sharedPtr);

    vector<ThreeAddressNodePtr> dealWithAssign(string functionName) {
        vector<ThreeAddressNodePtr> instructions = this->getFunction(functionName)->instructions;
        for(auto ins : instructions) {
            // find assign
            if(ins->getLhs() != nullptr && ins->getRhs() == nullptr && ins->getOp() == ASTNode::Operator::NULLOP) {
                ThreeAddressNodePtr lhs = ins->getLhs();
                vector<ThreeAddressNodeWeakPtr> parents = ins->getParents();
                for(auto ptr : parents) {
                    if(ptr.lock()->getLhs() == ins)
                        ptr.lock()->setLhs(lhs);
                    else if(ptr.lock() -> getRhs() == ins)
                        ptr.lock()->setRhs(lhs);
                }
                ins->getLhs()->setParents(parents);
//                cout << "use " << lhs->getID() << " substitudes " << ins->getID() << endl;
                removeInsFromInstructionSet(functionName, ins);
            }
        }
        return instructions;
    }

    static void DotGen(ThreeAddressNodePtr ptr, map<ThreeAddressNodePtr, set<ThreeAddressNodePtr>>& ddg) {
        if(ptr == nullptr)
            return;
        if(ptr->getLhs() != nullptr) {
            if(ddg.count(ptr) == 0) {
                set<ThreeAddressNodePtr> a;
                a.insert(ptr->getLhs());
                ddg[ptr] = a;
            } else {
                ddg[ptr].insert(ptr->getLhs());
            }

        }
        if(ptr->getRhs() != nullptr) {
            if(ddg.count(ptr) == 0) {
                set<ThreeAddressNodePtr> a;
                a.insert(ptr->getRhs());
                ddg[ptr] = a;
            } else {
                ddg[ptr].insert(ptr->getRhs());
            }
        }

        DotGen(ptr->getLhs(), ddg);
        DotGen(ptr->getRhs(), ddg);

    }

    void addToDDG(ThreeAddressNodePtr node) {
        DDG.push_back(node);
    }

    vector<ThreeAddressNodePtr>& getDDG() {
        return DDG;
    }

    void PrintSymTable() const{
        cout << "======= Print Symbol Table ========" << endl;
        string prefix = "";
        for(auto it=blockStack.begin(); it!=blockStack.end(); it++){
            for(auto it2=(*it)->locals.begin(); it2!=(*it)->locals.end(); it2++){
                cout << prefix << it2->first << " = " << it2->second << ": " << endl;
            }
            prefix += "\t";
        }
        cout << "===================================" << endl;
    }

    void PrintConcreteValue() const{
        cout << "======= Print ConcreteValue Table ========" << endl;
        string prefix = "";
        for(auto it=blockStack.begin(); it!=blockStack.end(); it++){
            for(auto it2=(*it)->localValues.begin(); it2!=(*it)->localValues.end(); it2++){
                cout << prefix << it2->first << " = " << it2->second << ": " << endl;
            }
            prefix += "\t";
        }
        cout << "===================================" << endl;
    }


};


#endif //TESTBISON_IRCODEGEN_H
