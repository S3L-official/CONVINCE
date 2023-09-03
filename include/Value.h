//
// Created by Pengfei Gao on 2020/2/17.
//

#ifndef SAFELANG_VALUE_H
#define SAFELANG_VALUE_H

#include <string>
#include <vector>
#include <set>
#include <map>
#include <ASTNode.h>
#include <math.h>

using namespace std;
enum Type {
    SI,
    UF,
    UK,
    LK
};

enum ValueType {
    VTPrivateValue,
    VTPublicValue,
    VTRandomVable,
    VTInternalBinValue,
    VTInternalUnValue,
    VTArrayValue,
    VTProcValue,
    VTParameterValue,
    VTProcCallValue,
    VTProcCallValueIndex,
    VTDomAssValue,
    VTRValueAssValue,
    VTTypeAssValue,
    VTPsiAssValue,
    VTConcreteNumValue,
    VTConstantValue,
    VTArrayValueIndex
};

typedef pair<Type, bool> TypeContext;
class RandomValue;
typedef shared_ptr<RandomValue> RandomValuePtr;

// 这里我声明了一个Value父类，在其中没有声明一个pure virtual函数，意味着我允许他们在没有实现的，可以继承default实现
class Value {
private:
    vector<ValuePtr> parents;
    string name;
    TypeContext typeContext;
    set<ValuePtr> supports;
    set<ValuePtr> uniqueM;
    set<ValuePtr> perfectM;
    set<ValuePtr> dominate;
    set<ValuePtr> randomV;
    set<ValuePtr> parameters;
public:
    Value(string valueName) {
        typeContext = TypeContext(Type::UK, 1);
        name = valueName;
    }
    virtual ~Value(){}
    virtual string toString() {
        assert(false);
        return "1";
    }

    virtual int value_of(const map<string, ValuePtr>& env) {
        assert(false);
        return 0;
    }

    virtual bool isRandomForBij() {
        return false;
    }


    virtual ValuePtr copy() {
        return nullptr;
    }


    void addParents(ValuePtr valuePtr) {
        parents.push_back(valuePtr);
    }

    vector<ValuePtr> &getParents() {
        return parents;
    }

    void setParents(const vector<ValuePtr> &parents) {
        Value::parents = parents;
    }

    virtual void replaceChild(ValuePtr v1, ValuePtr v2){
        cout << "no implementation" << endl;
        assert(false);
    }

    virtual void removeChild(ValuePtr child) {
        assert(false);
    };

    virtual ValueType getValueType() = 0;

    virtual string getName() {
        return name;
    }

    void setName(string newName) {
        name = newName;
    }

    TypeContext getTypeContext() {
        return typeContext;
    }

    void setTypeContext(TypeContext typeContext1) {
        typeContext = typeContext1;
    }

    set<ValuePtr>& getSupports() {
        return supports;
    }

    void setSupports(const set<ValuePtr> &supports) {
        Value::supports = supports;
    }

    set<ValuePtr>& getUniqueM() {
        return uniqueM;
    }

    void setUniqueM(const set<ValuePtr> &uniqueM) {
        Value::uniqueM = uniqueM;
    }

    set<ValuePtr>& getPerfectM() {
        return perfectM;
    }

    void setPerfectM(const set<ValuePtr> &perfectM) {
        Value::perfectM = perfectM;
    }

    set<ValuePtr>& getDominate() {
        return dominate;
    }

    void setDominate(const set<ValuePtr> &dominate) {
        Value::dominate = dominate;
    }

    void setParameters(const set<ValuePtr> &parameters) {
        Value::parameters = parameters;
    }


    void addToSupport(ValuePtr valuePtr) {
        this->supports.insert(valuePtr);
    }
    void addToUniqueM(ValuePtr valuePtr) {
        this->uniqueM.insert(valuePtr);
    }
    void addToPerfectM(ValuePtr valuePtr) {
        this->perfectM.insert(valuePtr);
    }
    void addToDominate(ValuePtr valuePtr) {
        this->dominate.insert(valuePtr);
    }
    void addToRandomV(ValuePtr valuePtr) {
        this->randomV.insert(valuePtr);
    }
    void addToParameters(ValuePtr valuePtr) {
        this->parameters.insert(valuePtr);
    }

    set<ValuePtr> &getRandomV() {
        return randomV;
    }

    void setRandomV(const set<ValuePtr> &randomV) {
        Value::randomV = randomV;
    }

    set<ValuePtr> &getParameters() {
        return parameters;
    }

    void clearTable() {
        supports.clear();
        uniqueM.clear();
        perfectM.clear();
        dominate.clear();
        randomV.clear();
        parameters.clear();
    }

//    virtual void accept(Visitor visitor) {
//
//    }


};

typedef shared_ptr<Value> ValuePtr;

class PrivateValue : public Value {
private:
    string name;
public:
    PrivateValue(const string &name);
    virtual string toString() override {
        return "Private(" + name + ")";
    }

    const string &getName() const;

    virtual ValueType getValueType() override {
        return ValueType::VTPrivateValue;
    }

};

typedef shared_ptr<PrivateValue> PrivateValuePtr;

class PublicValue : public Value {
private:
    string name;
public:
    PublicValue(const string &name);
    virtual string toString() override {
        return "Public(" + name + ")";
    }

    const string &getName() const;
    virtual ValueType getValueType() override {
        return ValueType::VTPublicValue;
    }


};

typedef shared_ptr<PublicValue> PublicValuePtr;

class RandomValue : public Value {
private:
    string name;
public:
    RandomValue(const string &name);
    virtual string toString() override {
        return "Random(" + name + ")";
    }

    const string &getName() const;
    virtual ValueType getValueType() override {
        return ValueType::VTRandomVable;
    }


    virtual ValuePtr copy() override{
        return make_shared<RandomValue>(name);
    }


};



class InternalBinValue : public Value {
private:
    ValuePtr left;
    ValuePtr right;
    ASTNode::Operator op;
public:
    InternalBinValue(string name, const ValuePtr &left, const ValuePtr &right, ASTNode::Operator op);
    virtual string toString() override {
        if(op == ASTNode::Operator::ADD)
            return left->toString() + " + " + right->toString();
        else if(op == ASTNode::Operator::AND)
            return left->toString() + " & " + right->toString();
        else if(op == ASTNode::Operator::FFTIMES)
            return left->toString() + " * " + right->toString();
        else if(op == ASTNode::Operator::XOR)
            return left->toString() + " ^ " + right->toString();
        else if(op == ASTNode::Operator::OR)
            return left->toString() + " | " + right->toString();
        else if(op == ASTNode::Operator::LSH)
            return left->toString() + " << " + right->toString();
        else if(op == ASTNode::Operator::RSH)
            return left->toString() + " >> " + right->toString();
        else if(op == ASTNode::Operator::MINUS)
            return left->toString() + " - " + right->toString();
        else {
            assert(false);
            return left->toString() + " * " + right->toString();
        }
    }

    virtual int value_of(const map<string, ValuePtr>& env) override {
        int lvalue = left->value_of(env);
        int rvalue = right->value_of(env);
        if(this->getOp() == ASTNode::Operator::ADD) {
            return lvalue + rvalue;
        } else if(this->getOp() == ASTNode::Operator::MINUS){
            return lvalue - rvalue;
        } else if(this->getOp() == ASTNode::Operator::FFTIMES){
            return lvalue * rvalue;
        } else if(this->getOp() == ASTNode::Operator::DIVIDE){
            return lvalue / rvalue;
        } else if(this->getOp() == ASTNode::Operator::XOR){
            return lvalue ^ rvalue;
        } else if(this->getOp() == ASTNode::Operator::LSH){
            return lvalue << rvalue;
        } else if(this->getOp() == ASTNode::Operator::AND){
            return lvalue & rvalue;
        } else if(this->getOp() == ASTNode::Operator::MOD){
            return lvalue % rvalue;
        } else if(this->getOp() == ASTNode::Operator::LE){
            if(lvalue <= rvalue) {
                return 1;
            } else {
                return 0;
            }
        } else if(this->getOp() == ASTNode::Operator::NE){
            if(lvalue == rvalue) {
                return 0;
            } else {
                return 1;
            }
        } else if(this->getOp() == ASTNode::Operator::EQ){
            if(lvalue == rvalue) {
                return 1;
            } else {
                return 0;
            }
        } else if(this->getOp() == ASTNode::Operator::LAND){
            if(lvalue == 1 && rvalue == 1) {
                return 1;
            } else {
                return 0;
            }
        } else {
            assert(false);
            return 0;
        }
    }

    const ValuePtr &getLeft() const;

    const ValuePtr &getRight() const;

    ASTNode::Operator getOp() const;

    virtual void replaceChild(ValuePtr v1, ValuePtr v2) override {
        if(left == v1) {
            left = v2;
        } else if(right == v1) {
            right = v2;
        } else {
            assert(left == v2 || right == v2);
        }
    }

    // 对bin来说，参数是remove哪个child
    virtual void removeChild(ValuePtr child) override {
        vector<ValuePtr>& childParents = child->getParents();
        auto it = childParents.begin();
        while(it != childParents.end()) {
            if(it->get() == this) {
                childParents.erase(it);
                break;
            } else {
                it++;
            }
        }
    }

    // 这是这个类自己的函数
    ValuePtr removeAnotherChild(ValuePtr child) {
        if(left == child) {
            removeChild(right);
            return right;
        }
        else if(right == child) {
            removeChild(left);
            return left;
        }
        else
            assert(false);
    }

    virtual ValueType getValueType() override {
        return ValueType::VTInternalBinValue;
    }

    virtual bool isRandomForBij() override {
        if(op == ASTNode::Operator::XOR ||
           op == ASTNode::Operator::ADD ||
           op == ASTNode::Operator::MINUS) {
            return true;
        }
        return false;
    }


};

typedef shared_ptr<InternalBinValue> InternalBinValuePtr;

class InternalUnValue : public Value {
private:
    ValuePtr rand;
    ASTNode::Operator op;
public:
    InternalUnValue(string name, const ValuePtr &rand, ASTNode::Operator op);
    virtual string toString() override {
        if(op == ASTNode::Operator::POW2)
            return "POW2 " + rand->toString();
        else if(op == ASTNode::Operator::POW4)
            return "POW4 " + rand->toString();
        else if(op == ASTNode::Operator::POW16)
            return "POW16 " + rand->toString();
        else if(op == ASTNode::Operator::AFFINE)
            return "AFFINE " + rand->toString();
        else if(op == ASTNode::Operator::TRCON)
            return "RCON " + rand->toString();
        else if(op == ASTNode::Operator::XTIMES)
            return "XTIMES " + rand->toString();
        else if(op == ASTNode::Operator::POL)
            return "POL " + rand->toString();
        else if(op == ASTNode::Operator::LIN)
            return "LIN " + rand->toString();
        else {
            assert(false);
            return "1";
        }
    }
    virtual int value_of(const map<string, ValuePtr>& env) override {
        int randValue = rand->value_of(env);
        if(op == ASTNode::Operator::POW2)
            return int(pow(randValue, 2)) % 256;
        else if(op == ASTNode::Operator::POW4)
            return int(pow(randValue,4)) % 256;
        else if(op == ASTNode::Operator::POW16)
            return int(pow(randValue,16)) % 256;
        else {
            assert(false);
            return 1;
        }
    }

    const ValuePtr &getRand() const;

    ASTNode::Operator getOp() const;

    virtual void replaceChild(ValuePtr v1, ValuePtr v2) override {
        assert(rand = v1);
        rand = v2;
    }

    virtual void removeChild(ValuePtr child) override {
        // 左孩子是random，那么就remove右孩子
        vector<ValuePtr>& childParents = rand->getParents();
        auto it = childParents.begin();
        while(it != childParents.end()) {
            if(it->get() == this) {
                childParents.erase(it);
                break;
            } else {
                it++;
            }
        }
    }

    virtual ValueType getValueType() override {
        return ValueType::VTInternalUnValue;
    }

    virtual bool isRandomForBij() override {
        if(op == ASTNode::Operator::NOT) {
            return true;
        }
        return false;
    }


};
typedef shared_ptr<InternalUnValue> InternalUnValuePtr;

class ArrayValue : public Value {
private:
    vector<ValuePtr> arrayValue;
public:
    ArrayValue(string name, const vector<ValuePtr> &arrayValue);
    virtual string toString() override {
        string res = "(Array type)";
        for(int i = 0; i < arrayValue.size(); i++) {
            if(i == 0) {
                if(!arrayValue[i]) {
                    res += "NULL";
                } else {
                    res += arrayValue[i]->toString();
                }
            } else {
                if(!arrayValue[i]) {
                    res += ", NULL";
                } else {
                    res += ", " + arrayValue[i]->toString();
                }
            }
        }
        return res;
    }

    ValuePtr getValueAt(int i) {
        return arrayValue[i];
    }

    void setValueAt(int loc, ValuePtr v) {
        arrayValue[loc] = v;
    }

    const vector<ValuePtr> &getArrayValue() const;

    virtual void replaceChild(ValuePtr v1, ValuePtr v2) override {
        bool flag = false;
        for(auto ele : arrayValue) {
            if(ele == v1) {
                flag = true;
                ele = v2;
            }
        }
        //assert(flag);
    }

    virtual ValueType getValueType() override {
        return ValueType::VTArrayValue;
    }
};

typedef shared_ptr<ArrayValue> ArrayValuePtr;


class Procedure {
private:
    string procName;
    vector<ValuePtr> assumptions;
    vector<ValuePtr> parameters;
    map<string, ValuePtr> env;
    vector<ValuePtr> block;
    vector<ValuePtr> hdBlock;
    ValuePtr returns;
    vector<set<ValuePtr>> domOfReturn;
public:
    const vector<ValuePtr> &getHdBlock() const;

public:
    Procedure(const string &procName, const vector<ValuePtr> &parameters, const vector<ValuePtr> &assumptions ,const map<string, ValuePtr>&env, const vector
              <ValuePtr> &block, const ValuePtr &returns, const vector <ValuePtr> &hdBlock);

    const string &getProcName() const;

    const vector<ValuePtr> &getParameters() const;

    vector<ValuePtr> &getBlock();

    const map<string, ValuePtr> &getEnv() const;

    const ValuePtr &getReturns() const;

    virtual string toString() {
        if(procName == "KeyExpansion") {
            return "ProcName: KeyExpansion jump\n";
        }
        string result = "";
        result += "ProcName: " + procName + "\n";

        result += "Parameters: ";
        for(int i = 0; i < parameters.size(); i++) {
            if(i == 0)
                result += parameters[i]->toString();
            else
                result += ", " + parameters[i]->toString();
        }
        result += "\n";
        result += "Assumptions: \n";
        for(auto ele : assumptions) {
            result += ele->toString() + "\n";
        }
        result += "Body: \n";
        for(auto ele : block) {
            if(ele != nullptr) {
                result += ele->toString() + "\n";
                cout << ele->toString() << endl;
            } else {
                result += "NULL\n";
            }
        }
        result += "Returns: \n";
        if(returns)
            result += returns->toString();

        return result;
    }

    const vector<ValuePtr> &getAssumptions() const;

    void setBlock(const vector<ValuePtr> &block);

    void setDomofReturn(const vector<set<ValuePtr>>& domOfReturn1) {
        domOfReturn = domOfReturn1;
    }

    vector<set<ValuePtr>>& getDomOfReturn() {
        return domOfReturn;
    }

};
typedef shared_ptr<Procedure> ProcedurePtr;

class ProcValue : public Value {
private:
    ProcedurePtr procedurePtr;
public:
    ProcValue(string name, const ProcedurePtr &procedurePtr);
    virtual string toString() override {
        return this->procedurePtr->toString();
    }

    string getProcName() {
        return procedurePtr->getProcName();
    }

    const vector<ValuePtr> &getParameters() {
        return procedurePtr->getParameters();
    }

    const ProcedurePtr &getProcedurePtr() const;

    virtual ValueType getValueType() override {
        return ValueType::VTProcValue;
    }
};
typedef shared_ptr<ProcValue> ProcValuePtr;

class ParameterValue : public Value {
private:
    string name;
public:
    ParameterValue(const string &name);
    virtual string toString() override {
        return "Parameter(" + name + ")";
    }

    string getName() override{
        return name;
    }

    virtual ValueType getValueType() override {
        return ValueType::VTParameterValue;
    }
};

typedef shared_ptr<ParameterValue> ParameterValuePtr;

class ConcreteNumValue : public Value {
private:
    int numer;
public:
    ConcreteNumValue(string name, int numer);

    int getNumer() const;

    virtual string toString() override {
        return to_string(numer);
    }


    virtual int value_of(const map<string, ValuePtr>& env) override {
        return numer;
    }

    virtual ValueType getValueType() override {
        return ValueType::VTConcreteNumValue;
    }
};

typedef shared_ptr<ConcreteNumValue> ConcreteNumValuePtr;

class ConstantValue : public Value {
private:
public:
    ConstantValue(string name);

    virtual ValueType getValueType() override {
        return ValueType::VTConstantValue;
    }

    virtual string toString() override {
        return "Parameter(Constant(" + Value::getName() + "))";
    }
};



class ProcCallValue : public Value {
private:
    ProcedurePtr procedurePtr;
    vector<ValuePtr> arguments;
    vector<int> callsite;
    vector<set<ValuePtr>> domOfProcCall;


public:
    ProcCallValue(string name, const ProcedurePtr &procedurePtr, const vector<ValuePtr> &arguments, vector<int> callsite);

    virtual string toString() override {
        string res = "";
        res += procedurePtr->getProcName() + "(";
//        for(int i = 0; i < arguments.size(); i++) {
//            if(i == 0) {
//                res += arguments[i]->toString();
//            } else {
//                res += ", " + arguments[i]->toString();
//            }
//        }
        res += "arguments";
        res += ")";
        string callsites;
        for(auto ele : callsite) {
            callsites += "@" + to_string(ele);
        }
        res += callsites;
        return res;
    }

    const ProcedurePtr &getProcedurePtr() const;

    const vector<ValuePtr> &getArguments() const;

    vector<int> getCallsite() const;

    virtual void replaceChild(ValuePtr v1, ValuePtr v2) override{
        bool flag = false;
        for(int i = 0; i < arguments.size(); i++) {
            // 如果第i个实际参数是v1，就换成v2
            if(arguments[i] == v1) {
                arguments[i] = v2;
                flag = true;
            }
            // 如果第i个实际参数已经替换为了v2,就不用替换
            if(arguments[i] == v2) {
                flag = true;
            }
            if(ArrayValue* array = dynamic_cast<ArrayValue*>(arguments[i].get())) {
                for(int i = 0; i < array->getArrayValue().size(); i++) {
                    if(array->getValueAt(i) == v1) {
                        array->setValueAt(i, v2);
                        flag = true;
                    } else if(array->getValueAt(i) == v2){
                        flag = true;
                    }
                }
            }
        }
        assert(flag);
    }

    //
    void addSummary() {
        map<ValuePtr, ValuePtr> saved;

        for(auto ele : procedurePtr->getDomOfReturn()) {
            set<ValuePtr> domOfThisReturn;
            for(auto ele1 : ele) {
                ValuePtr valuePtr = nullptr;
                if(saved.count(ele1) != 0) {
                    valuePtr = saved[ele1];
                } else {
                    RandomValue* randomValue = dynamic_cast<RandomValue*>(ele1.get());
                    valuePtr = make_shared<RandomValue>(randomValue->getName());
                    saved[ele1] = valuePtr;
                }
                domOfThisReturn.insert(valuePtr);
            }
            domOfProcCall.push_back(domOfThisReturn);
        }
    }

    set<ValuePtr> getDomOfNumberX(int i) {
        set<ValuePtr> temp;
        if(domOfProcCall.empty()) {
            return temp;
        } else {
            assert(domOfProcCall.size() > i);
            return domOfProcCall.at(i);
        }
    }

    //对于函数调用， remove的是本身
    virtual void removeChild(ValuePtr child) override {
        ValuePtr returnNode = nullptr;
        vector<ValuePtr>& childParents = child->getParents();
        auto it = childParents.begin();
        while(it != childParents.end()) {
            if(it->get() == this) {
                childParents.erase(it);
                break;
            } else {
                it++;
            }
        }
    }

    virtual ValueType getValueType() override {
        return ValueType::VTProcCallValue;
    }
};
typedef shared_ptr<ProcCallValue> ProcCallValuePtr;

class ProcCallValueIndex : public Value{
private:
    ValuePtr procCallValuePtr;
    int number;
public:
    ProcCallValueIndex(string name, const ValuePtr &procCallValuePtr, int number);

    const ValuePtr &getProcCallValuePtr() const;

    int getNumber() const;

    virtual string toString() override {
        return procCallValuePtr->toString() + "[" + to_string(number) + "]";
    }

    virtual ValueType getValueType() override {
        return ValueType::VTProcCallValueIndex;
    }

    /**
    virtual void removeNode(vector<RandomValuePtr>& toDo) override {

        // 如果该表达式中没有其他的用到函数返回值的地方，就可以把arg都干掉了
        ProcCallValue *procCallValuePtr = dynamic_cast<ProcCallValue *>(this->getProcCallValuePtr().get());
        bool noUseOfProc = true;
        for (auto ele : procCallValuePtr->getParents()) {
            if (ele->getParents().size() != 0) {
                noUseOfProc = false;
                break;
            }
        }
        if (noUseOfProc) {
            for (auto ele : procCallValuePtr->getArguments()) {
                if (ArrayValue *array = dynamic_cast<ArrayValue *>(ele.get())) {
                    // 如果参数是array的话，就依次remove
                    for (int i = 0; i < array->getArrayValue().size(); i++) {
                        ValuePtr returned = array->getValueAt(i);
                        procCallValuePtr->removeChild(returned);
                        if (returned && returned->getParents().size() == 0)
                            returned->removeNode(toDo);
                        else if (returned && returned->getParents().size() == 1) {
                            if(RandomValuePtr returnRand = dynamic_pointer_cast<RandomValue>(returned)) {
                                toDo.push_back(returnRand);
                            }
                        }
                    }
                } else {
                    ValuePtr returned = ele;
                    procCallValuePtr->removeChild(returned);
                    if (returned && returned->getParents().size() == 0)
                        returned->removeNode(toDo);
                    else if (returned && returned->getParents().size() == 1) {
                        if(RandomValuePtr returnRand = dynamic_pointer_cast<RandomValue>(returned)) {
                            toDo.push_back(returnRand);
                        }
                    }
                }
            }
        } else {
//                assert(false);
        }
    }
     **/
};

typedef shared_ptr<ProcCallValueIndex> ProcCallValueIndexPtr;

class AssumptionValue : public Value {
private:
public:

    AssumptionValue(const string &valueName) : Value(valueName) {}
    virtual string toString() override{
        return "Assumption Value";
    }
    virtual ValueType getValueType() override {
        return ValueType::VTArrayValueIndex;
    }

    virtual bool isSatisfied() = 0;

};

class DomAssValue : public AssumptionValue {
private:
    ValuePtr first;
    ValuePtr second;
public:
    DomAssValue(string name, const ValuePtr &first, const ValuePtr &second);
    virtual string toString() override {
        return "Dom: " + first->toString() + ", " + second->toString();
    }

    const ValuePtr &getFirst() const;

    const ValuePtr &getSecond() const;

    virtual ValueType getValueType() override {
        return ValueType::VTDomAssValue;
    }

    virtual bool isSatisfied() override {
        return false;
    }
};

class RValueAssValue : public AssumptionValue {
private:
    ValuePtr first;
    ValuePtr second;
public:
    RValueAssValue(string name, const ValuePtr &first, const ValuePtr &second);
    virtual string toString() override{
        return "RVar: " + first->toString() + ", " + second->toString();
    }

    const ValuePtr &getFirst() const;

    const ValuePtr &getSecond() const;

    virtual ValueType getValueType() override {
        return ValueType::VTRValueAssValue;
    }

    virtual bool isSatisfied() override {
        return false;
    }
};

class TypeAssValue : public AssumptionValue {
private:
    ValuePtr first;
    Type type;
public:
    TypeAssValue(string name, const ValuePtr &first, const Type &type);
    virtual string toString() override{
        string stype = "";
        if(type == Type::SI) {
            stype = "tSI";
        } else if(type == Type::LK) {
            stype = "tLK";
        } else if(type == Type::UF) {
            stype = "tUF";
        } else {
            stype = "tUK";
        }
        return "Type: " + first->toString() + ", " + stype;
    }

    const ValuePtr &getFirst() const;

    Type getType() const;

    virtual ValueType getValueType() override {
        return ValueType::VTTypeAssValue;
    }

    virtual bool isSatisfied() override {
        return false;
    }
};

class PsiAssValue : public Value {
private:
    vector<ValuePtr> assumptions;
public:
    PsiAssValue(string name, const vector<ValuePtr> &assumptions);

    const vector<ValuePtr> &getAssumptions() const;

    virtual string toString() override{
        string res = "";
        for(int i = 0; i < assumptions.size(); i++) {
            if(i == 0) {
                res += assumptions[i]->toString();
            } else {
                res += "\n" + assumptions[i]->toString();
            }
        }
        return res;
    }

    virtual ValueType getValueType() override {
        return ValueType::VTPsiAssValue;
    }

};

class ArrayValueIndex : public Value {
private:
    ValuePtr arrayValuePtr;
    ValuePtr symbolIndex;
public:
    ArrayValueIndex(const string &valueName, const ValuePtr &arrayValuePtr, const ValuePtr &symbolIndex);
    virtual string toString() override{
        return arrayValuePtr->getName() + "[SYMBOL]";
    }
    virtual ValueType getValueType() override {
        return ValueType::VTArrayValueIndex;
    }

    const ValuePtr &getArrayValuePtr() const;

    const ValuePtr &getSymbolIndex() const;

};

#endif //SAFELANG_VALUE_H
