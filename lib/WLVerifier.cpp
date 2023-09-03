//
// Created by Pengfei Gao on 2022/2/26.
//

#include "WLVerifier.h"


void WLVerifier::beginExpore() {
    WorkList workList;
    explore(workList);
}

void WLVerifier::beginExpore2(CheckingType checkingType) {
    explore2B(initWorkList, checkingType);
}

set<set<ThreeAddressNodePtr>> WLVerifier::choose(const WorkList& workList) {
    set<set<ThreeAddressNodePtr>> result;
    for(auto workListElement : workList) {
        for(int i = 0; i < workListElement.first; i++) {
            result.insert(workListElement.second[i]);
        }
    }
    return result;
}

bool WLVerifier::check(StateH& stateH) {
    // TODO: stateH should be refactored
    stateH.obtainKeyExpressionsMain();
    if(stateH.canSimulatedMain(order, checkType, parameters)) {
        return true;
    }

    if(stateH.canSimplyUsingDomMain(order, parameters, checkType)) {
        return true;
    }

    return false;
}

bool WLVerifier::check2(StateNew& stateNew) {
    stateNew.obtainKey();

    if(stateNew.domSimplifyUntilCanSimulate())
        return true;

    return false;
}

bool WLVerifier::check2ForWSNI(StateNew& stateNew, const set<ThreeAddressNodePtr>& subset1) {
    bool res = check2(stateNew);

    set<ThreeAddressNodePtr> subset1WithoutOutput;
    for(auto ele : subset1) {
        if(outputToShareIndice.count(ele) == 0) {
            subset1WithoutOutput.insert(ele);
        }
    }
    StateNew newStateWithoutOutput(subset1WithoutOutput, CheckingType::ydwsni, Y, order);
    newStateWithoutOutput.obtainKey();
    auto copiedKeysWithoutOutput = newStateWithoutOutput.getCopiedKeys();
    set<string> names;
    for(auto ele : copiedKeysWithoutOutput) names.insert(ele->getNodeName());
    //

    StateNew newStatePrime(subset1, CheckingType::ydwsni, Y, order);
    newStatePrime.obtainKey();
    auto copiedKeysTotal = newStatePrime.getCopiedKeys();
    set<ThreeAddressNodePtr> newKeys;
    for(auto ele : copiedKeysTotal) {
        if(names.count(ele->getNodeName()) != 0) newKeys.insert(ele);
    }
    newStatePrime.setCopiedKeys(newKeys);

    this->order = subset1WithoutOutput.size();
    res = check2(newStatePrime) & res;
    this->order = subset1.size();



    return res;

}

void WLVerifier::extend(StateH &state, const WorkList& workList) {
    set<ThreeAddressNodePtr> oldKeyExpressions = state.getKeyExpressions();
    for(auto subset: workList) {
        for(auto ele : subset.second) {
            state.setTop(ele, state.getOriginalY());
        }
    }

    for(auto domToExpression : state.getDomExpressions()) {
        state.removeAllNodeAndParent(domToExpression.first);
    }


    state.obtainKeyExpressionsMain();

    if(!state.canSimplyUsingDomMain(order, parameters, CheckType::NI)) {
        set<ThreeAddressNodePtr> newKeyExps = state.getKeyExpressions();
        while (!newKeyExps.empty()) {
            auto first = *newKeyExps.begin();
            if(!state.getCopiedProof().empty() && (*state.getCopiedProof().begin()).count(first) == 0)
                state.removeAllNodeAndParent(first);
            newKeyExps.erase(first);
        }
    }
}

void WLVerifier::extend2(StateNew& state, const WorkList& workList, CheckingType checkingType) {
    set<ThreeAddressNodePtr> oldKeyExpressions = state.getCopiedKeys();
    for(auto subset: workList) {
        for(auto ele : subset.second) {
            state.setTop(ele);
        }
    }


    for(auto domToExpression : state.getDomExpressions()) {
        state.removeAllNodeAndParent(domToExpression.first);
    }


    state.obtainKey();

    auto oldProof = state.getCopiedProofName();
    if(!state.domSimplifyUntilCanSimulate()) {
        set<ThreeAddressNodePtr> newKeyExps = state.getCopiedKeys();

        if(checkingType == CheckingType::ydwsni) {
            state.dealWithNewKeyExpsWSNI(newKeyExps);
        } else {
            state.dealWithNewKeyExps(newKeyExps);
        }
        while (!newKeyExps.empty()) {
            auto first = *newKeyExps.begin();
            state.removeAllNodeAndParent(first);
            newKeyExps.erase(first);
        }
    }
}


bool WLVerifier::explore(WorkList workList) {
    for(auto ele : workList) {
        if(ele.first > ele.second.size()) {
            return true;
        }
    }

    set<set<ThreeAddressNodePtr>> subset = choose(workList);

    set<ThreeAddressNodePtr> subset1;
    for(auto ele : subset) {
        subset1.insert(ele.begin(), ele.end());
    }

    set<set<ThreeAddressNodePtr>> subsetFromYb;
    set<string> namesFromYb;
    if(!fromY.empty()) {
        for(auto ele : subset) {
            if(fromY.count(ele) != 0) {
                subsetFromYb.insert(ele);
                for(auto ele1 : ele) {
                    namesFromYb.insert(ele1->getNodeName());
                }
            }
        }
    }


    cout << "==========================\n";
    for(auto ele : subset1) {
        cout << ele->getNodeName() << endl;
    }
    cout << "==========================\n";


    vector<Split> splits;

    StateH newState(subset1, order, keys);
    // check whether safe
    bool res = check(newState);

    if (res == true) {
        extend(newState, workList);
    }

    else if (checkType == CheckType::SNI) {
        throw subset1;
    } else {
        throw subset1;
    }


    WorkList temp;
    for(auto it = workList.begin(); it != workList.end(); ) {
        if(it->second.size() <= it->first) {
            if(it->first != 0)
                temp.push_back(*it);
            it = workList.erase(it);
        } else
            it++;
    }

    vector<set<ThreeAddressNodePtr>> safeSet;
    for(auto subset : workList) {
        vector<set<ThreeAddressNodePtr>> s1;
        vector<set<ThreeAddressNodePtr>> s2;
        for(auto elementHy : subset.second) {
            vector<ThreeAddressNodePtr> tempSet;
            StateH::addElementHY(tempSet, elementHy);
            bool flag = true;
            for(auto ele : tempSet) {
                if(!newState.hasTop(ele)) {
                    flag = false;
                    break;
                }
            }
            if(flag) {
                s1.push_back(elementHy);
                safeSet.push_back(elementHy);
            }
            else {
                s2.push_back(elementHy);
            }
        }
        assert(s1.size() >= subset.first);
        splits.push_back(make_pair(s1, s2));
    }

    WorkList accu;
    WorkList newWorklist;
    vector<int> enumration;
    return exploreAux(workList, enumration, newWorklist, temp, splits);

}

bool WLVerifier::explore2(WorkList workList, CheckingType checkingType) {
    count++;
    for(auto ele : workList) {
        if(ele.first > ele.second.size()) {
            return true;
        }
    }
    if(count == 4)
        cout << "1" << endl;

    set<set<ThreeAddressNodePtr>> subset = choose(workList);

    set<ThreeAddressNodePtr> subset1;
    for(auto ele : subset) {
        subset1.insert(ele.begin(), ele.end());
    }
    set<string> fordebug;
    for(auto ele : subset1) {
        fordebug.insert(ele->getNodeName());
    }


    set<set<ThreeAddressNodePtr>> subsetFromYb;
    set<string> namesFromYb;
    if(!fromY.empty()) {
        for(auto ele : subset) {
            if(fromY.count(ele) != 0) {
                subsetFromYb.insert(ele);
                for(auto ele1 : ele) {
                    namesFromYb.insert(ele1->getNodeName());
                }
            }
        }
    }


    vector<Split> splits;

    StateNew newState(subset1, checkingType, Y, order);


    bool res = check2(newState);

    if (res == true) {
        extend2(newState, workList, checkingType);
    } else if(checkingType == CheckingType::ydwsni) {
        setThatCanNotBeProved.insert(subset1);
    } else if (checkingType == CheckingType::ydsni) {
        throw subset1;
    } else {
        throw subset1;
    }




    vector<set<ThreeAddressNodePtr>> safeSet;
    for(auto element : workList) {
        set<set<ThreeAddressNodePtr>> s1;
        set<set<ThreeAddressNodePtr>> s2;
        for(auto elementHy : element.second) {
            if(subset.count(elementHy) != 0) {
                s1.insert(elementHy);
                safeSet.push_back(elementHy);
                continue;
            }
            vector<ThreeAddressNodePtr> tempSet;
            StateH::addElementHY(tempSet, elementHy);
            bool flag = true;
            for(auto ele : tempSet) {
                if(!newState.hasTop(ele)) {
                    flag = false;
                    break;
                }
            }
            if(flag) {
                s1.insert(elementHy);
                safeSet.push_back(elementHy);
            }
            else {
                s2.insert(elementHy);
            }
        }
        assert(s1.size() >= element.first);
        splits.push_back(make_pair(vector<set<ThreeAddressNodePtr>>{s1.begin(), s1.end()},
                                   vector<set<ThreeAddressNodePtr>>{s2.begin(), s2.end()}));
    }
    cout << "after extend:\n";
    for(auto ele : safeSet) {
        cout << "{";
        for (auto ele1 : ele)
            cout << ele1->getNodeName() << ", ";
        cout << "}\n";
    }

    WorkList temp;
    for(auto it = workList.begin(); it != workList.end(); ) {
        if(it->second.size() <= it->first) {
            if(it->first != 0)
                temp.push_back(*it);
            it = workList.erase(it);
        } else
            it++;
    }

    WorkList accu;
    WorkList newWorklist;
    vector<int> enumration;
    return exploreAux2(workList, enumration, newWorklist, temp, splits, checkingType);

}




bool WLVerifier::explore2B(WorkList workList, CheckingType checkingType) {
    count++;
    for(auto ele : workList) {
        if(ele.first > ele.second.size()) {
            return true;
        }
    }


    set<set<ThreeAddressNodePtr>> subset = choose(workList);

    set<ThreeAddressNodePtr> subset1;
    for(auto ele : subset) {
        subset1.insert(ele.begin(), ele.end());
    }
    set<string> fordebug;

    if(fordebug == set<string>{"res0", "t14", "t19"}) {
        for(auto ele : subset1) {
            cout << ele->prettyPrint5() << endl;
        }
    }


    set<set<ThreeAddressNodePtr>> subsetFromYb;
    set<string> namesFromYb;
    if(!fromY.empty()) {
        for(auto ele : subset) {
            if(fromY.count(ele) != 0) {
                subsetFromYb.insert(ele);
                for(auto ele1 : ele) {
                    namesFromYb.insert(ele1->getNodeName());
                }
            }
        }
    }


    vector<Split> splits;

    StateNew newState(subset1, checkingType, Y, order);
    bool res = false;
    if(checkingType == CheckingType::ydwsni) {
        res = check2ForWSNI(newState, subset1);
    } else if(checkingType == CheckingType::ydpini) {
        res = check2(newState);
        auto witness = newState.getCopiedProofName();
        set<int> outputIndices;
        for(auto ele : subset1) {
            if(outputToShareIndice.count(ele) != 0)
                outputIndices.insert(outputToShareIndice[ele]);
        }
        set<int> inputIndices;
        for(auto shares : witness) {
            for(auto share : shares) {
                assert(intputToShareIndice.count(share) != 0);
                inputIndices.insert(intputToShareIndice[share]);
            }
        }


        set<int> diff;
        for(auto ele : inputIndices) {
            if(outputIndices.count(ele) == 0) {
                diff.insert(ele);
            }
        }

        if(diff.size() > (order - outputIndices.size())) {
            res = false;
        } else {
            res = true;
        }



    } else {
        res = check2(newState);
    }


    if (res == true) {
        extend2(newState, workList, checkingType);
    } else if(checkingType == CheckingType::ydwsni) {
        throw subset1;
    } else if (checkingType == CheckingType::ydsni) {
        throw subset1;
    } else {
        setThatCanNotBeProved.insert(subset1);
    }




    vector<set<ThreeAddressNodePtr>> safeSet;
    for(auto element : workList) {
        set<set<ThreeAddressNodePtr>> s1;
        set<set<ThreeAddressNodePtr>> s2;
        for(auto elementHy : element.second) {
            if(subset.count(elementHy) != 0) {
                s1.insert(elementHy);
                safeSet.push_back(elementHy);
                continue;
            }
            vector<ThreeAddressNodePtr> tempSet;
            StateH::addElementHY(tempSet, elementHy);
            bool flag = true;
            for(auto ele : tempSet) {
                if(!newState.hasTop(ele)) {
                    flag = false;
                    break;
                }
            }
            if(flag) {
                s1.insert(elementHy);
                safeSet.push_back(elementHy);
            }
            else {
                s2.insert(elementHy);
            }
        }
        assert(s1.size() >= element.first);
        splits.push_back(make_pair(vector<set<ThreeAddressNodePtr>>{s1.begin(), s1.end()},
                                   vector<set<ThreeAddressNodePtr>>{s2.begin(), s2.end()}));
    }


    WorkList accu;
    return exploreAux2B(workList, accu, splits, checkingType);

}

bool WLVerifier::exploreAux(WorkList& workList, vector<int>& enumration, WorkList& newWorklist, WorkList& temp, vector<Split>& splits) {
    if(workList.size() == 0) {
        int sum = 0;
        for(auto ele : enumration) sum += ele;
        if(sum != 0) {
            WorkList newOne = newWorklist;
            newOne.insert(newOne.end(), temp.begin(), temp.end());
            bool res = explore(newOne);
            if(!res)
                return false;
        }
    } else {
        NumberSubsetPair numberSubsetPair = workList.back();
        Split split = splits.back();
        workList.pop_back();
        splits.pop_back();

        for(int j = 0; j <= std::min(numberSubsetPair.first, int(split.second.size())); j++) {
            NumberSubsetPair pair1(numberSubsetPair.first - j, split.first);
            NumberSubsetPair pair2(j, split.second);
            if(pair1.first != 0)
                newWorklist.push_back(pair1);
            if(pair2.first != 0)
                newWorklist.push_back(pair2);
            enumration.push_back(j);
            bool res = exploreAux(workList, enumration, newWorklist, temp, splits);
            if(!res) return false;
            enumration.pop_back();
            if(pair1.first != 0)
                newWorklist.pop_back();
            if(pair2.first != 0)
                newWorklist.pop_back();
        }
    }
    return true;
}

bool WLVerifier::exploreAux2B(WorkList workList, WorkList accu,
                             vector<Split> splits, CheckingType checkingType) {

    WorkList temp = workList;
    if(workList.size() == 0)
        return true;

    NumberSubsetPair numberSubsetPair = workList.back();
    Split split = splits.back();

    workList.pop_back();
    splits.pop_back();

    vector<set<ThreeAddressNodePtr>> s1 = split.first;
    vector<set<ThreeAddressNodePtr>> s2 = split.second;

    int d = numberSubsetPair.first;
    int len = numberSubsetPair.second.size();
    int len1 = s1.size();

    if(d > len1) assert(false);

    int len2 = len - len1;

    WorkList accu1 = accu;
    NumberSubsetPair numberSubsetPair1(d, s1);
    accu1.push_back(numberSubsetPair1);
    bool res = exploreAux2B(workList, accu1, splits, checkingType);
    if(!res)
        return false;

    workList.insert(workList.end(), accu.begin(), accu.end());

    if(d <= len2) {
        WorkList workList1 = workList;
        NumberSubsetPair numberSubsetPair2(d, s2);
        workList1.push_back(numberSubsetPair2);
        bool res = explore2B(workList1, checkingType);
        if(!res)
            return false;
    }

    int i1 = 1;
    while(i1 < d) {
        int i2 = d - i1;
        if(i1 <= len1 && i2 <= len2) {
            WorkList workList2 = workList;
            int reservedWorlistNumber = workList2.size();
            NumberSubsetPair numberSubsetPair3(i1, s1);
            NumberSubsetPair numberSubsetPair4(i2, s2);
            workList2.push_back(numberSubsetPair3);
            workList2.push_back(numberSubsetPair4);
            bool res = explore2B(workList2, checkingType);
            if(!res) return false;

            i1 += 1;
        } else {
            i1 += 1;
        }
    }
    return true;




}
bool WLVerifier::exploreAux2(WorkList& workList, vector<int>& enumration, WorkList& newWorklist, WorkList& temp,
                                vector<Split>& splits, CheckingType checkingType) {
    if(workList.size() == 0) {
        int sum = 0;
        for(auto ele : enumration) sum += ele;
        if(sum != 0) {
            WorkList newOne = newWorklist;
            newOne.insert(newOne.end(), temp.begin(), temp.end());
            bool res = explore2(newOne, checkingType);
            if(!res)
                return false;
        }
    } else {
        NumberSubsetPair numberSubsetPair = workList.back();
        Split split = splits.back();
        workList.pop_back();
        splits.pop_back();

        for(int j = 0; j <= std::min(numberSubsetPair.first, int(split.second.size())); j++) {
            NumberSubsetPair pair1(numberSubsetPair.first - j, split.first);
            NumberSubsetPair pair2(j, split.second);
            if(pair1.first != 0)
                newWorklist.push_back(pair1);
            if(pair2.first != 0)
                newWorklist.push_back(pair2);
            enumration.push_back(j);
            bool res = exploreAux2(workList, enumration, newWorklist, temp, splits, checkingType);
            if(!res) return false;
            enumration.pop_back();
            if(pair1.first != 0)
                newWorklist.pop_back();
            if(pair2.first != 0)
                newWorklist.pop_back();
        }
    }
    return true;
}

const set<set<ThreeAddressNodePtr>> &WLVerifier::getSetThatCanNotBeProved() const {
    return setThatCanNotBeProved;
}

int WLVerifier::getCount() const {
    return count;
}


void WLVerifier::printWorklist(WorkList& worklist) {
    for(auto ele : worklist) {
        cout << "[";
        cout << ele.first << ",";
        for(auto ele1 : ele.second) {
            cout << "{";
            for(auto ele2 : ele1) {
                cout << ele2->getNodeName() << ", ";
            }
            cout << "}" << ",";
        }
        cout << "]";
    }
    cout << endl;
}

void WLVerifier::setOutputToShareIndice(const map<ThreeAddressNodePtr, int> &outputs) {
    this->outputToShareIndice = outputs;
}

void WLVerifier::setIntputToShareIndice(const map<string, int> &intputToShareIndice) {
    WLVerifier::intputToShareIndice = intputToShareIndice;
}
