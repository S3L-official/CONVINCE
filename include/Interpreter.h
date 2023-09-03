//
// Created by Pengfei Gao on 2020/2/17.
//

#ifndef SAFELANG_INTERPRETER_H
#define SAFELANG_INTERPRETER_H


#include "ASTNode.h"
#include "Value.h"
#include "ValueCommon.h"

class CodeGenBlock{
public:
    ValuePtr returnValue;
    std::map<string, ValuePtr> env;
//    std::map<string, bool> isFuncArg;
    std::map<string, std::vector<int>> arraySizes;
    vector<ValuePtr> parameters;
    map<string, vector<string>> arrayNames;
    vector<ValuePtr> sequence;
    vector<ValuePtr> hdSequence;
    vector<ValuePtr> assumptions;
};
typedef shared_ptr<CodeGenBlock> CodeGenBlockPtr;

class Interpreter {
private:

    vector<CodeGenBlockPtr> blockStack;
    vector<ProcValuePtr> procs;
    int numberOfInternal;
    set<string> nameOfKey;
    set<string> nameOfPlain;


public:

    static bool isHD;
    static int assumptionLevel;
    static vector<string> functionName;
    Interpreter() {
        numberOfInternal = 0;
    }
    void addToIntenal(int number) {
        numberOfInternal += number;
    }
//    int getInternal() {
//        return numberOfInternal;
//    }
    void generateCode(ASTNode::NBlock& root) {
        pushBlock();
        ValuePtr valuePtr = root.compute(*this);
        popBlock();
    }

    ValuePtr getValue(string type, string name, bool isParameter) {
        ValuePtr re = nullptr;
        if(isParameter && type == "int") {
            re = make_shared<ConstantValue>(name);
        }
        else if(isParameter) {
            re = make_shared<ParameterValue>(name);
        } else if(type == "private") {
            re = make_shared<PrivateValue>(name);
        } else if(type == "public") {
            re = make_shared<PublicValue>(name);
        } else if(type == "internal") {
            re = nullptr;
        } else if(type == "share") {
            re = make_shared<ParameterValue>(name);
        } else if(type == "random") {
            re = make_shared<RandomValue>(name);
        } else if(type == "int") {
            re = nullptr;
        } else {
            assert(false);
        }
        return re;
    }

    void addToEnv(string name, ValuePtr value) {
        blockStack.back()->env[name] = value;
    }

    ValuePtr getFromEnv(string name) {
//        assert(blockStack.back()->env.count(name));
//        if(blockStack.back()->env.count(name) == 0)
//            return nullptr;
//        return blockStack.back()->env[name];

        for(auto it = blockStack.rbegin(); it != blockStack.rend(); it++){
            if( (*it)->env.find(name) != (*it)->env.end() ){
                return (*it)->env[name];
            }
        }
        return nullptr;
    }

    void pushBlock() {
        CodeGenBlockPtr codeGenBlock = make_shared<CodeGenBlock>();
        codeGenBlock->returnValue = nullptr;
        blockStack.push_back(codeGenBlock);
    }

    void popBlock() {
        CodeGenBlockPtr codeGenBlock = blockStack.back();
        blockStack.pop_back();
    }

    void addParameter(vector<ValuePtr>& para) {
        blockStack.back()->parameters.insert(blockStack.back()->parameters.end(), para.begin(), para.end());
    }

    void addParameter(const ValuePtr& para) {
        blockStack.back()->parameters.push_back(para);
    }

    const vector<ValuePtr>& getParameter() {
        return blockStack.back()->parameters;
    }

    void addNameOfKey(string res) {
        nameOfKey.insert(res);
    }

    void addNameOfPlain(string res) {
        nameOfPlain.insert(res);
    }

    set<string> getNameOfPlain() {
        return nameOfPlain;
    }
    set<string> getNameOfKey() {
        return nameOfKey;
    }

    std::vector<int> getArraySize(string name){
        for(auto it=blockStack.rbegin(); it!=blockStack.rend(); it++){
            if( (*it)->arraySizes.find(name) != (*it)->arraySizes.end() ){
                return (*it)->arraySizes[name];
            }
        }
        return blockStack.back()->arraySizes[name];
    }

    void setArraySize(string name, std::vector<int> value){
//        cout << "setArraySize: " << name << ": " << value.size() << endl;
        blockStack.back()->arraySizes[name] = value;
//        cout << "blockStack.back()->arraySizes.size()" << blockStack.back()->arraySizes.size() << endl;
    }

    void setArrayNames(string name, vector<string>& names) {
        blockStack.back()->arrayNames[name] = names;
    }

    vector<string> getArrayNames(string name) {
        if(blockStack.back()->arrayNames.count(name) == 0)
            assert(false);
        return blockStack.back()->arrayNames[name];
    }

    void setCurrentReturnValue(ValuePtr value){
        blockStack.back()->returnValue = value;
    }

    ValuePtr getCurrentReturnValue(){
        return blockStack.back()->returnValue;
    }

    vector<ValuePtr>& getSequence() {
        return blockStack.back()->sequence;
    }

    void addToSequence(ValuePtr loc) {
        blockStack.back()->sequence.push_back(loc);
    }

    void addToHDSequence(ValuePtr loc) {
        blockStack.back()->hdSequence.push_back(loc);
    }

    vector<ValuePtr> getHDSequence() {
        return blockStack.back()->hdSequence;
    }

    map<string, ValuePtr> getEnv() {
        return blockStack.back()->env;
    }

    void addProc(ValuePtr proc) {
        ProcValuePtr procPtr = dynamic_pointer_cast<ProcValue>(proc);
        procs.push_back(procPtr);
    }

    const vector<ProcValuePtr> &getProcs() const {
        return procs;
    }

    ValuePtr getProc(string name) {
        for(auto ele : procs) {
            if(ProcValue* proc = dynamic_cast<ProcValue*>(ele.get())) {
                if(proc->getProcName() == name) {
                    return ele;
                } else {
                    continue;
                }
            } else {
                assert(false);
                return nullptr;
            }
        }
        assert(false);
        return nullptr;
    }

    int value_of(ValuePtr value) {
        if(ConcreteNumValue* number = dynamic_cast<ConcreteNumValue*>(value.get())) {
            return number->getNumer();
        } else {
            assert(false);
            return 0;
        }
    }


    void addAssumptions(ValuePtr ass) {
        blockStack.back()->assumptions.push_back(ass);
    }

    vector<ValuePtr> getAssumptions() {
        return blockStack.back()->assumptions;
    }

    vector<int> computeIndex(string name, ASTNode::NExpressionListPtr expList) {
        auto sizeVec = getArraySize(name);

        int index = 0;
        int dimons = sizeVec.size();
        for(int i = 0; i < expList->size(); i++) {
            auto arraySize = expList->at(i);
            if(arraySize->getTypeName() == "NInteger") {
                ASTNode::NIntegerPtr ident = dynamic_pointer_cast<ASTNode::NInteger>(arraySize);
                if(i != dimons - 1) {
                    int temp = 1;
                    for(int j = i + 1; j < sizeVec.size(); j++)
                        temp *= sizeVec[j];
                    index += temp * ident->getValue();
                }
                else
                    index += ident->getValue();
            } else if(arraySize->getTypeName() == "NIdentifier") {
                ASTNode::NIdentifierPtr ident = dynamic_pointer_cast<ASTNode::NIdentifier>(arraySize);
                if(i != dimons - 1) {
                    int temp = 1;
                    for (int j = i + 1; j < sizeVec.size(); j++)
                        temp *= sizeVec[j];
                    index += temp * value_of(ident->compute(*this));
                }
                else
                    index += value_of(ident->compute(*this));
            } else if(arraySize->getTypeName() == "NBinaryOperator") {
                ValuePtr arraySizeValue = arraySize->compute(*this);
                if(!ValueCommon::isNoParameter(arraySizeValue)) {
                }
                int value = value_of(arraySizeValue);
                if(i != dimons - 1) {
                    int temp = 1;
                    for (int j = i + 1; j < sizeVec.size(); j++)
                        temp *= sizeVec[j];
                    index += temp * value;
                }
                else
                    index += value;
            } else {
                assert(false);
            }
        }

        int number = 1;
        if(expList->size() < sizeVec.size()) {
            for(int i = expList->size(); i < sizeVec.size(); i++) {
                number *= sizeVec.at(i);
            }
        }

        vector<int> res;
        if(expList->size() == sizeVec.size()) {
            res.push_back(index);
        } else {
            for(int i = index; i < index + number; i++) {
                res.push_back(i);
            }
        }
        return res;
    }

    vector<ValuePtr> computeIndexWithSymbol(string name, ASTNode::NExpressionListPtr expList) {
        auto sizeVec = getArraySize(name);
        ValuePtr index = make_shared<ConcreteNumValue>("index", 0);
        int dimons = sizeVec.size();
        for(int i = 0; i < expList->size(); i++) {
            auto arraySize = expList->at(i);
            if(arraySize->getTypeName() == "NInteger") {
                ASTNode::NIntegerPtr ident = dynamic_pointer_cast<ASTNode::NInteger>(arraySize);
                ValuePtr identValue = make_shared<ConcreteNumValue>("ident", ident->getValue());
                if(i != dimons - 1) {
//                    int temp = 1;
                    ValuePtr temp = make_shared<ConcreteNumValue>("1", 1);
                    for(int j = i + 1; j < sizeVec.size(); j++) {
                        shared_ptr<ConcreteNumValue> sizeVecJ = make_shared<ConcreteNumValue>("j", sizeVec[j]);
                        temp = make_shared<InternalBinValue>("temp", temp, sizeVecJ, ASTNode::Operator::FFTIMES);
                    }
                    temp = make_shared<InternalBinValue>("temp", temp, identValue, ASTNode::Operator::FFTIMES);
                    index = make_shared<InternalBinValue>("index", index, temp, ASTNode::Operator::ADD);
                }
                else {
                    index = make_shared<InternalBinValue>("index", index, identValue, ASTNode::Operator::ADD);
                }
            } else if(arraySize->getTypeName() == "NIdentifier") {
                ASTNode::NIdentifierPtr ident = dynamic_pointer_cast<ASTNode::NIdentifier>(arraySize);
                ValuePtr identValue = ident->compute(*this);
                if(!ValueCommon::isNoParameter(identValue)) assert(false);
                if(i != dimons - 1) {
//                    int temp = 1;
                    ValuePtr temp = make_shared<ConcreteNumValue>("1", 1);
                    for(int j = i + 1; j < sizeVec.size(); j++) {
                        shared_ptr<ConcreteNumValue> sizeVecJ = make_shared<ConcreteNumValue>("j", sizeVec[j]);
                        temp = make_shared<InternalBinValue>("temp", temp, sizeVecJ, ASTNode::Operator::FFTIMES);
                    }
                    temp = make_shared<InternalBinValue>("temp", temp, identValue, ASTNode::Operator::FFTIMES);
                    index = make_shared<InternalBinValue>("index", index, temp, ASTNode::Operator::ADD);
                }
                else {
                    index = make_shared<InternalBinValue>("index", index, identValue, ASTNode::Operator::ADD);
                }
            } else if(arraySize->getTypeName() == "NBinaryOperator" || arraySize->getTypeName() == "NArrayIndex") {
                ValuePtr arraySizeValue = arraySize->compute(*this);
                    if(i != dimons - 1) {
                        ValuePtr temp = make_shared<ConcreteNumValue>("1", 1);
                        for(int j = i + 1; j < sizeVec.size(); j++) {
                            shared_ptr<ConcreteNumValue> sizeVecJ = make_shared<ConcreteNumValue>("j", sizeVec[j]);
                            temp = make_shared<InternalBinValue>("temp", temp, sizeVecJ, ASTNode::Operator::FFTIMES);
                        }
                        temp = make_shared<InternalBinValue>("temp", temp, arraySizeValue, ASTNode::Operator::FFTIMES);
                        index = make_shared<InternalBinValue>("index", index, temp, ASTNode::Operator::ADD);
                    }
                    else {
                        index = make_shared<InternalBinValue>("index", index, arraySizeValue, ASTNode::Operator::ADD);
                    }
            } else {
                assert(false);
            }
        }

        int number = 1;
        if(expList->size() < sizeVec.size()) {
            for(int i = expList->size(); i < sizeVec.size(); i++) {
                number *= sizeVec.at(i);
            }
        }

        vector<ValuePtr> res;
        if(expList->size() == sizeVec.size()) {
            res.push_back(index);
        } else {
            res.push_back(index);
            for(int i = 1; i < number; i++) {
                ValuePtr temp = make_shared<ConcreteNumValue>("i", i);
                res.push_back(make_shared<InternalBinValue>(to_string(i), index, temp, ASTNode::Operator::ADD));
            }
        }
        return res;
    }


};









#endif //SAFELANG_INTERPRETER_H
