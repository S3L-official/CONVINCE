//
// Created by Pengfei Gao on 2020/2/17.
//

#include "Value.h"

PrivateValue::PrivateValue(const string &name) : Value(name), name(name) {

}

const string &PrivateValue::getName() const {
    return name;
}

PublicValue::PublicValue(const string &name) : Value(name), name(name) {
}

const string &PublicValue::getName() const {
    return name;
}

RandomValue::RandomValue(const string &name) : Value(name), name(name) {

}

const string &RandomValue::getName() const {
    return name;
}

InternalUnValue::InternalUnValue(string name, const ValuePtr &rand, ASTNode::Operator op) : Value(name), rand(rand), op(op) {

}

const ValuePtr &InternalUnValue::getRand() const {
    return rand;
}

ASTNode::Operator InternalUnValue::getOp() const {
    return op;
}

InternalBinValue::InternalBinValue(string name, const ValuePtr &left, const ValuePtr &right, ASTNode::Operator op) : Value(name),left(left),
                                                                                                        right(right),
                                                                                                        op(op) {
}

const ValuePtr &InternalBinValue::getLeft() const {
    return left;
}

const ValuePtr &InternalBinValue::getRight() const {
    return right;
}

ASTNode::Operator InternalBinValue::getOp() const {
    return op;
}

ParameterValue::ParameterValue(const string &name) : Value(name), name(name) {

}


ArrayValue::ArrayValue(string name, const vector<ValuePtr> &arrayValue) : Value(name), arrayValue(arrayValue) {

}

const vector<ValuePtr> &ArrayValue::getArrayValue() const {
    return arrayValue;
}


Procedure::Procedure(const string &procName, const vector<ValuePtr> &parameters,const vector<ValuePtr> &assumptions , const map<string, ValuePtr>&env, const vector
                     <ValuePtr> &block, const ValuePtr &returns, const vector <ValuePtr> &hdBlock) : procName(procName), parameters(parameters), assumptions(assumptions), env(env),
                                                                 block(block), returns(returns) , hdBlock(hdBlock){}

const string &Procedure::getProcName() const {
    return procName;
}

const vector<ValuePtr> &Procedure::getParameters() const {
    return parameters;
}

vector<ValuePtr> &Procedure::getBlock() {
    return block;
}

const map<string, ValuePtr>&Procedure::getEnv() const {
    return env;
}

const ValuePtr &Procedure::getReturns() const {
    return returns;
}

const vector<ValuePtr> &Procedure::getAssumptions() const {
    return assumptions;
}

const vector<ValuePtr> &Procedure::getHdBlock() const {
    return hdBlock;
}

void Procedure::setBlock(const vector<ValuePtr> &block) {
    Procedure::block = block;
}

ProcValue::ProcValue(string name, const ProcedurePtr &procedurePtr) : Value(name), procedurePtr(procedurePtr) {

}

const ProcedurePtr &ProcValue::getProcedurePtr() const {
    return procedurePtr;
}

ConcreteNumValue::ConcreteNumValue(string name, int numer) : Value(name), numer(numer) {

}

int ConcreteNumValue::getNumer() const {
    return numer;
}

const ProcedurePtr &ProcCallValue::getProcedurePtr() const {
    return procedurePtr;
}

const vector<ValuePtr> &ProcCallValue::getArguments() const {
    return arguments;
}

vector<int> ProcCallValue::getCallsite() const {
    return callsite;
}

ProcCallValue::ProcCallValue(string name, const ProcedurePtr &procedurePtr, const vector<ValuePtr> &arguments, vector<int> callsite
                             ) : Value(name), procedurePtr(procedurePtr), arguments(arguments), callsite(callsite) {
}

DomAssValue::DomAssValue(string name, const ValuePtr &first, const ValuePtr &second) : AssumptionValue(name), first(first), second(second) {


}

const ValuePtr &DomAssValue::getFirst() const {
    return first;
}

const ValuePtr &DomAssValue::getSecond() const {
    return second;
}


RValueAssValue::RValueAssValue(string name, const ValuePtr &first, const ValuePtr &second) : AssumptionValue(name), first(first), second(second) {

}

const ValuePtr &RValueAssValue::getFirst() const {
    return first;
}

const ValuePtr &RValueAssValue::getSecond() const {
    return second;
}

TypeAssValue::TypeAssValue(string name, const ValuePtr &first, const Type &type) : AssumptionValue(name), first(first), type(type) {
}

const ValuePtr &TypeAssValue::getFirst() const {
    return first;
}

Type TypeAssValue::getType() const {
    return type;
}

ConstantValue::ConstantValue(string name) : Value(name) {


}

PsiAssValue::PsiAssValue(string name, const vector<ValuePtr> &assumptions) : Value(name), assumptions(assumptions) {
}

const vector<ValuePtr> &PsiAssValue::getAssumptions() const {
    return assumptions;
}


const ValuePtr &ProcCallValueIndex::getProcCallValuePtr() const {
    return procCallValuePtr;
}

int ProcCallValueIndex::getNumber() const {
    return number;
}

ProcCallValueIndex::ProcCallValueIndex(string name, const ValuePtr &procCallValuePtr, int number) :Value(name), procCallValuePtr(
        procCallValuePtr), number(number) {}

ArrayValueIndex::ArrayValueIndex(const string &valueName, const ValuePtr &arrayValuePtr,
                                 const ValuePtr &symbolIndex) : Value(valueName), arrayValuePtr(arrayValuePtr),
                                                                symbolIndex(symbolIndex) {}

const ValuePtr &ArrayValueIndex::getArrayValuePtr() const {
    return arrayValuePtr;
}

const ValuePtr &ArrayValueIndex::getSymbolIndex() const {
    return symbolIndex;
}
