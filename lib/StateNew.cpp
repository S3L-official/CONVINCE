//
// Created by Pengfei Gao on 2022/3/14.
//

#include <algorithm>
#include "StateNew.h"

void StateNew::setTop(const set<ThreeAddressNodePtr>& observableSet) {
     for(auto ins : observableSet) {
         ThreeAddressNodePtr copidNode = addExprToState(ins);
         if(nodeHasTop.count(copidNode) == 0) {
             copidNode->addParents(StateNew::top);
             nodeHasTop.insert(copidNode);
         }
     }
}

ThreeAddressNodePtr StateNew::addExprToState(ThreeAddressNodePtr node) {
    if(!node.get())
        return nullptr;

    if(saved.count(node->getNodeName()) != 0) {
        assert(saved[node->getNodeName()] != nullptr);
        return saved[node->getNodeName()];
    }

    ThreeAddressNodePtr newNode = nullptr;//make_shared<ThreeAddressNode>(node, saved);
    if(node->getNodeType() == NodeType::RANDOM) {
        newNode = make_shared<ThreeAddressNode>(node);
        copiedRandoms.push_back(newNode);
    } else if(node->getNodeType() == NodeType::CONSTANT || node->getNodeType() == NodeType::PUBLIC) {
        newNode = make_shared<ThreeAddressNode>(node);
    } else if(node->getNodeType() == NodeType::PARAMETER) {
        newNode = make_shared<ThreeAddressNode>(node);
        copiedParameters.push_back(newNode);


    } else if(node->getNodeType() == NodeType::PRIVATE) {
        newNode = make_shared<ThreeAddressNode>(node);
        copiedPrivate.push_back(newNode);
    }
    else if(node->getNodeType() == NodeType::INTERNAL) {
        if(elementsInY.count(node) != 0) {
            newNode = make_shared<ThreeAddressNode>(node);
            newNode->setLhs(nullptr);
            newNode->setRhs(nullptr);
            copiedInternalFromY.push_back(newNode);
        } else {
            ThreeAddressNodePtr lhs = addExprToState(node->getLhs());

            ThreeAddressNodePtr rhs = nullptr;
            if (node->getRhs()) {
                rhs = addExprToState(node->getRhs());
            }

            newNode = make_shared<ThreeAddressNode>(node);
            newNode->setLhs(lhs);
            newNode->setRhs(rhs);
        }


    }

    else {
        assert(false);
    }

    setParents(newNode);

    saved[node->getNodeName()] = newNode;

    return newNode;
}

void StateNew::obtainKey() {
    if(!copiedPrivate.empty() && copiedParameters.empty()) {
        copiedKeys.insert(copiedPrivate.begin(), copiedPrivate.end());
    } else if(copiedPrivate.empty() && !copiedParameters.empty()) {
        copiedKeys.insert(copiedParameters.begin(), copiedParameters.end());
    } else if(copiedPrivate.empty() && copiedParameters.empty()){
    } else {
        assert(false);
    }

    if(!copiedInternalFromY.empty()) {
        copiedKeys.insert(copiedInternalFromY.begin(), copiedInternalFromY.end());
    }
}

bool StateNew::canSimulate() {
    if(checkingType == CheckingType::prob) {
        assert(Y.empty());
        return copiedKeys.empty();
    } else if(CheckingType::ni){
        assert(false);
        return false;
    } else if(CheckingType::ydsni){
        return canCoverSetNew();
    }
}

bool StateNew::canCoverSet() {
    map<int, int> sizeOfEachSetInY;
    map<int, int> curSizeOfEachSetInY;
    for(int j = 0; j < copiedYname.size(); j++) {
        sizeOfEachSetInY[j] = copiedYname[j].size();
    }
    map<string, ThreeAddressNodePtr> copiedKeysSet;
    set<string> copiedKeysNameSet;
    for(auto ele : copiedKeys) {
        copiedKeysSet[ele->getNodeName()] = ele;
        copiedKeysNameSet.insert(ele->getNodeName());
    }
    set<set<string>> subsetOfY;
    int count = 0;
    bool stopContinue = false;

    set<string> obtained;
    while(!copiedKeysNameSet.empty() && !stopContinue) {
        stopContinue = true;
        count++;
        curSizeOfEachSetInY.clear();
        for(auto curNode : copiedKeysNameSet) {
            bool find = false;
            if(obtained.count(curNode) != 0) continue;
            for(int j = 0; j < copiedYname.size(); j++) {
                if(copiedYname[j].count(curNode) != 0) {// && alreadyIn.count(curNode.first) == 0) {
                    curSizeOfEachSetInY[j]++;
                    if(curSizeOfEachSetInY[j] == sizeOfEachSetInY[j]) {
                        for(auto ele : copiedYname[j]) {
                            obtained.insert(ele);
                        }
                        find = true;
                        stopContinue = false;
                        subsetOfY.insert(copiedYname[j]);
                        break;
                    }
                }
            }
            if(find) break;
        }

    }
    if(obtained.size() < copiedKeysNameSet.size()) {

        set<string> difference;
        for(auto ele : copiedKeysNameSet) {
            if(obtained.count(ele) == 0) difference.insert(ele);
        }

        for(auto curNode : difference) {
            for(int j = 0; j < copiedYname.size(); j++) {
                if(copiedYname[j].count(curNode) != 0) {// && alreadyIn.count(curNode.first) == 0) {
                    subsetOfY.insert(copiedYname[j]);
                    break;
                }
            }
        }
        if(subsetOfY.size() > order)
            return false;
        else {
            copiedProofName = subsetOfY;
            return true;
        }
    } else {
        if (subsetOfY.size() <= order) {
            copiedProofName = subsetOfY;
            return true;
        } else {
            return false;
        }
    }


}


bool StateNew::canCoverSetNew() {
    map<int, int> sizeOfEachSetInY;
    map<int, int> curSizeOfEachSetInY;
    for(int j = 0; j < copiedYname.size(); j++) {
        sizeOfEachSetInY[j] = copiedYname[j].size();
    }

    map<string, ThreeAddressNodePtr> copiedKeysSet;
    set<string> copiedKeysNameSet;
    for(auto ele : copiedKeys) {
        copiedKeysSet[ele->getNodeName()] = ele;
        copiedKeysNameSet.insert(ele->getNodeName());
    }
    set<set<string>> subsetOfY;
    set<set<string>> witness;
    set<string> obtained;

    int max = 0;
    int maxIndex = 0;
    for (auto curNode: copiedKeysNameSet) {
        for (int j = 0; j < copiedYname.size(); j++) {
            if (copiedYname[j].count(curNode) != 0) {// && alreadyIn.count(curNode.first) == 0) {
                curSizeOfEachSetInY[j]++;
                if (curSizeOfEachSetInY[j] > max) {
                    max = curSizeOfEachSetInY[j];
                    maxIndex = j;
                }
            }
        }
    }

    for(int i = 0; i < order; i++) {
        if(curSizeOfEachSetInY.empty()) {
            continue;
        }
        if(curSizeOfEachSetInY[maxIndex] == 0)
            continue;
        set<string> choosed = copiedYname[maxIndex];
        obtained.insert(choosed.begin(), choosed.end());
        subsetOfY.insert(choosed);
        witness.insert(choosed);
        curSizeOfEachSetInY.erase(maxIndex);
        for(auto ele : choosed) {
            for(int j = 0; j < copiedYname.size(); j++) {
                if(curSizeOfEachSetInY.count(j) == 0) continue;
                if(copiedYname[j].count(ele) != 0) {
                    curSizeOfEachSetInY[j]--;
                }

            }
        }
        max = curSizeOfEachSetInY.begin()->second;
        maxIndex = curSizeOfEachSetInY.begin()->first;
        for(auto ele : curSizeOfEachSetInY) {
            if(ele.second > max) {
                max = ele.second;
                maxIndex = ele.first;
            }
        }

    }

    if(std::includes(obtained.begin(), obtained.end(), copiedKeysNameSet.begin(), copiedKeysNameSet.end())) {
        copiedProofName = subsetOfY;
        return true;
    } else {
        return false;
    }





}

bool StateNew::isDominateOperation(ASTNode::Operator op) {
    if(op == ASTNode::Operator::XOR ||
       op == ASTNode::Operator::ADD ||
       op == ASTNode::Operator::MINUS ||
       op == ASTNode::Operator::NOT) {
        return true;
    } else
        return false;
}

void StateNew::removeNode(ThreeAddressNodePtr node) {
    if(saved.count(node->getNodeName()) == 0)
        return;
    assert(saved.count(node->getNodeName()) > 0);
    assert((node->getParents()).size() == 0);

    if(node == nullptr)
        return;

    saved.erase(node->getNodeName());

    if(node->getNodeType() == NodeType::RANDOM) {
        auto it = copiedRandoms.begin();
        for(; it != copiedRandoms.end(); ) {
            if((*it) == node) {
                it = copiedRandoms.erase(it);
                break;
            }
            else
                ++it;
        }

    } else if(node->getNodeType() == NodeType::CONSTANT || node->getNodeType() == NodeType::PUBLIC ||
              node->getNodeType() == NodeType::PARAMETER || node->getNodeType() == NodeType::PRIVATE) {

        copiedParameters.erase(std::remove(copiedParameters.begin(), copiedParameters.end(), node), copiedParameters.end());
        copiedKeys.erase(node);
    }
    else if(node->getOp() == ASTNode::Operator::NOT
            || node->getOp() == ASTNode::Operator::SBOX
            || node->getOp() == ASTNode::Operator::AFFINE
            || node->getOp() == ASTNode::Operator::POW2
            || node->getOp() == ASTNode::Operator::POW4
            || node->getOp() == ASTNode::Operator::POL
            || node->getOp() == ASTNode::Operator::TABLELUT
            || node->getOp() == ASTNode::Operator::TABLELUT4
            || node->getOp() == ASTNode::Operator::TRCON
            || node->getOp() == ASTNode::Operator::XTIMES
            || node->getOp() == ASTNode::Operator::POW16) {


        removeChild(node->getLhs(), node);
    }
    else if(node->getOp() == ASTNode::Operator::XOR
            || node->getOp()  == ASTNode::Operator ::FFTIMES
            || node->getOp()  == ASTNode::Operator ::AND
            || node->getOp()  == ASTNode::Operator ::MINUS
            || node->getOp()  == ASTNode::Operator ::ADD
            || node->getOp()  == ASTNode::Operator ::OR
            || node->getOp()  == ASTNode::Operator ::MUL
            || node->getOp()  == ASTNode::Operator ::LSH
            || node->getOp() == ASTNode::Operator  ::RSH) {
        removeChild(node->getLhs(), node);
        removeChild(node->getRhs(), node);
    } else if(node->getOp() == ASTNode::Operator::CALL) {
    }
    else {
        assert(false);
    }
}

void StateNew::removeChild(ThreeAddressNodePtr child, ThreeAddressNodePtr parent) {
    if(child == nullptr)
        return;
    vector<ThreeAddressNodeWeakPtr>& parents = child->getParents();
    auto it = parents.begin();
    while(it != parents.end()){
        if((*it).lock() == parent) {
            parents.erase(it);
            break;
        } else {
            it++;
        }
    }

    if(child->getParents().size() == 0) {
        removeNode(child);
    }
    else if(child->getParents().size() == 1 && child->getNodeType() == NodeType::RANDOM
                && child->getParents()[0].lock() != StateNew::top)
        todo.push_back(child);
}

void StateNew::applySimplification(ThreeAddressNodePtr random) {

    ThreeAddressNodePtr parent = random->getParents()[0].lock();
    random->getParents().clear();

    domExpressions.push_back(DomExpressionPair(random, parent));
    replacedNode[parent] = random;

    if(parent->getLhs() == random) {
        removeChild(parent->getRhs(), parent);
    } else {
        removeChild(parent->getLhs(), parent);
    }

     replaceAbyB(parent, random);
    if(random->getParents().size() == 1)
        todo.push_back(random);
}

void StateNew::replaceAbyB(ThreeAddressNodePtr parent, ThreeAddressNodePtr random) {
    random->getParents().clear();
    for(auto ele : parent->getParents()) {
        if(ele.lock() != StateNew::top) {
            if (ele.lock()->getLhs() == parent) {
                ele.lock()->setLhs(random);
            } else if (ele.lock()->getRhs() == parent) {
                ele.lock()->setRhs(random);
            } else
                assert(false);
        } else {
            nodeHasTop.insert(random);
        }
        random->addParents(ele.lock());
    }

    parent->getParents().clear();
    parent->setLhs(nullptr);
    parent->setRhs(nullptr);
    removeNode(parent);

}

void StateNew::dominateSimplification() {
    assert(!todo.empty());
    ThreeAddressNodePtr random = todo.back();
    todo.pop_back();

    if (random->getParents().size() == 1 && random->getParents()[0].lock() != StateNew::top) {
        ThreeAddressNodePtr parent = random->getParents()[0].lock();
        assert(parent != StateNew::top);
        if (isDominateOperation(parent->getOp())) {
            applySimplification(random);
        }
    }
}

bool StateNew::domSimplifyUntilCanSimulate() {
    for(auto randoms : copiedRandoms) {
        if(randoms->getParents().size() == 1 && randoms->getParents()[0].lock() != StateNew::top)
            todo.push_back(randoms);
    }
    while(!todo.empty()) {
        dominateSimplification();
    }
    return canSimulate();

}

void StateNew::removeAllNodeAndParent(ThreeAddressNodeWeakPtr n1) {
    if(n1.expired())
        return;
    if(n1.lock()->getParents().empty())
        return;
    ThreeAddressNodePtr n = n1.lock();
    if(n != top) {
        while(!(n->getParents().size() == 0)) {
            ThreeAddressNodeWeakPtr par = n->getParents().back();
            n->getParents().pop_back();
            removeAllNodeAndParent(par);
        }
        n->getParents().clear();
        removeNode(n);
    }
}

bool StateNew::hasTop(ThreeAddressNodePtr node) {
    ThreeAddressNodePtr copiedNode = nullptr;
    if(saved.count(node->getNodeName()) == 0) {
        for(auto ele : replacedNode) {
            if(ele.first->getNodeName() == node->getNodeName()) {
                return hasTop(ele.second);
            }
        }
        return false;
    } else {
        copiedNode = saved[node->getNodeName()];
        for (auto it : copiedNode->getParents()) {
            if (it.lock() == top)
                return true;
        }
    }
    return false;
}

void StateNew::dealWithNewKeyExps(set<ThreeAddressNodePtr>& newkeyExps) {
    int margin = order - copiedProofName.size();
    set<set<string>> newAddedProof;
    for(auto ele : copiedYname) {
        if(margin == 0)
            break;
        if(copiedProofName.count(ele) == 0) {
            newAddedProof.insert(ele);
            margin--;
        }
    }
    set<string> alreadyHaved;
    for(auto ele : newAddedProof) {
        alreadyHaved.insert(ele.begin(), ele.end());
    }
    for(auto ele : copiedProofName) {
        alreadyHaved.insert(ele.begin(), ele.end());
    }
    set<ThreeAddressNodePtr> res;
    for(auto ele : newkeyExps) {
        if(alreadyHaved.count(ele->getNodeName()) == 0)
            res.insert(ele);
    }
    newkeyExps = res;
}

void StateNew::dealWithNewKeyExpsWSNI(set<ThreeAddressNodePtr>& newkeyExps) {
    set<string> alreadyHaved;
    for(auto ele : copiedProofName) {
        alreadyHaved.insert(ele.begin(), ele.end());
    }
    set<ThreeAddressNodePtr> res;
    for(auto ele : newkeyExps) {
        if(alreadyHaved.count(ele->getNodeName()) == 0)
            res.insert(ele);
    }
    newkeyExps = res;
}


ThreeAddressNodePtr StateNew::top = make_shared<ThreeAddressNode>("top_node", nullptr, nullptr, ASTNode::Operator::NULLOP, NodeType::UNKNOWN);

const set<ThreeAddressNodePtr> &StateNew::getCopiedKeys() const {
    return copiedKeys;
}

string StateNew::printState(string title) {
    string res = title + "====================================\n";
    for(auto ele : saved) {
        string parents = "[";
        for(auto ele1 : ele.second->getParents()) {
            parents += ele1.lock()->getNodeName() + ", ";
        }
        parents += "]";
        res += ele.first + parents + ": " + ele.second->prettyPrint5();
        res += "\n";
    }
    res += "====================================";
    return res;
}

const vector<DomExpressionPair> &StateNew::getDomExpressions() const {
    return domExpressions;
}

const set<set<string>> &StateNew::getCopiedProofName() const {
    return copiedProofName;
}

void StateNew::setCopiedKeys(const set<ThreeAddressNodePtr> &copiedKeys) {
    StateNew::copiedKeys = copiedKeys;
}
