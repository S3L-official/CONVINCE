//
// Created by Pengfei Gao on 2022/3/14.
//

#ifndef COMAVE_STATENEW_H
#define COMAVE_STATENEW_H

#include "ThreeAddressNode.h"

enum CheckingType {ni, sni, prob, ydwsni, ydni, ydsni, ydpini};
typedef pair<ThreeAddressNodePtr, ThreeAddressNodePtr> DomExpressionPair;
class StateNew {


private:
    static ThreeAddressNodePtr top;
    map<string, ThreeAddressNodePtr> saved;
    set<ThreeAddressNodePtr> nodeHasTop;
    int order;

    vector<ThreeAddressNodePtr> copiedRandoms;
    vector<ThreeAddressNodePtr> copiedParameters;
    vector<ThreeAddressNodePtr> copiedPrivate;
    vector<ThreeAddressNodePtr> copiedInternalFromY;
    set<ThreeAddressNodePtr> copiedKeys;
public:
    void setCopiedKeys(const set<ThreeAddressNodePtr> &copiedKeys);

private:
    vector<ThreeAddressNodePtr> todo;
    vector<DomExpressionPair> domExpressions;
    map<ThreeAddressNodePtr, ThreeAddressNodePtr> replacedNode;
    vector<set<ThreeAddressNodePtr>> Y;
    set<ThreeAddressNodePtr> elementsInY;
    vector<set<string>> copiedYname;
    set<set<string>> copiedProofName;
    CheckingType checkingType;

public:
    StateNew(const set<ThreeAddressNodePtr>& observableSet, CheckingType type, const vector<set<ThreeAddressNodePtr>>& Y, int order):
                    checkingType(type), Y(Y), order(order) {

        for(auto ele : Y) {
            for(auto ele1 : ele) elementsInY.insert(ele1);
        }

        copiedYname = vector<set<string>>(Y.size(), set<string>());
        for(int i = 0; i < Y.size(); i++) {
            for(auto ele : Y[i]) {
                copiedYname[i].insert(ele->getNodeName());
            }
        }
        setTop(observableSet);
    }

    static void setParents(ThreeAddressNodePtr node) {
        if(node->getLhs() != nullptr)
            node->getLhs()->addParents(node);

        if(node->getRhs() != nullptr)
            node->getRhs()->addParents(node);
    }

    const vector<DomExpressionPair> &getDomExpressions() const;
    const set<ThreeAddressNodePtr> &getCopiedKeys() const;
    const set<set<string>> &getCopiedProofName() const;

    void setTop(const set<ThreeAddressNodePtr>& observableSet);
    ThreeAddressNodePtr addExprToState(ThreeAddressNodePtr node);
    void obtainKey();
    bool canSimulate();
    bool isDominateOperation(ASTNode::Operator op);
    bool domSimplifyUntilCanSimulate();
    void dominateSimplification();
    bool hasTop(ThreeAddressNodePtr node);
    bool canCoverSet();
    bool canCoverSetNew();
    void dealWithNewKeyExps(set<ThreeAddressNodePtr>&);
    void dealWithNewKeyExpsWSNI(set<ThreeAddressNodePtr>& newkeyExps);

    void removeNode(ThreeAddressNodePtr node);
    void removeChild(ThreeAddressNodePtr child, ThreeAddressNodePtr parent);
    void removeAllNodeAndParent(ThreeAddressNodeWeakPtr n1);
    void applySimplification(ThreeAddressNodePtr random);
    void replaceAbyB(ThreeAddressNodePtr parent, ThreeAddressNodePtr random);


    string printState(string title);

};


#endif //COMAVE_STATENEW_H
