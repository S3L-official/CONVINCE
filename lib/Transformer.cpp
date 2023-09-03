//
// Created by Pengfei Gao on 2022/2/24.
//

#include "Transformer.h"
#include "ThreeAddressNode.h"




ProcedureHPtr Transformer::transform(ProcValuePtr procValuePtr) {
    vector<vector<ThreeAddressNodePtr>> parameters;
    vector<ThreeAddressNodePtr> block;
    vector<ThreeAddressNodePtr> returns;

    bool isSimple = true;

    map<ValuePtr, ThreeAddressNodePtr> saved;
    map<string, int> nameCount;

    for(auto ele : procValuePtr->getParameters()) {
        if(ArrayValuePtr arrayValuePtr = dynamic_pointer_cast<ArrayValue>(ele)) {
            vector<ThreeAddressNodePtr> paras;
            for(auto ele1 : arrayValuePtr->getArrayValue()) {
                ThreeAddressNodePtr para(new ThreeAddressNode(ele1->getName(), nullptr, nullptr, ASTNode::Operator::NULLOP, NodeType::PARAMETER));
                paras.push_back(para);
                saved[ele1] = para;
            }
            parameters.push_back(paras);
        } else {
            vector<ThreeAddressNodePtr> paras;
            ThreeAddressNodePtr para(new ThreeAddressNode(ele->getName(), nullptr, nullptr, ASTNode::Operator::NULLOP, NodeType::PARAMETER));
            paras.push_back(para);
            saved[ele] = para;
            parameters.push_back(paras);
        }
    }

    if(procValuePtr->getName() == "preshare" || procValuePtr->getName() == "preshare_public" ) {
        for(auto ele : procValuePtr->getProcedurePtr()->getBlock()) {
            if(RandomValuePtr randomValuePtr = dynamic_pointer_cast<RandomValue>(ele)) {
                if(saved.count(randomValuePtr) == 0)
                    block.push_back(transformRandom(randomValuePtr, saved, nameCount));
            } else if(InternalBinValuePtr internalBinValuePtr = dynamic_pointer_cast<InternalBinValue>(ele)) {
                block.push_back(transformInternalBinForShare(internalBinValuePtr, saved, nameCount));
            }
        }
    } else {

        for (auto ele : procValuePtr->getProcedurePtr()->getBlock()) {
            if (InternalBinValuePtr internalBinValuePtr = dynamic_pointer_cast<InternalBinValue>(ele)) {
                block.push_back(transformInternalBin(internalBinValuePtr, saved, nameCount));
            } else if (InternalUnValuePtr internalUnValue = dynamic_pointer_cast<InternalUnValue>(ele)) {
                block.push_back(transformInternalUn(internalUnValue, saved, nameCount));
            } else if (ArrayValuePtr arrayValuePtr = dynamic_pointer_cast<ArrayValue>(ele)) {
                vector<ThreeAddressNodePtr> res = transformArrayValue(arrayValuePtr, saved, nameCount, isSimple);
                block.insert(block.end(), res.begin(), res.end());
            } else if (RandomValuePtr randomValuePtr = dynamic_pointer_cast<RandomValue>(ele)) {
                block.push_back(transformRandom(randomValuePtr, saved, nameCount));
            }
        }
    }
    if(ArrayValuePtr arrayValuePtr = dynamic_pointer_cast<ArrayValue>(procValuePtr->getProcedurePtr()->getReturns())) {
        for(auto ele1 : arrayValuePtr->getArrayValue()) {
            if(saved.count(ele1) != 0) {
                returns.push_back(saved[ele1]);
            } else {
                if (InternalBinValuePtr internalBinValuePtr = dynamic_pointer_cast<InternalBinValue>(ele1)) {
                    returns.push_back(transformInternalBin(internalBinValuePtr, saved, nameCount));
                } else if (InternalUnValuePtr internalUnValue = dynamic_pointer_cast<InternalUnValue>(ele1)) {
                    returns.push_back(transformInternalUn(internalUnValue, saved, nameCount));
                } else if(ConcreteNumValuePtr concreteNumValue = dynamic_pointer_cast<ConcreteNumValue>(ele1)){
                    auto res = make_shared<ThreeAddressNode>(to_string(concreteNumValue->getNumer()), nullptr, nullptr,
                                                             ASTNode::Operator::NULLOP, NodeType::CONSTANT);
                    returns.push_back(res);
                    saved[ele1] = res;
                } else {
                    assert(false);
                }
            }
        }
    } else {
        returns.push_back(saved[procValuePtr->getProcedurePtr()->getReturns()]);
    }

    map<string, ProcedureHPtr> nameToProc;
    for(auto ele : procedureHs) {
        nameToProc[ele->getName()] = ele;
    }

    auto newProc = ProcedureHPtr(new ProcedureH(procValuePtr->getName(), parameters, block, returns, nameToProc));
    newProc->setIsSimple(isSimple);
    return newProc;

}

ThreeAddressNodePtr Transformer::transformInternalBinForShare(InternalBinValuePtr internalBinValuePtr, map<ValuePtr, ThreeAddressNodePtr>& saved, map<string, int>& nameCount) {
    ThreeAddressNodePtr left = nullptr;
    ThreeAddressNodePtr right = nullptr;
    string name = "";
    if(internalBinValuePtr->getName() == "")
        name = "t";
    else
        name = internalBinValuePtr->getName();

    if(internalBinValuePtr->getLeft()->getValueType() == ValueType::VTRandomVable &&
       saved.count(internalBinValuePtr->getLeft()) == 0) {
        string count = getCount(internalBinValuePtr->getLeft()->getName(), nameCount);
        left = make_shared<ThreeAddressNode>(internalBinValuePtr->getLeft()->getName() + count, nullptr, nullptr, ASTNode::Operator::NULLOP, NodeType::RANDOM);
        saved[internalBinValuePtr->getLeft()] = left;
    } else if(ParameterValuePtr parameterValuePtr = dynamic_pointer_cast<ParameterValue>(internalBinValuePtr->getLeft())) {
        if(saved.count(parameterValuePtr) == 0) {
            string count = getCount(internalBinValuePtr->getLeft()->getName(), nameCount);
            left = make_shared<ThreeAddressNode>(internalBinValuePtr->getLeft()->getName() + count, nullptr, nullptr, ASTNode::Operator::NULLOP, NodeType::PARAMETER);
            saved[internalBinValuePtr->getLeft()] = left;
        } else {
            left = saved[parameterValuePtr];
        }

    } else if(InternalBinValuePtr internalBinValuePtr1 = dynamic_pointer_cast<InternalBinValue>(internalBinValuePtr->getLeft())) {
        if(saved.count(internalBinValuePtr1) == 0) {
            left = transformInternalBinForShare(internalBinValuePtr1, saved, nameCount);
        } else {
            left = saved[internalBinValuePtr->getLeft()];
        }
    }
    if(saved.count(internalBinValuePtr->getRight()) != 0) {
        right = saved[internalBinValuePtr->getRight()];
    } else if(internalBinValuePtr->getRight()->getValueType() == ValueType::VTRandomVable &&
              saved.count(internalBinValuePtr->getRight()) == 0) {
        string count = getCount(internalBinValuePtr->getRight()->getName(), nameCount);
        right = make_shared<ThreeAddressNode>(internalBinValuePtr->getRight()->getName() + count, nullptr, nullptr, ASTNode::Operator::NULLOP, NodeType::RANDOM);
        saved[internalBinValuePtr->getRight()] = right;
    } else if(InternalBinValuePtr internalBinValuePtr1 = dynamic_pointer_cast<InternalBinValue>(internalBinValuePtr->getRight())) {
        if(saved.count(internalBinValuePtr1) == 0) {
            right = transformInternalBinForShare(internalBinValuePtr1, saved, nameCount);
        } else {
            right = saved[internalBinValuePtr->getRight()];
        }
    }

    string count = getCount(name, nameCount);
    ThreeAddressNodePtr para(new ThreeAddressNode(name + count, left, right, internalBinValuePtr->getOp(), NodeType::INTERNAL));
    left->addParents(para);
    right->addParents(para);
    saved[internalBinValuePtr] = para;

    return para;

}

ThreeAddressNodePtr Transformer::transformInternalBin(InternalBinValuePtr internalBinValuePtr, map<ValuePtr, ThreeAddressNodePtr>& saved, map<string, int>& nameCount) {
    ThreeAddressNodePtr left = nullptr;
    ThreeAddressNodePtr right = nullptr;
    string name = "";
    if(internalBinValuePtr->getName() == "")
        name = "t";
    else
        name = internalBinValuePtr->getName();

    if(internalBinValuePtr->getLeft()->getValueType() == ValueType::VTRandomVable &&
       saved.count(internalBinValuePtr->getLeft()) == 0) {
        string count = getCount(internalBinValuePtr->getLeft()->getName(), nameCount);
        left = make_shared<ThreeAddressNode>(internalBinValuePtr->getLeft()->getName() + count, nullptr, nullptr, ASTNode::Operator::NULLOP, NodeType::RANDOM);
        saved[internalBinValuePtr->getLeft()] = left;
    } else if(ConcreteNumValuePtr concreteNumValuePtr = dynamic_pointer_cast<ConcreteNumValue>(internalBinValuePtr->getLeft())){
        if(saved.count(internalBinValuePtr->getLeft()) == 0) {
            left = make_shared<ThreeAddressNode>(to_string(concreteNumValuePtr->getNumer()), nullptr, nullptr,
                                                 ASTNode::Operator::NULLOP, NodeType::CONSTANT);
            saved[internalBinValuePtr->getLeft()] = left;
        } else {
            left = saved[internalBinValuePtr->getLeft()];
        }
    } else {
        left = saved[internalBinValuePtr->getLeft()];
        assert(left);
    }

    if(internalBinValuePtr->getRight()->getValueType() == ValueType::VTRandomVable &&
       saved.count(internalBinValuePtr->getRight()) == 0) {
        string count = getCount(internalBinValuePtr->getRight()->getName(), nameCount);
        right = make_shared<ThreeAddressNode>(internalBinValuePtr->getRight()->getName() + count, nullptr, nullptr, ASTNode::Operator::NULLOP, NodeType::RANDOM);
        saved[internalBinValuePtr->getRight()] = right;
    } else if(ConcreteNumValuePtr concreteNumValuePtr = dynamic_pointer_cast<ConcreteNumValue>(internalBinValuePtr->getRight())){
        if(saved.count(internalBinValuePtr->getRight()) == 0) {
            right = make_shared<ThreeAddressNode>(to_string(concreteNumValuePtr->getNumer()), nullptr, nullptr,
                                                  ASTNode::Operator::NULLOP, NodeType::CONSTANT);
            saved[internalBinValuePtr->getRight()] = right;
        } else {
            right = saved[internalBinValuePtr->getRight()];
        }
    }
    else {
        right = saved[internalBinValuePtr->getRight()];
        assert(right);
    }

    string count = getCount(name, nameCount);
    ThreeAddressNodePtr para(new ThreeAddressNode(name + count, left, right, internalBinValuePtr->getOp(), NodeType::INTERNAL));
    left->addParents(para);
    right->addParents(para);
    saved[internalBinValuePtr] = para;

    return para;

}

ThreeAddressNodePtr Transformer::transformInternalUn(InternalUnValuePtr internalUnValuePtr, map<ValuePtr, ThreeAddressNodePtr>& saved, map<string, int>& nameCount) {

    string name = "";
    if(internalUnValuePtr->getName() == "")
        name = "t";
    else
        name = internalUnValuePtr->getName();

    ThreeAddressNodePtr operation = saved.count(internalUnValuePtr->getRand()) == 0 ? nullptr : saved[internalUnValuePtr->getRand()];

    if(operation == nullptr) {
        if(ConcreteNumValuePtr concreteNumValuePtr = dynamic_pointer_cast<ConcreteNumValue>(internalUnValuePtr->getRand())) {
            operation =  make_shared<ThreeAddressNode>(to_string(concreteNumValuePtr->getNumer()), nullptr, nullptr, ASTNode::Operator::NULLOP, NodeType::CONSTANT);
            saved[internalUnValuePtr->getRand()] = operation;
        } else {
            assert(false);
        }
    }

    string count = getCount(name, nameCount);
    ThreeAddressNodePtr para(new ThreeAddressNode(name + count, operation, nullptr, internalUnValuePtr->getOp(), NodeType::INTERNAL));

    operation->addParents(para);

    saved[internalUnValuePtr] = para;

    return para;
}


vector<ThreeAddressNodePtr> Transformer::transformArrayValue(ArrayValuePtr arrayValuePtr, map<ValuePtr, ThreeAddressNodePtr>& saved, map<string, int>& nameCount, bool& isSimple) {
    vector<ThreeAddressNodePtr> res;

    for(auto ele : arrayValuePtr->getArrayValue()) {
        if(ProcCallValueIndexPtr procCallValueIndexPtr = dynamic_pointer_cast<ProcCallValueIndex>(ele)) {
            isSimple = false;
            ProcCallValuePtr procCallValuePtr = dynamic_pointer_cast<ProcCallValue>(procCallValueIndexPtr->getProcCallValuePtr());
            for(int i = 0; i < procCallValuePtr->getArguments().size(); i++) {
                auto ele = procCallValuePtr->getArguments()[i];
                if(ArrayValuePtr arrayValuePtr1 = dynamic_pointer_cast<ArrayValue>(ele)) {
                    for(auto ele1 : arrayValuePtr1->getArrayValue()) {
                        if(ConcreteNumValuePtr concreteNumValue = dynamic_pointer_cast<ConcreteNumValue>(ele1)) {
                            auto tempres = make_shared<ThreeAddressNode>(to_string(concreteNumValue->getNumer()),
                                                                         nullptr, nullptr,
                                                                         ASTNode::Operator::NULLOP, NodeType::CONSTANT);
                            res.push_back(tempres);
                            saved[ele1] = tempres;
                            ThreeAddressNodePtr threeAddressNodePtr(
                                    new ThreeAddressNode("push", saved[ele1], nullptr, ASTNode::Operator::PUSH,
                                                         NodeType::INTERNAL));
                        } else {
                            assert(saved[ele1]);
                            ThreeAddressNodePtr threeAddressNodePtr(
                                    new ThreeAddressNode("push", saved[ele1], nullptr, ASTNode::Operator::PUSH,
                                                         NodeType::INTERNAL));
                            res.push_back(threeAddressNodePtr);
                        }
                    }
                } else if(ConcreteNumValuePtr concreteNumValue = dynamic_pointer_cast<ConcreteNumValue>(ele)) {
                    auto tempres = make_shared<ThreeAddressNode>(to_string(concreteNumValue->getNumer()), nullptr, nullptr,
                                                                 ASTNode::Operator::NULLOP, NodeType::CONSTANT);
                    res.push_back(tempres);
                    saved[ele] = tempres;
                } else {
                    assert(saved[ele]);
                    ThreeAddressNodePtr threeAddressNodePtr(new ThreeAddressNode("push", saved[ele], nullptr, ASTNode::Operator::PUSH, NodeType::INTERNAL));
                    res.push_back(threeAddressNodePtr);
                }
            }
        }
        break;
    }

    for(int i = 0; i < arrayValuePtr->getArrayValue().size(); i++) {
        auto ele = arrayValuePtr->getArrayValue()[i];
        if(ProcCallValueIndexPtr procCallValueIndexPtr = dynamic_pointer_cast<ProcCallValueIndex>(ele)) {
            isSimple = false;
            ThreeAddressNodePtr func = nullptr;
            if(saved.count(procCallValueIndexPtr->getProcCallValuePtr()) == 0) {
                ThreeAddressNodePtr function(new ThreeAddressNode(procCallValueIndexPtr->getProcCallValuePtr()->getName(), nullptr, nullptr, ASTNode::Operator::NULLOP, NodeType::FUNCTION));
                saved[procCallValueIndexPtr->getProcCallValuePtr()] = function;
                func = function;
            } else {
                func = saved[procCallValueIndexPtr->getProcCallValuePtr()];
            }

            string name = "t";
            string count = getCount(name, nameCount);
            ThreeAddressNodePtr threeAddressNodePtr(new ThreeAddressNode(name + count, func, nullptr, ASTNode::Operator::CALL, NodeType::INTERNAL));
            threeAddressNodePtr->setIndexCall(i);
            res.push_back(threeAddressNodePtr);
            saved[ele] = threeAddressNodePtr;
        } else {
        }
    }
    return res;
}


ThreeAddressNodePtr Transformer::transformRandom(RandomValuePtr randomValuePtr, map<ValuePtr, ThreeAddressNodePtr>& saved,
                                               map<string, int> & nameCount) {
    ThreeAddressNodePtr random = nullptr;
    if(saved.count(randomValuePtr) != 0)
        return saved[randomValuePtr];
    string count = getCount(randomValuePtr->getName(), nameCount);
    random = make_shared<ThreeAddressNode>(randomValuePtr->getName() + count, nullptr, nullptr, ASTNode::Operator::NULLOP, NodeType::RANDOM);
    saved[randomValuePtr] = random;
    return random;

}
void Transformer::transformProcedures() {
    for(auto ele : procedures) {
        ProcedureHPtr temp = transform(ele);
        nameToProc[ele->getName()] = temp;
        procedureHs.push_back(temp);
    }
}
string Transformer::getCount(string name, map<string, int>& nameCount){
    if(nameCount.count(name) == 0) {
        nameCount[name] = 0;
        return "";
    } else {
        nameCount[name]++;
        return to_string(nameCount[name]);
    }
}

const vector<ProcedureHPtr> &Transformer::getProcedureHs() const {
    return procedureHs;
}
