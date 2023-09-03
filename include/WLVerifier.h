//
// Created by Pengfei Gao on 2022/2/26.
//

#ifndef COMAVE_WLVERIFIER_H
#define COMAVE_WLVERIFIER_H
#include "ThreeAddressNode.h"
#include "StateH.h"
#include "StateNew.h"


typedef pair<int, vector<set<ThreeAddressNodePtr>>> NumberSubsetPair;
typedef vector<NumberSubsetPair> WorkList;
typedef pair<vector<set<ThreeAddressNodePtr>>, vector<set<ThreeAddressNodePtr>>> Split;

class WLVerifier {
private:
    int order;
    WorkList initWorkList;
    set<ThreeAddressNodePtr> keys;
    vector<vector<ThreeAddressNodePtr>> parameters;
    CheckType checkType;
    set<set<ThreeAddressNodePtr>> fromY;
    vector<set<ThreeAddressNodePtr>> Y;
    set<set<ThreeAddressNodePtr>> setThatCanNotBeProved;
    map<ThreeAddressNodePtr, int> outputToShareIndice;
    map<string, int> intputToShareIndice;

public:
    void setIntputToShareIndice(const map<string, int> &intputToShareIndice);

public:
    void setOutputToShareIndice(const map<ThreeAddressNodePtr, int> &outputs);

private:
    int count;
public:
    int getCount() const;

public:
    WLVerifier(const WorkList& workList, const set<set<ThreeAddressNodePtr>>& fromY, int order, const vector<set<ThreeAddressNodePtr>>& Y) {
        this->order = order;
        this->initWorkList = workList;
        this->fromY = fromY;
        this->Y = Y;
        this->count = 0;
    }

    const set<set<ThreeAddressNodePtr>> &getSetThatCanNotBeProved() const;
    void beginExpore();
    void beginExpore2(CheckingType checkingType);
    bool explore(WorkList workList1);
    bool explore2(WorkList workList1, CheckingType checkingType);
    bool explore2B(WorkList workList1, CheckingType checkingType);
    bool explore2B(WorkList workList, const set<set<ThreeAddressNodePtr>>& Y, CheckingType checkingType);
    bool exploreAux(WorkList& workList, vector<int>& enumration, WorkList& newWorklist, WorkList& temp, vector<Split>& splits);
    bool exploreAux2(WorkList& workList, vector<int>& enumration, WorkList& newWorklist, WorkList& temp, vector<Split>& splits, CheckingType checkingType);
    bool exploreAux2B(WorkList workList, WorkList accu, vector<Split> splits, CheckingType checkingType);
    bool check(StateH& state);
    bool check2(StateNew& state);
    bool check2ForWSNI(StateNew& stateNew, const set<ThreeAddressNodePtr>& subset1);
    void extend(StateH& state, const WorkList& workList);
    void extend2(StateNew& state, const WorkList& workList, CheckingType);
    set<set<ThreeAddressNodePtr>> choose(const WorkList& workList);

    static void printWorklist(WorkList& worklist);


};


#endif //COMAVE_WLVERIFIER_H
