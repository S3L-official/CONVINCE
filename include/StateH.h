//
// Created by Pengfei Gao on 2020/10/14.
//

#ifndef SAFELANG_STATEH_H
#define SAFELANG_STATEH_H

#include "ThreeAddressNode.h"
#include "Utils.h"


enum CheckType {NI, SNI, Probing, WSNI};
typedef pair<ThreeAddressNodePtr, ThreeAddressNodePtr> DomExpressionPair;
class StateH {
private:
    map<string, ThreeAddressNodePtr> saved;
    vector<ThreeAddressNodePtr> copiedRandoms;
    set<ThreeAddressNodePtr> copiedKeyExpsAfterSimp;
    set<set<ThreeAddressNodePtr>> subset1;
    map<string, ThreeAddressNodePtr> localOriginalRandoms;

public:
    const set<ThreeAddressNodePtr> &getCopiedKeyExpressions() const;

    static void addElementHY(vector<ThreeAddressNodePtr>& res, set<ThreeAddressNodePtr >& elementHy) {
        for(auto ele : elementHy) {
            res.push_back(ele);
        }
    }
    void addToCopiedProof(set<ThreeAddressNodePtr>& nodeSet) {
        copiedProof.insert(nodeSet);
    }

    vector<ThreeAddressNodePtr> getInitTodo() {
        return todo;
    }



private:
    struct ptrComp
    {
        bool operator() (const ThreeAddressNodePtr& ptr1, const ThreeAddressNodePtr& ptr2) const {
            return ptr1->getNodeName() < ptr2->getNodeName();
        }
    };
    set<ThreeAddressNodePtr> copiedKeyExpsSet;

    vector<ThreeAddressNodePtr> todo;

    set<ThreeAddressNodePtr> originalValue;
    set<ThreeAddressNodePtr> orginalAuxY;
    set<ThreeAddressNodePtr> copiedAuxY;
    set<ThreeAddressNodePtr> copiedValue;

    set<ThreeAddressNodePtr> inputShares;

    vector<set<ThreeAddressNodePtr>> copiedYset;
    map<ThreeAddressNodePtr, set<ThreeAddressNodePtr>> copiedAuxYset;
    set<ThreeAddressNodePtr, ptrComp> copiedYIntermediate;
    map<ThreeAddressNodePtr, ThreeAddressNodePtr> Ymap;

    vector<DomExpressionPair> domExpressions;
    set<set<ThreeAddressNodePtr>> copiedProof;

    int order;
public:
    const vector<DomExpressionPair> &getDomExpressions() const;

    const vector<set<ThreeAddressNodePtr>> &getOriginalY() const;
    int getOrder() {
        return order;
    }
private:

    vector<set<ThreeAddressNodePtr>> originalY;
    map<set<ThreeAddressNodePtr>, ThreeAddressNodePtr> originalYsetToInternal;
    map<ThreeAddressNodePtr, set<ThreeAddressNodePtr>> originalinternalToY;


private:

    vector<ThreeAddressNodePtr> copiedParameters;

    vector<set<ThreeAddressNodePtr>> formalParas;

    static ThreeAddressNodePtr top;


    ThreeAddressNodePtr addExprToState(ThreeAddressNodePtr node) {

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
            copiedKeyExpsSet.insert(newNode);
        } else if(node->getNodeType() == NodeType::PRIVATE) {
            newNode = make_shared<ThreeAddressNode>(node);
            copiedKeyExpsSet.insert(newNode);
        } else if(node->getNodeType() == NodeType::INTERNAL) {
            ThreeAddressNodePtr lhs = addExprToState(node->getLhs());

            ThreeAddressNodePtr rhs = nullptr;
            if(node->getRhs()) {
                rhs = addExprToState(node->getRhs());
            }

            newNode = make_shared<ThreeAddressNode>(node);
            newNode->setLhs(lhs);
            newNode->setRhs(rhs);

            if(newNode->getRhs()) {
                if(copiedKeyExpsSet.count(newNode->getLhs()) != 0 && copiedKeyExpsSet.count(newNode->getRhs()) != 0) {
                    copiedKeyExpsSet.insert(newNode);
                }

            } else {
                if(copiedKeyExpsSet.count(newNode->getLhs()) != 0) {
                    copiedKeyExpsSet.insert(newNode);
                }

            }

            if(newNode->getOp() == ASTNode::Operator::CALL) {
                copiedKeyExpsSet.insert(newNode);
            }
        } else {
            assert(false);
        }

        setParents(newNode);
        saved[node->getNodeName()] = newNode;

    }

    ThreeAddressNodePtr addExprToState(ThreeAddressNodePtr node, const vector<set<ThreeAddressNodePtr>>& Y) {
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
            copiedKeyExpsSet.insert(newNode);
            copiedParameters.push_back(newNode);
        } else if(node->getNodeType() == NodeType::PRIVATE) {
            newNode = make_shared<ThreeAddressNode>(node);
            copiedKeyExpsSet.insert(newNode);
        }
        else if(node->getNodeType() == NodeType::INTERNAL) {
            ThreeAddressNodePtr lhs = addExprToState(node->getLhs(), Y);

            ThreeAddressNodePtr rhs = nullptr;
            if(node->getRhs()) {
                rhs = addExprToState(node->getRhs(), Y);
            }

            newNode = make_shared<ThreeAddressNode>(node);
            newNode->setLhs(lhs);
            newNode->setRhs(rhs);

            if(newNode->getRhs()) {
                if(copiedKeyExpsSet.count(newNode->getLhs()) != 0 && copiedKeyExpsSet.count(newNode->getRhs()) != 0) {
                    copiedKeyExpsSet.insert(newNode);
                }

            } else {
                if(copiedKeyExpsSet.count(newNode->getLhs()) != 0) {
                    copiedKeyExpsSet.insert(newNode);
                }

            }

            if(newNode->getOp() == ASTNode::Operator::CALL) {
                copiedKeyExpsSet.insert(newNode);
            }


        }
        else if(node->getNodeType() == NodeType::FUNCTION) {
            newNode = make_shared<ThreeAddressNode>(node);
        } else {
            assert(false);
        }
        setParents(newNode);
        saved[node->getNodeName()] = newNode;

        if(originalinternalToY.count(node) != 0) {
            set<ThreeAddressNodePtr> tempSet;
            for(auto ele : originalinternalToY[node]) {
                assert(saved.count(ele->getNodeName()) != 0);
                tempSet.insert(saved[ele->getNodeName()]);
            }
            copiedYset.push_back(tempSet);
            copiedAuxYset[newNode] = tempSet;
            copiedAuxY.insert(newNode);
            copiedYIntermediate.insert(newNode);
            Ymap[newNode] = node;
        }

        return newNode;
    }


    ThreeAddressNodePtr addExprToStateMain(ThreeAddressNodePtr node, const vector<set<ThreeAddressNodePtr>>& Y, const set<ThreeAddressNodePtr>& keys) {
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
            if(node->getNodeName().find("plain") != node->getNodeName().npos) {

            }else if(keys.empty() || keys.count(node) != 0)
                copiedKeyExpsSet.insert(newNode);

        } else if(node->getNodeType() == NodeType::PRIVATE) {
            newNode = make_shared<ThreeAddressNode>(node);
            if(keys.empty() || keys.count(node) != 0)
                copiedKeyExpsSet.insert(newNode);
        }
        else if(node->getNodeType() == NodeType::INTERNAL) {
            ThreeAddressNodePtr lhs = addExprToStateMain(node->getLhs(), Y, keys);

            ThreeAddressNodePtr rhs = nullptr;
            if(node->getRhs()) {
                rhs = addExprToStateMain(node->getRhs(), Y, keys);
            }

            newNode = make_shared<ThreeAddressNode>(node);
            newNode->setLhs(lhs);
            newNode->setRhs(rhs);

            if(newNode->getRhs()) {
                if(copiedKeyExpsSet.count(newNode->getLhs()) != 0 && copiedKeyExpsSet.count(newNode->getRhs()) != 0) {
                    copiedKeyExpsSet.insert(newNode);
                }

            } else {
                if(copiedKeyExpsSet.count(newNode->getLhs()) != 0) {
                    copiedKeyExpsSet.insert(newNode);
                }

            }

            if(newNode->getOp() == ASTNode::Operator::CALL) {
                if(keys.empty() || keys.count(node) != 0)
                    copiedKeyExpsSet.insert(newNode);
            }


        }
        else if(node->getNodeType() == NodeType::FUNCTION) {
            newNode = make_shared<ThreeAddressNode>(node);
        } else {
            assert(false);
        }
        setParents(newNode);
        saved[node->getNodeName()] = newNode;

        for(set<ThreeAddressNodePtr> nodeSet : Y) {
            if(originalYsetToInternal.count(nodeSet) != 0 && originalYsetToInternal[nodeSet] == node) {
                set<ThreeAddressNodePtr> tempSet;
                for(auto ele : nodeSet) {
                    assert(saved.count(ele->getNodeName()) != 0);
                    tempSet.insert(saved[ele->getNodeName()]);
                }
                copiedYset.push_back(tempSet);
                copiedAuxYset[newNode] = tempSet;
                copiedAuxY.insert(newNode);
                copiedYIntermediate.insert(newNode);
                Ymap[newNode] = node;
                break;
            }
        }
        return newNode;
    }


public:

    StateH(const set<ThreeAddressNodePtr>& observableSet, const vector<set<ThreeAddressNodePtr>>& Y, map<set<ThreeAddressNodePtr>, ThreeAddressNodePtr>& YsetToInternal,
           map<ThreeAddressNodePtr, set<ThreeAddressNodePtr>>& internalToY, int order) {
        originalValue = observableSet;
        this->order = order;
        originalY = Y;
        originalYsetToInternal = YsetToInternal;
        originalinternalToY = internalToY;
        for(auto ele : internalToY) {
            orginalAuxY.insert(ele.first);
        }

        setTop(observableSet, Y);
    }

    StateH(const set<ThreeAddressNodePtr>& observableSet, int order, const set<ThreeAddressNodePtr>& keys) {
        originalValue = observableSet;
        this->order = order;

        vector<set<ThreeAddressNodePtr>> Yset;
        setTopMain(observableSet, Yset, keys);
    }

    ThreeAddressNodePtr getSaved(ThreeAddressNodePtr key) {
        if(saved.count(key->getNodeName()) == 0)
            return nullptr;
        return saved[key->getNodeName()];
    }

    void setParents(ThreeAddressNodePtr node) {
        if(node->getLhs() != nullptr)
            node->getLhs()->addParents(node);

        if(node->getRhs() != nullptr)
            node->getRhs()->addParents(node);
    }

    void obtainKeyExpressions(const set<string>& Ynames) {
        copiedKeyExpsAfterSimp.clear();
        for(auto ele : copiedValue) {
            if(Ynames.count(ele->getNodeName()) == 0)
                simplifyKeyExpressinsAux(ele);
        }
    }

    void obtainKeyExpressionsMain() {
        copiedKeyExpsAfterSimp.clear();
        for(auto ele : copiedKeyExpsSet) {
            if(ele->getNodeType() != NodeType::INTERNAL) {
                copiedKeyExpsAfterSimp.insert(ele);
            }
        }
    }


    void simplifyKeyExpressinsAux(ThreeAddressNodePtr valuePtr) {
        if(copiedKeyExpsSet.count(valuePtr) != 0) {
            copiedKeyExpsAfterSimp.insert(valuePtr);
            return;
        }

        if(valuePtr->getNodeType() == NodeType::INTERNAL) {
            simplifyKeyExpressinsAux(valuePtr->getLhs());
            if (valuePtr->getRhs())
                simplifyKeyExpressinsAux(valuePtr->getRhs());
        } else {
            return;
        }

    }

    void initTodo() {
        for(auto random : copiedRandoms) {
            if(random->getParents().size() == 1) {
                todo.push_back(random);
            }
        }
    }

    bool canSimulated(int limit) {

        set<set<ThreeAddressNodePtr>> Yprime;
        bool inY = true;
        for(auto ele : copiedKeyExpsAfterSimp) {
            bool inYp = true;
            traverseExp(ele, Yprime, inYp);
            inY = inYp & inY;
            if(!inY)
                break;
        }
        if(Yprime.size() <= limit && inY) {
            for(auto ele : Yprime) {
                copiedProof.insert(ele);
            }
            return true;
        } else {
            return false;
        }
    }

    int numberOfShares(const vector<vector<ThreeAddressNodePtr>>& Paras, const set<string>& shares) {
        vector<set<string>> ParaNames;
        for(auto ele : Paras) {
            set<string> names;
            for(auto ele1 : ele) {
                names.insert(ele1->getNodeName());
            }
            ParaNames.push_back(names);
        }
        map<int, int> res;
        for(auto ele : shares) {
            for(int i = 0; i < ParaNames.size(); i++) {
                if(ParaNames[i].count(ele) != 0) {
                    if(res.count(i) == 0) {
                        res[i] = 1;
                    } else {
                        res[i]++;
                    }
                    break;
                }
            }
        }
        int max = (*res.begin()).second;
        for(auto ele : res) {
            if(ele.second > max)
                max = ele.second;
        }
        return max;
    }

    set<set<ThreeAddressNodePtr>> getCopiedProof() {
        return copiedProof;
    }

    bool canSimulatedMain(int limit, CheckType checkType, const vector<vector<ThreeAddressNodePtr>>& Paras) {
        if(checkType == CheckType::Probing) {
            if(getKeyExpressions().empty())
                return true;
            else {
                return false;
            }
        } else {
            set<ThreeAddressNodePtr> shares;
            set<string> sharesName;
            if(getKeyExpressions().empty())
                return true;
            for(auto ele : copiedKeyExpsAfterSimp) {
                if(ele->getNodeType() == NodeType::PARAMETER) {
                    shares.insert(ele);
                } else {
                    set<ThreeAddressNodePtr> temp;
                    getLeaves(ele, temp);
                    shares.insert(temp.begin(), temp.end());
                }
            }
            for(auto ele : shares) {
                sharesName.insert(ele->getNodeName());
            }
            if(numberOfShares(Paras, sharesName) > limit)
                return false;
            else {
                addToCopiedProof(shares);
                return true;
            }
        }
    }



    bool canSimulatedWithProof(const set<ThreeAddressNodePtr>& targetVector) {
        set<set<ThreeAddressNodePtr>> Yprime;

        bool inY = true;
        for(auto ele : targetVector) {
            bool inYp = true;
            traverseExp(ele, Yprime, inYp);
            inY = inYp & inY;
        }
        set<ThreeAddressNodePtr> Yprimeprime;
        for(auto ele : Yprime) {
            for(auto ele1 : ele) {
                Yprimeprime.insert(ele1);
            }
        }

        set<ThreeAddressNodePtr> copidProofAux;
        for(auto ele : copiedProof) {
            for(auto ele1 : ele)
                copidProofAux.insert(ele1);
        }
        if(std::includes(copidProofAux.begin(), copidProofAux.end(), Yprimeprime.begin(), Yprimeprime.end()) && inY) {
            return true;
        } else {
            return false;
        }
    }

    void canSimulatedWithProof2Aux(ThreeAddressNodePtr nodePtr, const set<ThreeAddressNodePtr>& proof, bool& flag) {

        if(!flag)
            return;
        if(!nodePtr)
            return;

        if(proof.count(nodePtr) == 0) {
            if(nodePtr->getNodeType() == NodeType::PARAMETER || nodePtr->getOp() == ASTNode::Operator::CALL) {
                flag = false;
                return;
            } else {
                canSimulatedWithProof2Aux(nodePtr->getLhs(), proof, flag);
                canSimulatedWithProof2Aux(nodePtr->getRhs(), proof, flag);
            }
        }
    }

    void traverseExp(ThreeAddressNodePtr exp, set<set<ThreeAddressNodePtr>>& Yprime, bool& inY) {
        if(!exp)
            return;
        if(!inY) {
            return;
        }
        for(auto ele : Yprime) {
            if(ele.count(exp) != 0)
                return;
        }
        for(auto ele : copiedProof) {
            if(ele.count(exp) != 0) {
                Yprime.insert(ele);
                return;
            }
        }
        if(copiedYIntermediate.count(exp) != 0) {
            assert(copiedAuxYset.count(exp) != 0);
            Yprime.insert(copiedAuxYset[exp]);
            return;
        }
        if(exp->getNodeType() == NodeType::PARAMETER) {
//            assert(false);
            inY = false;
            return;
        }
        if(exp->getNodeType() == NodeType::FUNCTION) {
            inY = false;
            return;
        }

        traverseExp(exp->getLhs(), Yprime, inY);
        if(exp->getRhs())
            traverseExp(exp->getRhs(), Yprime, inY);
    }



    bool isDominateOperation(ASTNode::Operator op) {
        if(op == ASTNode::Operator::XOR ||
           op == ASTNode::Operator::ADD ||
           op == ASTNode::Operator::MINUS ||
           op == ASTNode::Operator::AFFINE ||
                op == ASTNode::Operator::POW2 ||
                op == ASTNode::Operator::POW4 ||
                op == ASTNode::Operator::POW16 ||
                op == ASTNode::Operator::XTIMES ||
           op == ASTNode::Operator::NOT) {
            return true;
        } else
            return false;
    }

    void removeAnotherChild(ThreeAddressNodePtr random, ThreeAddressNodePtr parent) {
        if(parent->getLhs() == random) {
            removeChild(parent->getRhs(), parent);
        } else if(parent->getRhs() == random)
            removeChild(parent->getLhs(), parent);
    }

    void removeNode(ThreeAddressNodePtr node) {
        if(saved.count(node->getNodeName()) == 0)
            return;
        assert(saved.count(node->getNodeName()) > 0);
        assert((node->getParents()).size() == 0);

        if(node == nullptr)
            return;

        saved.erase(node->getNodeName());
        copiedKeyExpsAfterSimp.erase(node);
        copiedValue.erase(node);

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
            if(copiedKeyExpsSet.count(node) != 0)
                copiedKeyExpsSet.erase(node);
            for(auto ele : copiedYset) {
                ele.erase(node);
            }
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

    void removeChild(ThreeAddressNodePtr child, ThreeAddressNodePtr parent) {
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
        else if(child->getParents().size() == 1 && child->getNodeType() == NodeType::RANDOM)
            todo.push_back(child);
    }

    bool isRndForDominate(ThreeAddressNodePtr random) {
        if(random->getParents().size() == 1 && random->getNodeType() == NodeType::RANDOM) {
            if(random->getParents()[0].expired()){
                cout << "fuck" << endl;
                return false;
            }
            ThreeAddressNodePtr parent = random->getParents()[0].lock();
            return isDominateOperation(parent->getOp());

        }

        return false;
    }


    vector<ThreeAddressNodePtr> test() {
        vector<ThreeAddressNodePtr> res;
        for(auto ele : copiedKeyExpsAfterSimp) {
            set<ThreeAddressNodePtr> temp;
            temp.insert(ele);
            if(canSimulatedWithProof(temp))
                res.push_back(ele);
        }
        return res;
    }

    bool canSimulatedUsingSet1(int limit) {
        obtainInputShares();
        auto res = tryUsingSets(limit);
        if(!res.empty()) {
            return true;
        }
        return false;

    }

    set<ThreeAddressNodePtr> canSimulatedUsingSetFO() {
        obtainInputShares();
        auto res = tryUsingSets(1);
        return res;
    }

    void applySimplification(ThreeAddressNodePtr random) {

        ThreeAddressNodePtr parent = random->getParents()[0].lock();
        random->getParents().clear();

        domExpressions.push_back(DomExpressionPair(random, parent));

        removeAnotherChild(random, parent);

        copyNode(parent, random);

        copiedRandoms.push_back(parent);

        auto new_end = std::remove_if(copiedRandoms.begin(), copiedRandoms.end(),
                                      [random](const ThreeAddressNodePtr& randomA)
                                      { return random == randomA; });
        copiedRandoms.erase(new_end, copiedRandoms.end());

        if(isRndForDominate(parent))
            todo.push_back(parent);
    }

    void copyNode(ThreeAddressNodePtr des, ThreeAddressNodePtr sou) {
        des->setNodeName(sou->getNodeName());

        des->setNodeType(sou->getNodeType());
        des->setOp(sou->getOp());
        des->setLhs(sou->getLhs());
        des->setRhs(sou->getRhs());
    }


    void dominateSimplification() {
        assert(!todo.empty());
        ThreeAddressNodePtr random = todo.back();
        todo.pop_back();

        if (random->getParents().size() == 1) {
            ThreeAddressNodePtr parent = random->getParents()[0].lock();
            if (parent && isDominateOperation(parent->getOp())) {
                applySimplification(random);
            }
        }
    }


    bool canSimplyUsingDomMain(int limit, const vector<vector<ThreeAddressNodePtr >>& Paras, CheckType checkType) {
        initTodo();
        if(canSimulatedMain(limit, checkType, Paras))
            return true;
        else {
            while (!canSimulatedMain(limit, checkType, Paras)) {
                if (todo.size() > 0) {
                    dominateSimplification();
                } else {
                    return false;
                }
            }
        }
        return true;
    }

    bool canSimplyUsingSpecificDomMain(string name) {
        initTodo();
        vector<ThreeAddressNodePtr> newTodo;
        for(auto ele : todo) {
            if(ele->getNodeName() == name)
                newTodo.push_back(ele);
        }
        todo = newTodo;
        if((*(copiedValue.begin()))->getNodeType() == NodeType::RANDOM)
            return true;
        else {
            while ((*(copiedValue.begin()))->getNodeType() != NodeType::RANDOM) {
                if (todo.size() > 0) {
                    dominateSimplification();
                } else {
                    return false;
                }
            }
        }
        return true;
    }

    bool canSimplyUsingDom(int limit) {
        initTodo();
        if(canSimulated(limit))
            return true;
        else {
            while (!canSimulated(limit)) {
                if (todo.size() > 0) {
                    dominateSimplification();
                } else {
                    return false;
                }
            }
        }
        return true;
    }


    void removeUsedKeyExpressions() {
        while(!copiedKeyExpsAfterSimp.empty()) {
            auto first = *copiedKeyExpsAfterSimp.begin();
            removeAllNodeAndParent(first);
            copiedKeyExpsAfterSimp.erase(first);
        }
    }

    void removeUsedKeyExpressions1(const set<ThreeAddressNodePtr>& oldset, int remaind) {
        while(copiedKeyExpsAfterSimp.size() > remaind) {
            auto first = *copiedKeyExpsAfterSimp.begin();
            removeAllNodeAndParentExcept(first, oldset);
            copiedKeyExpsAfterSimp.erase(first);
        }


    }


    vector<ThreeAddressNodePtr> getElementInProof() {
        vector<ThreeAddressNodePtr> res;
        for(auto ele : copiedProof) {
                for(auto ele1 : ele) {
                    res.push_back(ele1);
                }
        }
        return res;
    }

    void trimKeyExpressions(const set<ThreeAddressNodePtr>& old) {
        set<ThreeAddressNodePtr> keySet(old.begin(), old.end());
        set<ThreeAddressNodePtr> keyExpressionsSet(copiedKeyExpsAfterSimp.begin(), copiedKeyExpsAfterSimp.end());
        for(auto ele : copiedKeyExpsAfterSimp) {
            if(keySet.count(ele) != 0) {
                keyExpressionsSet.erase(ele);
            }
        }
        this->copiedKeyExpsAfterSimp = keyExpressionsSet;
    }

    bool hasTop(ThreeAddressNodePtr node) {
        ThreeAddressNodePtr copiedNode = nullptr;
        if(saved.count(node->getNodeName()) == 0) {
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


    void getShares(ThreeAddressNodePtr node) {
        if(!node)
            return;
        if(node->getNodeType() == NodeType::PARAMETER) {
            inputShares.insert(node);
        } else if(node->getOp() == ASTNode::Operator::CALL) {
            inputShares.insert(node);
        }
        getShares(node->getLhs());
        getShares(node->getRhs());
    }


    void obtainInputShares() {
        inputShares.clear();
        for(auto ele : copiedKeyExpsAfterSimp) {
            getShares(ele);
        }
    }

    void getLeaves(ThreeAddressNodePtr ele, set<ThreeAddressNodePtr>& leaves) {
        if(!ele)
            return;
        if(ele->getNodeType() == NodeType::PARAMETER) {
            leaves.insert(ele);
            return;
        }
        getLeaves(ele->getLhs(), leaves);
        if(ele->getRhs()) {
            getLeaves(ele->getRhs(), leaves);
        }
    }


    set<ThreeAddressNodePtr> tryUsingSets(int limit) {
        vector<set<ThreeAddressNodePtr>> existing;
        for(auto ele : originalYsetToInternal) {
            existing.push_back(ele.first);
        }
        if(existing.empty()) {
            existing = originalY;
        }

        vector<int> array;
        bool initSuccess = Utils::gen_comb_norep_lex_init(array, existing.size(), limit);
        set<ThreeAddressNodePtr> observableShares;
        for(auto i : array) {
            set<ThreeAddressNodePtr>& leaves = existing[i];
            for(auto ele : leaves) {
                if(saved.count(ele->getNodeName()) == 0) {
                    addExprToState(ele, originalY);

                }
                assert(saved.count(ele->getNodeName()) != 0);
                observableShares.insert(saved[ele->getNodeName()]);

            }
        }
        if(includes(observableShares.begin(), observableShares.end(), inputShares.begin(), inputShares.end())) {
            for(auto ele : array) {
                set<ThreeAddressNodePtr> savedRes;
                for(auto ele1 : existing[ele]) {
                    assert(saved.count(ele1->getNodeName()) != 0);
                    savedRes.insert(saved[ele1->getNodeName()]);
                }
                copiedProof.insert(savedRes);
            }
            return existing[array[0]];
        } else {
            if(initSuccess) {
                while(Utils::gen_comb_norep_lex_next(array, existing.size(), limit)) {
                    observableShares.clear();
                    for (auto i : array) {
                        set<ThreeAddressNodePtr>& leaves = existing[i];
                        for(auto ele : leaves) {
                            if(saved.count(ele->getNodeName()) == 0) {
                                addExprToState(ele, originalY);

                            }
                            assert(saved.count(ele->getNodeName()) != 0);
                            observableShares.insert(saved[ele->getNodeName()]);

                        }
                    }
                    if(includes(observableShares.begin(), observableShares.end(), inputShares.begin(), inputShares.end())){
                        for(auto ele : array) {
                            set<ThreeAddressNodePtr> savedRes;
                            for(auto ele1 : existing[ele]) {
                                assert(saved.count(ele1->getNodeName()) != 0);
                                savedRes.insert(saved[ele1->getNodeName()]);
                            }
                            copiedProof.insert(savedRes);
                        }
                        return existing[array[0]];;
                    }
                }
            } else {
                return set<ThreeAddressNodePtr>();
            }
        }
        return set<ThreeAddressNodePtr>();
    }

    bool tryUseExistingSets(int limit) {
        vector<set<ThreeAddressNodePtr>> existing;
        vector<set<ThreeAddressNodePtr>> copiedExistingSet;
        for(auto ele : originalinternalToY) {
            set<ThreeAddressNodePtr> temp;
            for(auto ele1 : ele.second) {
                if(saved.count(ele1->getNodeName()) == 0) {
                    addExprToState(ele1, originalY);
                }
                assert(saved.count(ele1->getNodeName()) != 0);
                temp.insert(saved[ele1->getNodeName()]);
            }
            existing.push_back(temp);
            bool copidAuxYsethas = false;
            for(auto ele1 : copiedAuxYset) {
                if(ele1.first->getNodeName() == ele.first->getNodeName()) {
                    copiedExistingSet.push_back(ele1.second);
                    copidAuxYsethas = true;
                }
            }
            if(!copidAuxYsethas) {
                set<ThreeAddressNodePtr> second;
                for(auto ele1 : ele.second) {
                    second.insert(addExprToState(ele1, originalY));
                }
                copiedExistingSet.push_back(second);
            }
        }
        assert(existing.size() == copiedExistingSet.size());
        if(existing.size() == 0)
            return false;

        vector<int> array;
        bool initSuccess = Utils::gen_comb_norep_lex_init(array, existing.size(), limit);
        set<ThreeAddressNodePtr> observableShares;
        for(auto i : array) {
            set<ThreeAddressNodePtr>& leaves = existing[i];
            observableShares.insert(leaves.begin(), leaves.end());
        }
        if(includes(observableShares.begin(), observableShares.end(), inputShares.begin(), inputShares.end())) {
            for(auto ele : array) {
                copiedProof.insert(copiedExistingSet[ele]);
            }
            return true;
        } else {
            if(initSuccess) {
                while(Utils::gen_comb_norep_lex_next(array, existing.size(), limit)) {
                    observableShares.clear();
                    for (auto i : array) {
                        set<ThreeAddressNodePtr>& leaves = existing[i];
                        observableShares.insert(leaves.begin(), leaves.end());
                    }
                    if(includes(observableShares.begin(), observableShares.end(), inputShares.begin(), inputShares.end())){
                        for(auto ele : array) {
                            copiedProof.insert(copiedExistingSet[ele]);
                        }
                        return true;
                    }
                }
            } else {
                return false;
            }
        }

        return false;
    }

    bool findFromCopiedKeyExpressionsOrInput(set<ThreeAddressNodePtr>& base1, int limit) {
        map<ThreeAddressNodePtr , set<ThreeAddressNodePtr>> internalToLeaves;
        vector<ThreeAddressNodePtr> base(base1.begin(), base1.end());

        for(auto ele : base) {
            set<ThreeAddressNodePtr> leaves;
            getLeaves(ele, leaves);
            internalToLeaves[ele] = leaves;
        }

        vector<int> array;
        bool initSuccess = Utils::gen_comb_norep_lex_init(array, base.size(), limit);
        set<ThreeAddressNodePtr> observableShares;
        string test = "try {";
        for(auto i : array) {
            set<ThreeAddressNodePtr>& leaves = internalToLeaves[base[i]];
            observableShares.insert(leaves.begin(), leaves.end());
            test += to_string(i) + base[i]->getNodeName() + ",";
        }
        test += "}";
        cout << test << endl;
        if(includes(observableShares.begin(), observableShares.end(), inputShares.begin(), inputShares.end())) {
            return true;
        } else {
            if(initSuccess) {
                while(Utils::gen_comb_norep_lex_next(array, base.size(), limit)) {
                    set<ThreeAddressNodePtr> observableShares;
                    string test = "try {";
                    for (auto i : array) {
                        set<ThreeAddressNodePtr>& leaves = internalToLeaves[base[i]];
                        observableShares.insert(leaves.begin(), leaves.end());
                        test += to_string(i) + ",";
                    }
                    test += "}";
                    cout << test << endl;
                    if(includes(observableShares.begin(), observableShares.end(), inputShares.begin(), inputShares.end())) {
                        for(auto i : array) {
                            for(auto& ele : copiedYset) {
//                                if(base[i] == ele.first) {
//                                    ele.second = internalToLeaves[base[i]];
//                                    copiedProof.insert(ele);
//                                }
                            }
                        }
                        return true;
                    }
                }
            } else {
                assert(false);
            }
        }
        return false;

    }


    const set<ThreeAddressNodePtr>& getKeyExpressions() {
        return copiedKeyExpsAfterSimp;
    }

    static void getAuxiliaryTable(ThreeAddressNodePtr node) {
        // leaf node
        if (node->getLhs() == nullptr && node->getRhs() == nullptr) {
            if (node->getNodeType() == NodeType::RANDOM) {
                node->insertSupportV(node);
                node->insertUniqueM(node);
                node->insertPerfectM(node);
                node->insertDominant(node);
                return;
            }
            else {
                node->insertSupportV(node);
                return;
            }
        }
            // internal node
        else {
            if (node->getLhs() != nullptr && node->getLhs()->getSupportVSize() == 0) {
                getAuxiliaryTable(node->getLhs());
            }

            if (node->getRhs() != nullptr && node->getRhs()->getSupportVSize() == 0) {
                getAuxiliaryTable(node->getRhs());
            }

            if (node->getRhs() == nullptr) {
                node->setSupportV(node->getLhs()->getSupportV());
                node->setUniqueM(node->getLhs()->getUniqueM());
                node->setPerfectM(node->getLhs()->getPerfectM());
                node->setDominant(node->getLhs()->getDominant());
            }
            else {
                Utils::setUnion(node->getLhs()->getSupportV(), node->getRhs()->getSupportV(), node->getSupportV());

                set<ThreeAddressNodePtr> temp1;
                set<ThreeAddressNodePtr> temp2;

                Utils::setUnion(node->getLhs()->getUniqueM(), node->getRhs()->getUniqueM(), temp1);
                Utils::setIntersection(node->getLhs()->getSupportV(), node->getRhs()->getSupportV(), temp2);
                Utils::setDifference(temp1, temp2, node->getUniqueM());

                if (node->getOp() == ASTNode::Operator::XOR || node->getOp() == ASTNode::Operator ::ADD
                    || node->getOp() == ASTNode::Operator::MINUS ||
                    (node->getOp() == ASTNode::Operator::MUL && (node->getLhs()->getNodeType() == NodeType::CONSTANT||
                                                                 node->getRhs()->getNodeType() == NodeType::CONSTANT))) {
                    set<ThreeAddressNodePtr> temp3;
                    Utils::setUnion(node->getLhs()->getPerfectM(), node->getRhs()->getPerfectM(), temp3);
                    Utils::setIntersection(node->getUniqueM(), temp3, node->getPerfectM());

                    set<ThreeAddressNodePtr> temp4;
                    Utils::setUnion(node->getLhs()->getDominant(), node->getRhs()->getDominant(), temp4);
                    Utils::setIntersection(temp4, node->getUniqueM(), node->getDominant());
                } else {
                    node->getDominant().clear();
                }
            }

        }
    }

    void setTop(set<ThreeAddressNodePtr> subset, const vector<set<ThreeAddressNodePtr>>& Yset) {
        for(auto ins : subset) {
            originalValue.insert(ins);
            ThreeAddressNodePtr node = addExprToState(ins, Yset);
            node->addParents(StateH::top);
            copiedValue.insert(node);
        }
    }

    void setTopMain(set<ThreeAddressNodePtr> subset, const vector<set<ThreeAddressNodePtr>>& Yset, const set<ThreeAddressNodePtr>& keys) {
        for(auto ins : subset) {
            originalValue.insert(ins);
            ThreeAddressNodePtr node = addExprToStateMain(ins, Yset, keys);
            node->addParents(StateH::top);
            copiedValue.insert(node);
        }
    }




    bool directlyInclude(int limit);

    void removeAllNodeAndParent(ThreeAddressNodeWeakPtr n1) {
        if(n1.expired())
            return;
        ThreeAddressNodePtr n = n1.lock();
        if(n != top) {
            while(!(n->getParents().size() == 0  || (n->getParents().size() == 1 && n->getParents().back().lock() == top))) {
                ThreeAddressNodeWeakPtr par = n->getParents().back();
                n->getParents().pop_back();
                removeAllNodeAndParent(par);
            }
            n->getParents().clear();
            removeNode(n);
        }
    }

    void removeAllNodeAndParentExcept(ThreeAddressNodeWeakPtr n1, const set<ThreeAddressNodePtr>& oldSet) {
        if(n1.expired())
            return;
        ThreeAddressNodePtr n = n1.lock();
        if(n != top) {
            vector<ThreeAddressNodeWeakPtr > tempSaved;
            while(!(n->getParents().size() == 0 ) || (n->getParents().size() == 1 && n->getParents().back().lock() == top)) {
                ThreeAddressNodeWeakPtr par = n->getParents().back();
                n->getParents().pop_back();

                if(oldSet.count(par.lock()) != 0)
                    tempSaved.push_back(par);
                else if(par.lock() == top && oldSet.count(n) != 0)
                    tempSaved.push_back(top);
                else
                    removeAllNodeAndParentExcept(par, oldSet);
            }
            if(!tempSaved.empty()) {
                for(auto ele : tempSaved) n->getParents().push_back(ele);
            } else {
                n->getParents().clear();
                removeNode(n);
            }
        }
    }

    bool terminateFORConditionA() {
        if(copiedKeyExpsAfterSimp.size() <= order)
            return true;

        map<int, set<ThreeAddressNodePtr>> numberOfReturnsOfCalls;
        for(auto ele : copiedKeyExpsAfterSimp) {
            int index = ele->getIndexCall() / (order + 1);
            numberOfReturnsOfCalls[index].insert(ele);
            if(numberOfReturnsOfCalls[index].size() > order)
                return false;
        }
        return true;


    }

    bool terminateFORMain() {
        if(copiedKeyExpsSet.empty())
            return true;
        return false;
    }








    bool terminateFORConditionB() {
        for(auto ele : copiedKeyExpsAfterSimp) {
            set<ThreeAddressNodePtr> res;
            ThreeAddressNode::getSupport(ele, res);
            if(res.size() == 1 && (res.begin())->get()->getNodeType() == NodeType::RANDOM) {
                cout << res.begin()->get()->prettyPrint5() << endl;
                continue;
            } else if(res.size() > 1) {
                for(auto ele1 : res) {
                    if(ele1->getNodeType() == NodeType::RANDOM)
                        continue;
                    else if(ele1->getNodeType() == NodeType::CONSTANT)
                        continue;
                    else
                        return false;
                }
            }
            else {
                cout << res.begin()->get()->prettyPrint5() << endl;
                cout << res.end()->get()->prettyPrint5() << endl;
                return false;
            }
        }
        return true;
    }


};


#endif //SAFELANG_STATEH_H
