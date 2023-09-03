//
// Created by Pengfei Gao on 2021/9/19.
//

#include "HOChecker.h"


bool HOChecker::checkWhetherDom(ThreeAddressNodePtr newReturnNode, const set<ThreeAddressNodePtr>& actuals) {
    for(auto ele : actuals) {
        set<ThreeAddressNodePtr> subset1{newReturnNode};
        set<ThreeAddressNodePtr> keys;
        StateH newState(subset1, 1, keys);
        if(newState.canSimplyUsingSpecificDomMain(ele->getNodeName())) {
            return true;
        }
    }
    return false;

}


YAndType HOChecker::sgadget2(ProcedureHPtr f, int d, CheckType t) {
    auto Y = f->getY();

    procaToInlinedBlock[f] = f->getBlock();

    cout << "check single gadget: " << f->getName() << endl;

//    if(f->getName() == "compression") {
//        cout << "test" << endl;
//    }

    if(f->getName() == "XOR") {
        Y.clear();
        for(auto ele : f->getReturns()) {
            set<ThreeAddressNodePtr> temp;
            temp.insert(ele);
            Y.push_back(temp);
        }

        return YAndType(Y, CheckType::NI);
    }
    if(t == CheckType::NI) {
        vector<set<ThreeAddressNodePtr>> p1;
        for(auto ele : f->getBlock()) {
            p1.push_back(set<ThreeAddressNodePtr>{ele});
        }
        WorkList workList;
        workList.push_back(NumberSubsetPair(d, p1));
        WLVerifier wlVerifier(workList, set<set<ThreeAddressNodePtr>>(), d, Y);
        wlVerifier.beginExpore2(CheckingType::ydni);
        cout << "count for checking NI: " << wlVerifier.getCount() << endl;

    } else if(t == CheckType::SNI){
        int numberOfReturn = f->getReturns().size() / (d + 1);
        vector<vector<set<ThreeAddressNodePtr>>> returnNodes;
        map<ThreeAddressNodePtr, int> returnNodeToIndice;
        for(int i = 0; i < numberOfReturn; i++) {
            returnNodes.push_back(vector<set<ThreeAddressNodePtr>>());
        }
        for(int i = 0; i < f->getReturns().size(); i++) {
            returnNodes[i / (d + 1)].push_back(set<ThreeAddressNodePtr>{f->getReturns()[i]});
            returnNodeToIndice[f->getReturns()[i]] = i / (d + 1);
        }
        vector<set<ThreeAddressNodePtr>> blockWithOutReturn;
        for(auto ele : f->getBlock()) {
            if(returnNodeToIndice.count(ele) == 0)
                blockWithOutReturn.push_back(set<ThreeAddressNodePtr >{ele});
        }
        for(int i = 0; i <= d; i++) {
            WorkList workList;
            if(i != 0)
                workList.push_back(NumberSubsetPair(i, blockWithOutReturn));
            if((d - i) != 0) {
                for(int j = 0; j < returnNodes.size(); j++)
                    workList.push_back(NumberSubsetPair(d - i, returnNodes[j]));
            }
            try {
                WLVerifier wlVerifier(workList, set<set<ThreeAddressNodePtr>>(), i, Y);
                wlVerifier.beginExpore2(CheckingType::ydsni);
                cout << "count for checking SNI: " << wlVerifier.getCount() << endl;
            } catch (set<ThreeAddressNodePtr> subset) {
                throw subset;
            }
        }
    } else if(t == CheckType::WSNI) {
        vector<vector<set<ThreeAddressNodePtr>>> returnNodes;
        map<ThreeAddressNodePtr, int> returnNodeToIndice;
        int numberOfReturn = f->getReturns().size() / (d + 1);
        for(int i = 0; i < numberOfReturn; i++) {
            returnNodes.push_back(vector<set<ThreeAddressNodePtr>>());
        }
        for(int i = 0; i < f->getReturns().size(); i++) {
            returnNodes[i / (d + 1)].push_back(set<ThreeAddressNodePtr>{f->getReturns()[i]});
            returnNodeToIndice[f->getReturns()[i]] = i / (d + 1);
        }
        vector<set<ThreeAddressNodePtr>> blockWithOutReturn;
        for(auto ele : f->getBlock()) {
            if(returnNodeToIndice.count(ele) == 0)
                blockWithOutReturn.push_back(set<ThreeAddressNodePtr >{ele});
        }
        set<set<ThreeAddressNodePtr>> fromY;
        for(auto ele : f->getReturns()) {
            set<ThreeAddressNodePtr> tempset{ele};
            fromY.insert(tempset);
        }

        int totalWSNI = 0;
        for(int i = 0; i <= d; i++) {
            WorkList workList;
            if(i != 0)
                workList.push_back(NumberSubsetPair(i, blockWithOutReturn));
            if((d - i) != 0) {
                for (int j = 0; j < returnNodes.size(); j++)
                    workList.push_back(NumberSubsetPair(d - i, returnNodes[j]));
            }
            WLVerifier wlVerifier(workList, set<set<ThreeAddressNodePtr>>(), d, Y);
            wlVerifier.setOutputToShareIndice(returnNodeToIndice);
            wlVerifier.beginExpore2(CheckingType::ydwsni);
            weakSNIToCannotCheck[f].insert(wlVerifier.getSetThatCanNotBeProved().begin(), wlVerifier.getSetThatCanNotBeProved().end());
            totalWSNI += wlVerifier.getCount();
        }
        cout << "count for checking WSNI: " << totalWSNI << endl;

        for(auto ele : weakSNIToCannotCheck[f]) {
            cout << "==========" << endl;
            for(auto ele1 : ele) {
                cout << ele1->prettyPrint5() << endl;
            }
            cout << "==========" << endl;
        }

    }
    cout << "Y size: " << Y.size() << endl;

    set<set<ThreeAddressNodePtr>> tempSet(Y.begin(), Y.end());
    Y.clear();
    for(auto ele : tempSet)
        Y.push_back(ele);
    if(t == CheckType::WSNI)
        cout << "hello" << endl;
    return YAndType(Y, t);
}

YAndType HOChecker::sgadget(ProcedureHPtr f, int d, CheckType t) {
    auto Y = f->getY();

    procaToInlinedBlock[f] = f->getBlock();

    cout << "check single gadget: " << f->getName() << endl;

    if(f->getName() == "XOR")
        return YAndType(Y, CheckType::NI);
    if(t == CheckType::NI) {
        WorkList workList;
        vector<set<ThreeAddressNodePtr>> p1;
        for(auto ele : f->getBlock()) {
            p1.push_back(set<ThreeAddressNodePtr>{ele});
        }
        workList.push_back(NumberSubsetPair(d, p1));
        int count = 0;
        explore(f, d, workList, Y, t, YsetToInternal[f], internalToY[f], count, set<set<ThreeAddressNodePtr>>());
        cout << "count for checking NI: " << count << endl;
        totalChecked += count;
        singleChecked += count;
    } else if(t == CheckType::SNI){
        int numberOfReturn = f->getReturns().size() / (d + 1);
        vector<vector<set<ThreeAddressNodePtr>>> returnNodes;
        set<ThreeAddressNodePtr> returnNodesSet;
        for(int i = 0; i < numberOfReturn; i++) {
            returnNodes.push_back(vector<set<ThreeAddressNodePtr>>());
        }
        for(int i = 0; i < f->getReturns().size(); i++) {
            returnNodes[i / (d + 1)].push_back(set<ThreeAddressNodePtr>{f->getReturns()[i]});
            returnNodesSet.insert(f->getReturns()[i]);
        }

        vector<set<ThreeAddressNodePtr>> blockWithOutReturn;
        for(auto ele : f->getBlock()) {
            if(returnNodesSet.count(ele) == 0)
                blockWithOutReturn.push_back(set<ThreeAddressNodePtr >{ele});
        }
        for(int i = 0; i <= d; i++) {
            WorkList workList;
            if(i != 0)
                workList.push_back(NumberSubsetPair(i, blockWithOutReturn));
            if((d - i) != 0) {
                for(int j = 0; j < returnNodes.size(); j++)
                    workList.push_back(NumberSubsetPair(d - i, returnNodes[j]));
            }
            try {
                int count = 0;
                explore(f, i, workList, Y, t, YsetToInternal[f], internalToY[f], count, set<set<ThreeAddressNodePtr>>());
                cout << "count for checking SNI: " << count << endl;
                totalChecked += count;
                singleChecked += count;
            } catch (set<ThreeAddressNodePtr> subset) {
                throw subset;
            }
        }
    } else if(t == CheckType::WSNI) {
        vector<vector<set<ThreeAddressNodePtr>>> returnNodes;
        set<ThreeAddressNodePtr> returnNodesSet;
        int numberOfReturn = f->getReturns().size() / (d + 1);
        for(int i = 0; i < numberOfReturn; i++) {
            returnNodes.push_back(vector<set<ThreeAddressNodePtr>>());
        }
        for(int i = 0; i < f->getReturns().size(); i++) {
            returnNodes[i / (d + 1)].push_back(set<ThreeAddressNodePtr>{f->getReturns()[i]});
            returnNodesSet.insert(f->getReturns()[i]);
        }
        vector<set<ThreeAddressNodePtr>> blockWithOutReturn;
        for(auto ele : f->getBlock()) {
            if(returnNodesSet.count(ele) == 0)
                blockWithOutReturn.push_back(set<ThreeAddressNodePtr >{ele});
        }
        set<set<ThreeAddressNodePtr>> fromY;
        for(auto ele : f->getReturns()) {
            set<ThreeAddressNodePtr> tempset{ele};
            fromY.insert(tempset);
        }

        for(int i = 0; i <= d; i++) {
            WorkList workList;
            if(i != 0)
                workList.push_back(NumberSubsetPair(i, blockWithOutReturn));
            if((d - i) != 0)
                for(int j = 0; j < returnNodes.size(); j++)
                    workList.push_back(NumberSubsetPair(d - i, returnNodes[j]));

            int count = 0;
            explore(f, d, workList, Y, t, YsetToInternal[f], internalToY[f], count, fromY);
            cout << "count for checking WSNI: " << count << endl;
            totalChecked += count;
            singleChecked += count;

        }

        for(auto ele : weakSNIToCannotCheck[f]) {
            cout << "==========" << endl;
            for(auto ele1 : ele) {
                cout << ele1->prettyPrint5() << endl;
            }
            cout << "==========" << endl;
        }

    }
    cout << "Y size: " << Y.size() << endl;

    set<set<ThreeAddressNodePtr>> tempSet(Y.begin(), Y.end());
    Y.clear();
    for(auto ele : tempSet)
        Y.push_back(ele);

    return YAndType(Y, t);
}


set<set<ThreeAddressNodePtr>> HOChecker::choose(const WorkList& worklist) {
    set<set<ThreeAddressNodePtr>> result;
    for(auto workListElement : worklist) {
        for(int i = 0; i < workListElement.first; i++) {
            result.insert(workListElement.second[i]);
        }
    }
    return result;
}

bool HOChecker::checkMain(int d, StateH& state, const vector<vector<ThreeAddressNodePtr>>& Paras, CheckType t) {
    state.obtainKeyExpressionsMain();
    if(state.canSimulatedMain(d, t, Paras))
        return true;

    if(state.canSimplyUsingDomMain(d, Paras, t)) {
        return true;
    }
    return false;

}

bool HOChecker::check(int d, StateH& state, const set<string>& Ybnames) {
    state.obtainKeyExpressions(Ybnames);
    if(state.directlyInclude(d)) {
        return true;
    }

    if(state.canSimulated(d)) {
        return true;
    }

    if(state.canSimplyUsingDom(d))
        return true;

    if(state.canSimulatedUsingSet1(d)) {
        return true;
    }

    return false;
}

void HOChecker::extendMain(StateH& state, WorkList worklist,  const vector<vector<ThreeAddressNodePtr>>& Para) {
    set<ThreeAddressNodePtr> oldKeyExpressions = state.getKeyExpressions();
    for(auto subset : worklist) {
        for(auto ele : subset.second) {
            state.setTop(ele, state.getOriginalY());
        }
    }

    for(auto domToExpression : state.getDomExpressions()) {
        state.removeAllNodeAndParent(domToExpression.first);
    }


    state.obtainKeyExpressionsMain();

    if(!state.canSimplyUsingDomMain(state.getOrder(), Para, CheckType::NI)) {
        set<ThreeAddressNodePtr> newKeyExps = state.getKeyExpressions();
        while (!newKeyExps.empty()) {
            auto first = *newKeyExps.begin();
            if(!state.getCopiedProof().empty() && (*state.getCopiedProof().begin()).count(first) == 0)
                state.removeAllNodeAndParent(first);
            newKeyExps.erase(first);
        }
    }


}

void HOChecker::extend(StateH& state, WorkList worklist) {
    set<ThreeAddressNodePtr> oldKeyExpressions = state.getKeyExpressions();
    for(auto subset : worklist) {
        for(auto ele : subset.second) {
            state.setTop(ele, state.getOriginalY());
        }
    }

    for(auto domToExpression : state.getDomExpressions()) {
        state.removeAllNodeAndParent(domToExpression.first);
    }

    state.obtainKeyExpressions(set<string>());
    if(!state.canSimulatedWithProof(state.getCopiedKeyExpressions())) {
        vector<ThreeAddressNodePtr> eleInProof = state.getElementInProof();
        vector<ThreeAddressNodePtr> eleInProof2 = state.test();
        oldKeyExpressions.insert(eleInProof.begin(), eleInProof.end());
        oldKeyExpressions.insert(eleInProof2.begin(), eleInProof2.end());

        state.trimKeyExpressions(oldKeyExpressions);

        state.removeUsedKeyExpressions1(oldKeyExpressions, 0);
    }
}

bool HOChecker::exploreMain(int d, WorkList workList, const vector<vector<ThreeAddressNodePtr>>& parameters, const set<ThreeAddressNodePtr>& keys, CheckType t, int& count) {
    for(auto ele : workList) {
        if(ele.first > ele.second.size())
            return true;
    }
    set<set<ThreeAddressNodePtr>> subset = choose(workList);
    set<ThreeAddressNodePtr> subset1;
    for(auto ele : subset) {
        subset1.insert(ele.begin(), ele.end());
    }

    count++;
    vector<Split> splits;

    StateH newState(subset1, d, keys);
    bool res = checkMain(d, newState, parameters, t);
    if (res == true) {
        extendMain(newState, workList, parameters);
    }
    else if (t == CheckType::SNI) {
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
    return exploreAuxMain(workList, parameters, keys, splits, enumration, newWorklist, temp, count, t, d);

}





bool HOChecker::explore(ProcedureHPtr f, int d, WorkList workList, vector<set<ThreeAddressNodePtr>>& Y, CheckType t,
                                                    map<set<ThreeAddressNodePtr>, ThreeAddressNodePtr>& YToInternal,
                                                    map<ThreeAddressNodePtr, set<ThreeAddressNodePtr>>& internalToY, int& count,
                                                    const set<set<ThreeAddressNodePtr>>& fromY) {
    for(auto ele : workList) {
        if(ele.first > ele.second.size())
            return true;
    }
    set<set<ThreeAddressNodePtr>> subset = choose(workList);
    set<ThreeAddressNodePtr> subset1;
    for(auto ele : subset) {
        subset1.insert(ele.begin(), ele.end());
    }


    set<set<ThreeAddressNodePtr>> Ybsubset1;
    set<string> Ybnames;
    if(!fromY.empty()) {
        for(auto ele : subset) {
            if(fromY.count(ele) != 0) {
                Ybsubset1.insert(ele);
                for(auto ele1 : ele) {
                    Ybnames.insert(ele1->getNodeName());
                }
            }
        }
    }



    count++;


    vector<Split> splits;

    StateH newState(subset1, Y, YToInternal, internalToY, d);

    for(auto ele : Ybsubset1) {
        set<ThreeAddressNodePtr> temp;
        for(auto ele1 : ele) {
            assert(newState.getSaved(ele1) != nullptr);
            temp.insert(newState.getSaved(ele1));
        }
        newState.addToCopiedProof(temp);
    }

    bool res = check(d - Ybsubset1.size(), newState, Ybnames);

    if (res == true) {
        extend(newState, workList);
    }
    else if (t == CheckType::SNI) {
        for(auto ele : subset) {
            cout << "{";
            for(auto ele1 : ele)
                cout << ele1->prettyPrint5() << endl;
            cout << "}\n";
        }
        cout << "==========================\n";
        cout << "}" << endl;
        throw subset1;
    } else if(t == CheckType::WSNI) {
        weakSNIToCannotCheck[f].insert(subset1);
    }
    else {
        for(auto ele : subset) {
            if(ele.size() == 1 && (*ele.begin())->getNodeType() == NodeType::RANDOM)
                continue;
            Y.push_back(ele);
        }
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
    cout << "after extend:\n";
    for(auto ele : safeSet) {
        cout << "{";
        for (auto ele1 : ele)
            cout << ele1->getNodeName() << ", ";
        cout << "}\n";
    }

    WorkList accu;
    WorkList newWorklist;
    vector<int> enumration;
    return exploreAux(f, workList, Y, splits, enumration, newWorklist, temp, count, t, d, YToInternal, internalToY, fromY);

}


bool HOChecker::exploreAuxMain(WorkList workList, const vector<vector<ThreeAddressNodePtr >>& Paras, const set<ThreeAddressNodePtr>& keys, vector<Split> splits, vector<int>& enumration, WorkList& newWorklist, WorkList& temp, int& count, CheckType t,int d
                           ) {
    if(workList.size() == 0) {
        int sum = 0;
        for(auto ele : enumration) sum += ele;
        if(sum != 0) {
            WorkList newOne = newWorklist;
            newOne.insert(newOne.end(), temp.begin(), temp.end());
            bool res = exploreMain(d, newOne, Paras, keys, t,  count);
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
            bool res = exploreAuxMain(workList, Paras, keys, splits, enumration, newWorklist, temp, count, t, d);
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

bool HOChecker::exploreAux(ProcedureHPtr f ,WorkList workList, vector<set<ThreeAddressNodePtr >>& Y, vector<Split> splits, vector<int>& enumration, WorkList& newWorklist, WorkList& temp, int& count, CheckType t,int d,
                           map<set<ThreeAddressNodePtr>, ThreeAddressNodePtr>& YToInternal,
                           map<ThreeAddressNodePtr, set<ThreeAddressNodePtr>>& internalToY, const set<set<ThreeAddressNodePtr>>& fromY) {
    if(workList.size() == 0) {
        int sum = 0;
        for(auto ele : enumration) sum += ele;
        if(sum != 0) {
            WorkList newOne = newWorklist;
            newOne.insert(newOne.end(), temp.begin(), temp.end());
            bool res = explore(f, d, newOne, Y, t, YToInternal, internalToY, count, fromY);
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
            bool res = exploreAux(f, workList, Y, splits, enumration, newWorklist, temp, count, t, d, YToInternal, internalToY, fromY);
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



YAndType HOChecker::gadget2(ProcedureHPtr f, int d) {
    if(storedInfo.count(f) != 0) {
        return storedInfo[f];
    }

    YAndType res1;
    if(f->isSimple1()) {
        try {
            res1 = sgadget2(f, d, CheckType::SNI);
        }
        catch (set<ThreeAddressNodePtr> subset){
            try {
                res1 = sgadget2(f, d, CheckType::WSNI);
            } catch (set<ThreeAddressNodePtr> subset) {
                res1 = sgadget2(f,d, CheckType::NI);
            }
        }
    } else {
        res1 = gadgetCalls3(f, d);
    }
    storedInfo[f] = res1;
    return res1;



}

YAndType HOChecker::gadget(ProcedureHPtr f, int d) {
    if(storedInfo.count(f) != 0) {
        return storedInfo[f];
    }
    bool isSimpleGadget = true;
    for(auto ele : f->getBlock()) {
        if(ele->getOp() == ASTNode::Operator::PUSH) {
            isSimpleGadget = false;
            break;
        }
    }

    YAndType res1;
    if(isSimpleGadget) {
        try {
            res1 = sgadget(f, d, CheckType::SNI);
        }
        catch (set<ThreeAddressNodePtr> subset){
            try {
                res1 = sgadget(f, d, CheckType::WSNI);
            } catch (set<ThreeAddressNodePtr> subset) {
                res1 = sgadget(f,d, CheckType::NI);
            }
        }
    } else {
        res1 = gadgetCalls(f, d);
    }
    storedInfo[f] = res1;
    return res1;
}

void HOChecker::checkNI(string name, int d) {

    ProcedureHPtr f;
    for(auto ele : procedureHs) {
        if(ele->getName() == name) {
            f = ele;
            YAndType y = gadget2(f, d);
            vector<set<ThreeAddressNodePtr>> X;
            getCartesian(f->getParameters(), X);
            WorkList workList;
            workList.push_back(NumberSubsetPair(d, y.first));
            int count = 0;
            exploreMain(d, workList, f->getParameters(), set<ThreeAddressNodePtr>(), CheckType::SNI, count);
        }
    }
}

void HOChecker::checkSNI(string name, int d) {

    ProcedureHPtr f;
    for(auto ele : procedureHs) {
        if(ele->getName() == name) {
            f = ele;
            YAndType y = gadget(f, d);
            if(y.second != CheckType::SNI){
                cout << "not SNI" << endl;
                return;
            }
            vector<set<ThreeAddressNodePtr>> X;
            getCartesian(f->getParameters(), X);
            int count = 0;

            int numberOfReturn = f->getReturns().size() / (d + 1);
            vector<vector<set<ThreeAddressNodePtr>>> returnNodes;
            for(int i = 0; i < numberOfReturn; i++) {
                returnNodes.push_back(vector<set<ThreeAddressNodePtr>>());
            }
            for(int i = 0; i < f->getReturns().size(); i++) {
                returnNodes[i / (d + 1)].push_back(set<ThreeAddressNodePtr>{f->getReturns()[i]});
            }

            for(int i = 0;i <= d; i++) {
                WorkList workList;
                workList.push_back(NumberSubsetPair(i, y.first));
                for(int j = 0; j < numberOfReturn; j++)
                    workList.push_back(NumberSubsetPair(d - i, returnNodes[j]));
                exploreMain(i, workList, f->getParameters(), set<ThreeAddressNodePtr>(), CheckType::SNI, count);
            }
        }
    }
}


void HOChecker::checkProbing(int d) {
    clock_t s1 = clock();
    for(auto ele : procedureHs) {
        if(ele->getName() == "main") {
            YAndType y = gadget2(ele, d);

            map<string, ThreeAddressNodePtr> nodeMap;
            set<ThreeAddressNodePtr> saved;
            for(auto v : y.first) {
                for(auto e : v) {
                    checkInlinedNode(e, nodeMap, saved);
                }
            }

            WorkList workList;
            workList.push_back(NumberSubsetPair(d, y.first));
            WLVerifier wlVerifier(workList, set<set<ThreeAddressNodePtr>>(), d, vector<set<ThreeAddressNodePtr>>());
            wlVerifier.beginExpore2(CheckingType::prob);
            if(wlVerifier.getSetThatCanNotBeProved().empty()) {
                cout << "size of Y: " << y.first.size() << endl;
                cout << "count: " << wlVerifier.getCount() << endl;
                cout << "This masked implementation is probing secure." << endl;
            } else {
                for(auto ele : wlVerifier.getSetThatCanNotBeProved()) {
                    vector<ThreeAddressNodePtr> nodeV;
                    cout << "==================" << endl;
                    for(auto ele1 : ele) {
                        nodeV.push_back(ele1);
                        cout << ele1->prettyPrint5() << endl;
                    }
                    cout << "==================" << endl;
//                    GPUCounter::GPUCount(nodeV, 1);
                }
                cout << "need model-counting: " << wlVerifier.getSetThatCanNotBeProved().size() << endl;
            }
        }
    }
    clock_t s2 = clock();


}


void HOChecker::checkInlinedNode(ThreeAddressNodePtr node, map<string, ThreeAddressNodePtr>& nodeMap, set<ThreeAddressNodePtr>& saved) {

    if (!node)
        return;

    if (saved.count(node) != 0)
        return;

    checkInlinedNode(node->getLhs(), nodeMap, saved);
    checkInlinedNode(node->getRhs(), nodeMap, saved);

    if (node->getLhs() == nullptr && node->getRhs() == nullptr) {
        if (node->getNodeType() == NodeType::CONSTANT ||
            node->getNodeType() == NodeType::RANDOM || node->getNodeType() == NodeType::PUBLIC) {
            if (nodeMap.count(node->getNodeName()) == 0)
                nodeMap[node->getNodeName()] = node;
            else {
                assert(nodeMap[node->getNodeName()] == node);
            }
            return;
        } else if (nameOfKey.count(node->getNodeName()) != 0) {
            if (nodeMap.count(node->getNodeName()) == 0)
                nodeMap[node->getNodeName()] = node;
            else {
                assert(nodeMap[node->getNodeName()] == node);
            }
            node->setNodeType(NodeType::PRIVATE);
        } else if (nameOfPlain.count(node->getNodeName()) != 0) {
            if (nodeMap.count(node->getNodeName()) == 0)
                nodeMap[node->getNodeName()] = node;
            else {
                assert(nodeMap[node->getNodeName()] == node);
            }
            node->setNodeType(NodeType::PUBLIC);
        } else {
            assert(false);
        }
        assert(node->getNodeType() != NodeType::PARAMETER);
    } else {

        assert(node->getNodeType() != NodeType::PARAMETER);
        if (nodeMap.count(node->getNodeName()) == 0) {
            nodeMap[node->getNodeName()] = node;
        } else {
            if (node != nodeMap[node->getNodeName()]) {
                assert(false);
            }
        }
    }
    saved.insert(node);
}

void HOChecker::getCartesian(const vector<vector<ThreeAddressNodePtr>>& parameters, vector<set<ThreeAddressNodePtr>>& res) {
    if(parameters.size() == 1) {
        for(auto ele : parameters[0]) {
            res.push_back(set<ThreeAddressNodePtr>{ele});
        }
    } else {
        auto temp = vector<vector<ThreeAddressNodePtr>>{parameters.begin() + 1, parameters.end()};
        getCartesian(temp, res);
        auto tempRes = res;
        res.clear();
        for(auto ele : parameters[0]) {
            for(auto ele1 : tempRes) {
                ele1.insert(ele);
                res.push_back(ele1);
            }
        }
    }
}





void HOChecker::instantiateNewYg(YAndType& yAndType, ThreeAddressNodePtr callInstruction, vector<set<ThreeAddressNodePtr>>& newYg) {
    for(auto subset : yAndType.first) {
        set<ThreeAddressNodePtr> newSubset;
        for(auto ele : subset) {
            newSubset.insert(inliner.getMapCallToInlinedNode().at(callInstruction).at(ele));
        }
        newYg.push_back(newSubset);
    }
}


YAndType HOChecker::gadgetCalls3(ProcedureHPtr proc, int d) {
    Utils::debug_info("checkYtComp: " + proc->getName());
    vector<set<ThreeAddressNodePtr>> Yset = proc->getY();

    list<ThreeAddressNodePtr> nodelist;
    std::copy(proc->getBlock().begin(), proc->getBlock().end(), std::back_inserter(nodelist));

    vector<ThreeAddressNodePtr> functionCall;
    map<ThreeAddressNodePtr, vector<ActualPara>> mapCallToArguments;
    map<ThreeAddressNodePtr, vector<ThreeAddressNodePtr>> mapCallToReturnValues;
    map<ThreeAddressNodePtr, set<ThreeAddressNodePtr>> mapFuncCallArgumentsToCalls;
    map<ActualPara, set<ThreeAddressNodePtr>> mapInputArgumentsToCalls;
    proc->arrangeArguments(functionCall, mapCallToArguments, mapFuncCallArgumentsToCalls, mapInputArgumentsToCalls, mapCallToReturnValues);

    vector<ThreeAddressNodePtr> inlinedBlock;

    std::reverse(functionCall.begin(), functionCall.end());

    vector<set<ThreeAddressNodePtr>> finalY;

    vector<ThreeAddressNodePtr> finalOutputs = proc->getReturns();

    set<ThreeAddressNodePtr> curShares;

    for (int p = 0; p < functionCall.size(); p++) {
        auto callInstruction = functionCall[p];
        string nodeName = callInstruction->getNodeName();
        cout << "check: " << nodeName << endl;
        string::size_type pos1 = callInstruction->getNodeName().find("@");
        string path = callInstruction->getNodeName().substr(pos1, callInstruction->getNodeName().size());
        string calleeName = callInstruction->getNodeName().substr(0, pos1);
        ProcedureHPtr callee = nameToProc[calleeName];
        map<string, ThreeAddressNodePtr> saved;
        map<ThreeAddressNodePtr, ThreeAddressNodePtr> savedForFunction;

        YAndType yAndType = gadget2(callee, d);

        vector<set<ThreeAddressNodePtr>> newYg;


        vector<ThreeAddressNodePtr> newBlock;
        vector<ThreeAddressNodePtr> newReturns;


        if(p == 0){
            instantiateNewYg(yAndType, callInstruction, newYg);
            finalY.insert(finalY.end(), newYg.begin(), newYg.end());

            auto test = inliner.getMapCallToInlinedNode().at(callInstruction);
            for(auto ele : callee->getParameters()) {
                for(auto ele1 : ele) {
                    assert(test.count(ele1) != 0);
                    curShares.insert(test.at(ele1));
                }
            }
            continue;
        }

        if(calleeName != "preshare") {
            bool abortOccurs = false;
            if(yAndType.second == CheckType::SNI || yAndType.second == CheckType::WSNI) {
                bool res = false;
                if(!res) {
                    try {
                        set<ThreeAddressNodePtr> I1;
                        set<ThreeAddressNodePtr> X;
                        for (auto ele: finalY) {
                            for (auto ele1: ele) {
                                if ((ele1->getOp() == ASTNode::Operator::CALL && ele1->getLhs() != callInstruction) ||
                                    ele1->getNodeType() == NodeType::PARAMETER) {

                                    I1.insert(ele1);
                                } else if (ele1->getOp() == ASTNode::Operator::CALL && ele1->getLhs() == callInstruction) {
                                    X.insert(ele1);
                                }
                            }
                        }
                        auto test = inliner.getMapCallToInlinedNode().at(callInstruction);
                        vector<ThreeAddressNodePtr> inlinedReturns;
                        set<ThreeAddressNodePtr> inlinedReturnsSet;
                        for(auto ele : callee->getReturns()) {
                            inlinedReturns.push_back(test.at(ele));
                            inlinedReturnsSet.insert(test.at(ele));
                        }


                        set<ThreeAddressNodePtr> curSharesWithout;
                        for(auto ele : curShares) {
                            if(inlinedReturnsSet.count(ele) == 0) curSharesWithout.insert(ele);
                        }

                        vector<set<ThreeAddressNodePtr>> YfromReturns;
                        for(auto ele : inlinedReturnsSet) {
                            set<ThreeAddressNodePtr> temp{ele};
                            YfromReturns.push_back(temp);
                        }
                        for(auto& ele : YfromReturns) {
                            ele.insert(curSharesWithout.begin(), curSharesWithout.end());
                        }

                        for(int i = 1; i <= d - 1; i++) {
                            WorkList workList;
                            workList.push_back(NumberSubsetPair(i, finalY));


                            WLVerifier wlVerifier(workList, set<set<ThreeAddressNodePtr>>(), i, YfromReturns);
                            wlVerifier.beginExpore2(CheckingType::sni);
                            cout << "check SNI/WSNI self: " << wlVerifier.getCount() << endl;
                        }


                    } catch (set<ThreeAddressNodePtr> O) {
                        abortOccurs = true;
                    }

                }


                if((res || !abortOccurs) && (yAndType.second == CheckType::SNI ||
                                             (yAndType.second == CheckType::WSNI && weakSNIToCannotCheck[callee].empty()))) {
                    instantiateNewYg(yAndType, callInstruction, newYg);
                    if(yAndType.second == CheckType::SNI && functionCall.size() > 10) {
                        map<string, ThreeAddressNodePtr> nameToNewReturns;
                        map<ThreeAddressNodePtr, ThreeAddressNodePtr> record;

//                        for(int i = 0; i < callee->getReturns().size() ; i++) {
//                            auto ele = callee->getReturns()[i];
//                            ThreeAddressNodePtr newNode = make_shared<ThreeAddressNode>(ele->getNodeName() + "@" + path + "#" + to_string(i), nullptr, nullptr, ASTNode::Operator::NULLOP, NodeType::RANDOM);
//                            nameToNewReturns[ele->getNodeName()] = newNode;
//
//                        }

                        ThreeAddressNodePtr newNode = make_shared<ThreeAddressNode>(callee->getReturns()[0]->getNodeName() + "@" + path + "#" + to_string(0), nullptr, nullptr, ASTNode::Operator::NULLOP, NodeType::RANDOM);
                        nameToNewReturns[callee->getReturns()[0]->getNodeName()] = newNode;
                        ThreeAddressNodePtr last = newNode;
                        for(int i = 1; i < callee->getReturns().size() - 1; i++) {
                            auto ele = callee->getReturns()[i];
                            ThreeAddressNodePtr newNode = make_shared<ThreeAddressNode>(ele->getNodeName() + "@" + path + "#" + to_string(i), nullptr, nullptr, ASTNode::Operator::NULLOP, NodeType::RANDOM);
                            nameToNewReturns[ele->getNodeName()] = newNode;
                            last = make_shared<ThreeAddressNode>("last", last, newNode, ASTNode::Operator::XOR, NodeType::INTERNAL);

                        }
                        string lastName = callee->getReturns()[callee->getReturns().size() - 1]->getNodeName() + "@" + path + "#" +  to_string(callee->getReturns().size() - 1);
                        last->setNodeName(lastName);
                        nameToNewReturns[callee->getReturns()[callee->getReturns().size() - 1]->getNodeName()] = last;

                        set<ThreeAddressNodePtr > tempSaved;

                        for(auto& ele : finalY) {
                            set<ThreeAddressNodePtr> needTobeRemoved;
                            set<ThreeAddressNodePtr> needTobeAdded;
                            for(auto& ele1 : ele) {
                                if(ele1->getNodeName().find("@") == ele1->getNodeName().npos)
                                    continue;
                                string currentPath = ele1->getNodeName().substr(ele1->getNodeName().find("@"), ele1->getNodeName().find("(") - ele1->getNodeName().find("@"));
                                if(path == currentPath) {
                                    set<ThreeAddressNodePtr> corresponding;

                                    corresponding.insert(nameToNewReturns.at(ele1->getNodeName().substr(0, ele1->getNodeName().find("@"))));
                                    needTobeRemoved.insert(ele1);
                                    for(auto ele2 : corresponding) {
                                        needTobeAdded.insert(ele2);
                                        record[ele1] = ele2;
                                    }

                                } else {
                                    int count = 0;
                                    replaceByNewReturnsByName(ele1, path, nameToNewReturns, count, tempSaved, record);
                                }
                            }
                            for(auto ele1 : needTobeRemoved) {
                                ele.erase(ele1);
                            }
                            ele.insert(needTobeAdded.begin(), needTobeAdded.end());
                        }

                        for(auto& ele : inliner.getMutableMapCallToInlinedNode()) {
                            string name = ele.first->getNodeName();
                            if(proc->getName() == name.substr(0, name.find("@"))) {
                                map<ThreeAddressNodePtr, ThreeAddressNodePtr> oldToNew;

                                ThreeAddressNodePtr lastfirst = nullptr;
                                ThreeAddressNodePtr lastsecond = nullptr;
                                ThreeAddressNodePtr temp = nullptr;
                                string lastname = "";
                                for(auto ele1 : record) {


                                    if(ele1.second != nullptr) {
                                        cout << ele1.second->getNodeName() << endl;
//                                        ThreeAddressNodePtr insNode(
//                                                new ThreeAddressNode(ele.second[ele1.first]->getNodeName() + "#",
//                                                                     nullptr, nullptr, ASTNode::Operator::NULLOP,
//                                                                     NodeType::RANDOM));
//                                        oldToNew[ele.second[ele1.first]] = insNode;
//                                        ele.second[ele1.second] = oldToNew[ele.second[ele1.first]];

                                        ThreeAddressNodePtr insNode(
                                                    new ThreeAddressNode(ele.second[ele1.first]->getNodeName() + "#",
                                                                         nullptr, nullptr, ASTNode::Operator::NULLOP,
                                                                         NodeType::RANDOM));
                                        if(ele1.second->getNodeType() == NodeType::RANDOM) {
                                            ThreeAddressNodePtr insNode(
                                                    new ThreeAddressNode(ele.second[ele1.first]->getNodeName() + "#",
                                                                         nullptr, nullptr, ASTNode::Operator::NULLOP,
                                                                         NodeType::RANDOM));
                                            oldToNew[ele.second[ele1.first]] = insNode;
                                            ele.second[ele1.second] = oldToNew[ele.second[ele1.first]];
                                            if(temp == nullptr) {
                                                temp = insNode;
                                            } else {
                                                temp = make_shared<ThreeAddressNode>("last", temp, insNode, ASTNode::Operator::XOR, NodeType::INTERNAL);
                                            }
                                        } else {
                                            assert(ele1.second->getNodeType() == NodeType::INTERNAL);
                                            lastfirst = ele1.first;
                                            lastsecond = ele1.second;
                                            lastname = ele.second[ele1.first]->getNodeName() + "#";
                                        }
                                    }

                                }
                                if(temp != nullptr) {
                                    temp->setNodeName(lastname);
                                    oldToNew[ele.second[lastfirst]] = temp;
                                    ele.second[lastsecond] = oldToNew[ele.second[lastfirst]];
                                }

                                for(auto ele1 : record) {
                                    if(ele1.second != nullptr) {
                                        assert(ele.second.count(ele1.first) != 0);
                                        ele.second.at(ele1.first) = oldToNew[ele.second[ele1.first]];

                                    } else{
                                        for(auto ele2 : oldToNew) {
                                            if (ele.second[ele1.first]->getLhs() == ele2.first) {
                                                ele.second[ele1.first]->setLhs(ele2.second);
                                            } else if(ele.second[ele1.first]->getRhs() == ele2.first){
                                                ele.second[ele1.first]->setRhs(ele2.second);
                                            }
                                        }

                                    }
                                }
                            }
                        }
                    }

                    finalY.insert(finalY.end(), newYg.begin(), newYg.end());
                }

                if(!abortOccurs && yAndType.second == CheckType::WSNI && !weakSNIToCannotCheck[callee].empty()) {
                    instantiateNewYg(yAndType, callInstruction, newYg);
                    auto tempFinalY = finalY;
                    finalY.insert(finalY.end(), newYg.begin(), newYg.end());
                    set<set<ThreeAddressNodePtr >> fromY;
                    for(auto ele : tempFinalY)
                        fromY.insert(ele);
                    for(auto candidate : weakSNIToCannotCheck[callee]) {
                        set<ThreeAddressNodePtr> returnSet;
                        set<ThreeAddressNodePtr> internalSet;
                        if (calleeName != "XOR") {
                            vector<set<ThreeAddressNodePtr>> X;
                            for (auto ele: candidate) {
                                assert(ele->getOp() != ASTNode::Operator::PUSH);
                                if(ele->getNodeName().find("res") != ele->getNodeName().npos)
                                    returnSet.insert(ele);
                                else {
                                    internalSet.insert(ele);
                                    X.push_back(set<ThreeAddressNodePtr>{ele});
                                }
                            }
                            WorkList workList;
                            workList.push_back(NumberSubsetPair(internalSet.size(), X));
                            workList.push_back(NumberSubsetPair(returnSet.size(), tempFinalY));

                            for(auto ele : finalY) {
                                cout << "{";
                                for(auto ele1 : ele) {
                                    cout << ele1->prettyPrint5() << endl;
                                }
                                cout << "}\n";
                            }

                            int count = 0;
                            WLVerifier wlVerifier(workList, set<set<ThreeAddressNodePtr>>(), d, finalY);
                            wlVerifier.beginExpore2(CheckingType::ni);
                            cout << "check WSNI self: " << wlVerifier.getCount() << endl;
                            totalChecked += count;
                            compChecked += count;
                        }
                    }
                }


            }
            if(abortOccurs || yAndType.second == CheckType::NI) {
                instantiateNewYg(yAndType, callInstruction, newYg);
                auto tempFinalY = finalY;
                finalY.insert(finalY.end(), newYg.begin(), newYg.end());
                set<set<ThreeAddressNodePtr >> fromY;
                for(auto ele : tempFinalY) fromY.insert(ele);
                if(calleeName != "XOR") {
                    vector<set<ThreeAddressNodePtr>> X;
                    for (auto ele : newBlock) {
                        assert(ele->getOp() != ASTNode::Operator::PUSH);
                        X.push_back(set<ThreeAddressNodePtr>{ele});
                    }
                    for (int i = 1; i < d; i++) {
                        WorkList workList;
                        workList.push_back(NumberSubsetPair(i, X));
                        workList.push_back(NumberSubsetPair(d - i, tempFinalY));
                        int count = 0;
                        WLVerifier wlVerifier(workList, set<set<ThreeAddressNodePtr>>(), d, finalY);
                        wlVerifier.beginExpore2(CheckingType::ni);
                        cout << "check NI self: " << wlVerifier.getCount() << endl;
                        totalChecked += count;
                        compChecked += count;
                    }
                }
            }

            cout << "Y size: " << finalY.size() << endl;
            set<set<ThreeAddressNodePtr>> tempSet(finalY.begin(), finalY.end());
            finalY.clear();
            for(auto ele : tempSet)
                finalY.push_back(ele);
            cout << "Y size: " << finalY.size() << endl;

            cout << "==================================" << endl;

            auto test = inliner.getMapCallToInlinedNode().at(callInstruction);
            for(auto ele : callee->getParameters()) {
                for(auto ele1 : ele) {
                    assert(test.count(ele1) != 0);
                    curShares.insert(test.at(ele1));
                }
            }

        }


    }

    Utils::dealWithYt(finalY);

    return YAndType(finalY, CheckType::NI);

}



YAndType HOChecker::gadgetCalls(ProcedureHPtr proc, int d) {
    Utils::debug_info("checkYtComp: " + proc->getName());
    vector<set<ThreeAddressNodePtr>> Yset = proc->getY();
    list<ThreeAddressNodePtr> nodelist;
    std::copy(proc->getBlock().begin(), proc->getBlock().end(), std::back_inserter(nodelist));

    vector<ThreeAddressNodePtr> functionCall;
    map<ThreeAddressNodePtr, vector<ActualPara>> mapCallToArguments;
    map<ThreeAddressNodePtr, vector<ThreeAddressNodePtr>> mapCallToReturnValues;
    map<ThreeAddressNodePtr, set<ThreeAddressNodePtr>> mapFuncCallArgumentsToCalls;
    map<ActualPara, set<ThreeAddressNodePtr>> mapInputArgumentsToCalls;
    map<set<ThreeAddressNodePtr>, ThreeAddressNodePtr> myYsetToInternal;
    map<ThreeAddressNodePtr, set<ThreeAddressNodePtr>> myInternalToYset;

    proc->arrangeArguments(functionCall, mapCallToArguments, mapFuncCallArgumentsToCalls, mapInputArgumentsToCalls, mapCallToReturnValues);


    vector<ThreeAddressNodePtr> inlinedBlock;

    std::reverse(functionCall.begin(), functionCall.end());

    vector<set<ThreeAddressNodePtr>> finalY;

    vector<ThreeAddressNodePtr> finalOutputs = proc->getReturns();

    for (int p = 0; p < functionCall.size(); p++) {

        auto callInstruction = functionCall[p];
        string nodeName = callInstruction->getNodeName();
        cout << "check: " << nodeName << endl;
        string::size_type pos1 = callInstruction->getNodeName().find("@");
        string calleeName = callInstruction->getNodeName().substr(0, pos1);
        ProcedureHPtr callee = nameToProc[calleeName];
        map<set<ThreeAddressNodePtr>, ThreeAddressNodePtr> newYsetToInternal;
        map<ThreeAddressNodePtr, set<ThreeAddressNodePtr>> newInternalToYset;
        map<string, ThreeAddressNodePtr> saved;
        map<ThreeAddressNodePtr, ThreeAddressNodePtr> savedForFunction;

        YAndType yAndType = gadget(callee, d);

        vector<set<ThreeAddressNodePtr>> newYg;


        vector<ThreeAddressNodePtr> newBlock;
        vector<ThreeAddressNodePtr> newReturns;


        if(p == 0){
            instantitateYg(callee, mapCallToArguments[callInstruction], callInstruction, newYg, newYsetToInternal, newInternalToYset,
                           newBlock, newReturns, saved, savedForFunction);
            inlinedBlock = newBlock;
            updateFinalY(finalY, callInstruction, newReturns);
            finalY.insert(finalY.end(), newYg.begin(), newYg.end());
            myInternalToYset = newInternalToYset;
            myYsetToInternal = newYsetToInternal;
            finalOutputs = updateFinalOutput(finalOutputs, callInstruction, newReturns);
            continue;
        }

        if(calleeName == "preshare") {
            instantitateYg(callee, mapCallToArguments[callInstruction], callInstruction, newYg, newYsetToInternal, newInternalToYset,
                           newBlock, newReturns, saved, savedForFunction);
            inlinedBlock.insert(inlinedBlock.begin(), newBlock.begin(), newBlock.end());
            updateFinalY(finalY, callInstruction, newReturns);
            updateInternalToYAndYsetToInternal(callInstruction, newReturns, myYsetToInternal, myInternalToYset, newYsetToInternal, newInternalToYset);
            continue;
        }


        bool abortOccurs = false;
        if(yAndType.second == CheckType::SNI || yAndType.second == CheckType::WSNI) {

            bool res = tryOptimization(finalY, callInstruction);
            if(!res) {
                try {
                    set<ThreeAddressNodePtr> I1;
                    set<ThreeAddressNodePtr> X;
                    for (auto ele: finalY) {
                        for (auto ele1: ele) {
                            if ((ele1->getOp() == ASTNode::Operator::CALL && ele1->getLhs() != callInstruction) ||
                                ele1->getNodeType() == NodeType::PARAMETER) {

                                I1.insert(ele1);
                            } else if (ele1->getOp() == ASTNode::Operator::CALL && ele1->getLhs() == callInstruction) {
                                X.insert(ele1);
                            }
                        }
                    }
                    set<ThreeAddressNodePtr> keys(mapCallToReturnValues[callInstruction].begin(), mapCallToReturnValues[callInstruction].end());


                    for(int i = 1; i <= d - 1; i++) {
                        WorkList workList;
                        workList.push_back(NumberSubsetPair(i, finalY));
                        int count = 0;
                        exploreMain(i, workList, vector<vector<ThreeAddressNodePtr>>(), keys, CheckType::SNI, count);
                    }


                } catch (set<ThreeAddressNodePtr> O) {
                    for(auto ele : O) {
                        cout << ele->prettyPrint5() << endl;
                    }
                    abortOccurs = true;
                }

            }

            if((res || !abortOccurs) && (yAndType.second == CheckType::SNI ||
                                         (yAndType.second == CheckType::WSNI && weakSNIToCannotCheck[callee].empty()))) {
                instantitateYg(callee, mapCallToArguments[callInstruction], callInstruction, newYg,
                               newYsetToInternal, newInternalToYset,
                               newBlock, newReturns, saved, savedForFunction);
                inlinedBlock.insert(inlinedBlock.begin(), newBlock.begin(), newBlock.end());
                updateFinalY(finalY, callInstruction, newReturns);
                updateInternalToYAndYsetToInternal(callInstruction, newReturns, myYsetToInternal, myInternalToYset,
                                                   newYsetToInternal, newInternalToYset);
                finalY.insert(finalY.end(), newYg.begin(), newYg.end());
                finalOutputs = updateFinalOutput(finalOutputs, callInstruction, newReturns);
            }

            if(!abortOccurs && yAndType.second == CheckType::WSNI && !weakSNIToCannotCheck[callee].empty()) {
                instantitateYg(callee, mapCallToArguments[callInstruction], callInstruction, newYg, newYsetToInternal, newInternalToYset,
                               newBlock, newReturns, saved, savedForFunction);
                inlinedBlock.insert(inlinedBlock.begin(), newBlock.begin(), newBlock.end());
                updateFinalY(finalY, callInstruction, newReturns);
                auto tempFinalY = finalY;
                finalY.insert(finalY.end(), newYg.begin(), newYg.end());
                set<set<ThreeAddressNodePtr >> fromY;
                for(auto ele : tempFinalY) fromY.insert(ele);
                updateInternalToYAndYsetToInternal(callInstruction, newReturns, myYsetToInternal, myInternalToYset, newYsetToInternal, newInternalToYset);
                finalOutputs = updateFinalOutput(finalOutputs, callInstruction, newReturns);
                for(auto candidate : weakSNIToCannotCheck[callee]) {
                    set<ThreeAddressNodePtr> returnSet;
                    set<ThreeAddressNodePtr> internalSet;
                    if (calleeName != "XOR") {
                        vector<set<ThreeAddressNodePtr>> X;
                        for (auto ele: candidate) {
                            assert(ele->getOp() != ASTNode::Operator::PUSH);
                            if(ele->getNodeName().find("res") != ele->getNodeName().npos)
                                returnSet.insert(ele);
                            else {
                                internalSet.insert(ele);
                                X.push_back(set<ThreeAddressNodePtr>{ele});
                            }
                        }
                        WorkList workList;
                        workList.push_back(NumberSubsetPair(internalSet.size(), X));
                        workList.push_back(NumberSubsetPair(returnSet.size(), tempFinalY));

                        int count = 0;
                        explore(callee, d, workList, finalY, CheckType::NI, myYsetToInternal, myInternalToYset,
                                count, fromY);
                        totalChecked += count;
                        compChecked += count;
                    }
                }
            }


        }
        if(abortOccurs || yAndType.second == CheckType::NI) {
            instantitateYg(callee, mapCallToArguments[callInstruction], callInstruction, newYg, newYsetToInternal, newInternalToYset,
                           newBlock, newReturns, saved, savedForFunction);
            inlinedBlock.insert(inlinedBlock.begin(), newBlock.begin(), newBlock.end());
            updateFinalY(finalY, callInstruction, newReturns);
            auto tempFinalY = finalY;
            finalY.insert(finalY.end(), newYg.begin(), newYg.end());
            set<set<ThreeAddressNodePtr >> fromY;
            for(auto ele : tempFinalY) fromY.insert(ele);
            updateInternalToYAndYsetToInternal(callInstruction, newReturns, myYsetToInternal, myInternalToYset, newYsetToInternal, newInternalToYset);
            finalOutputs = updateFinalOutput(finalOutputs, callInstruction, newReturns);
            if(calleeName != "XOR") {
                vector<set<ThreeAddressNodePtr>> X;
                for (auto ele : newBlock) {
                    assert(ele->getOp() != ASTNode::Operator::PUSH);
                    X.push_back(set<ThreeAddressNodePtr>{ele});
                }
                for (int i = 1; i < d; i++) {
                    cout << X.size() << ", " << tempFinalY.size() << "." << endl;
                    WorkList workList;
                    workList.push_back(NumberSubsetPair(i, X));
                    workList.push_back(NumberSubsetPair(d - i, tempFinalY));

                    int count = 0;
                    explore(callee, d, workList, finalY, CheckType::NI, myYsetToInternal, myInternalToYset, count, fromY);
                    totalChecked += count;
                    compChecked += count;
                }
            }
        }

        cout << "Y size: " << finalY.size() << endl;
        set<set<ThreeAddressNodePtr>> tempSet(finalY.begin(), finalY.end());
        finalY.clear();
        for(auto ele : tempSet)
            finalY.push_back(ele);
        cout << "Y size: " << finalY.size() << endl;

        cout << "==================================" << endl;
    }
    procaToInlinedBlock[proc] = inlinedBlock;
    procToOutputs[proc] = finalOutputs;
    return YAndType(finalY, CheckType::NI);
}


void HOChecker::replaceByNewReturns(ThreeAddressNodePtr node, ThreeAddressNodePtr callInstruction, const vector<ThreeAddressNodePtr>& newReturns, int& count, set<ThreeAddressNodePtr>& tempSaved) {
    if(!node)
        return;
    if(tempSaved.count(node) != 0) {
        return;
    }
    if(node->getOp() == ASTNode::Operator::CALL && node->getLhs() == callInstruction) {
        count++;
        ThreeAddressNodePtr corresponding = newReturns.at(node->getIndexCall());
        for(auto ele : node->getParents()) {
            if(!ele.expired()) {
                if (ele.lock()->getLhs() == node) {
                    ele.lock()->setLhs(corresponding);
                } else if(ele.lock()->getRhs() == node){
                    ele.lock()->setRhs(corresponding);
                }
            }
        }
    }
    replaceByNewReturns(node->getLhs(), callInstruction, newReturns, count, tempSaved);
    replaceByNewReturns(node->getRhs(), callInstruction, newReturns, count, tempSaved);
    tempSaved.insert(node);
}

void HOChecker::replaceByNewReturnsByName(ThreeAddressNodePtr node, string path, const map<string, ThreeAddressNodePtr>& newReturns,
                                          int& count, set<ThreeAddressNodePtr>& tempSaved, map<ThreeAddressNodePtr, ThreeAddressNodePtr>& record) {
    if(!node)
        return;
    if(tempSaved.count(node) != 0) {
        return;
    }
    if(node->getNodeName().find("@") == node->getNodeName().npos) {
        return;
    }
    string currentPath = node->getNodeName().substr(node->getNodeName().find("@"), node->getNodeName().find("(") - node->getNodeName().find("@"));
    if(currentPath == path) {
        count++;
        ThreeAddressNodePtr corresponding = newReturns.at(node->getNodeName().substr(0, node->getNodeName().find("@")));
        record[node] = corresponding;
        for(auto ele : node->getParents()) {
            if(!ele.expired()) {
                if (ele.lock()->getLhs() == node) {
                    ele.lock()->setLhs(corresponding);
                } else if(ele.lock()->getRhs() == node){
                    ele.lock()->setRhs(corresponding);
                }
                record[ele.lock()] = nullptr;
            }
        }
    } else {
        replaceByNewReturnsByName(node->getLhs(), path, newReturns, count, tempSaved, record);
        replaceByNewReturnsByName(node->getRhs(), path, newReturns, count, tempSaved, record);
    }
    tempSaved.insert(node);
}


void HOChecker::updateInternalToYAndYsetToInternal(const ThreeAddressNodePtr& callInstruction,
                                                     const vector<ThreeAddressNodePtr>& newReturns,
                                                     map<set<ThreeAddressNodePtr>, ThreeAddressNodePtr>& myYsetToInternal,
                                                     map<ThreeAddressNodePtr, set<ThreeAddressNodePtr>>& myInternalToY,
                                                     map<set<ThreeAddressNodePtr>, ThreeAddressNodePtr>& newYsetToInternal,
                                                     map<ThreeAddressNodePtr, set<ThreeAddressNodePtr>>& newInternalToYset
){

    map<set<ThreeAddressNodePtr>, ThreeAddressNodePtr> needTobeRemoved;
    map<set<ThreeAddressNodePtr>, ThreeAddressNodePtr> needTobeAdded;
    set<ThreeAddressNodePtr > tempSaved;
    // YsetToInternal发生改变
    for (auto ele : myYsetToInternal) {
        set<ThreeAddressNodePtr> needTobeRemovedSet;
        set<ThreeAddressNodePtr> needTobeAddedSet;
        for (auto ele1 : ele.first) {
            if (ele1->getOp() == ASTNode::Operator::CALL && ele1->getLhs() == callInstruction) {
                // 获取到相应的返回值
                ThreeAddressNodePtr corresponding = newReturns.at(ele1->getIndexCall());
                needTobeRemovedSet.insert(ele1);
                needTobeAddedSet.insert(corresponding);
                // 替换过去
            } else {
                int count = 0;
                replaceByNewReturns(ele1, callInstruction, newReturns, count, tempSaved);
            }
        }
        if (needTobeAddedSet.empty() && needTobeRemovedSet.empty()) {
            continue;
        } else {
            set<ThreeAddressNodePtr> temp = ele.first;
            for (auto ele1 : needTobeRemovedSet) {
                temp.erase(ele1);
            }
            int count = 0;
            replaceByNewReturns(ele.second, callInstruction, newReturns, count, tempSaved);
            temp.insert(needTobeAddedSet.begin(), needTobeAddedSet.end());
            needTobeRemoved.insert(ele);
            needTobeAdded[temp] = ele.second;
        }

    }

    clock_t s2 = clock();


    for (auto ele : needTobeRemoved) {
        assert(myYsetToInternal.count(ele.first) != 0);
        myYsetToInternal.erase(ele.first);
    }
    for (auto ele : needTobeAdded) {
        myYsetToInternal[ele.first] = ele.second;
    }

    for(auto ele : newYsetToInternal) {
        if(myYsetToInternal.count(ele.first) != 0)
         myYsetToInternal[ele.first] = ele.second;
    }



    map<ThreeAddressNodePtr, set<ThreeAddressNodePtr>> needTobeRemoved1;
    map<ThreeAddressNodePtr, set<ThreeAddressNodePtr>> needTobeAdded1;
    for (auto ele : myInternalToY) {
        set<ThreeAddressNodePtr> needTobeRemovedSet;
        set<ThreeAddressNodePtr> needTobeAddedSet;
        for (auto ele1 : ele.second) {
            if (ele1->getOp() == ASTNode::Operator::CALL && ele1->getLhs() == callInstruction) {
                ThreeAddressNodePtr corresponding = newReturns.at(ele1->getIndexCall());
                needTobeRemovedSet.insert(ele1);
                needTobeAddedSet.insert(corresponding);
            } else {
                int count = 0;
                replaceByNewReturns(ele1, callInstruction, newReturns, count, tempSaved);
            }
        }
        if (needTobeAddedSet.empty() && needTobeRemovedSet.empty()) {
            continue;
        } else {
            auto temp = ele.second;
            for (auto ele1 : needTobeRemovedSet) {
                temp.erase(ele1);
            }
            int count = 0;
            replaceByNewReturns(ele.first, callInstruction, newReturns, count, tempSaved);
            temp.insert(needTobeAddedSet.begin(), needTobeAddedSet.end());
            needTobeRemoved1.insert(ele);
            needTobeAdded1[ele.first] = temp;
        }

    }

    for (auto ele : needTobeRemoved1) {
        assert(myInternalToY.count(ele.first) != 0);
        myInternalToY.erase(ele.first);
    }
    for (auto ele : needTobeAdded1) {
        myInternalToY[ele.first] = ele.second;
    }
    for(auto ele : newInternalToYset) {
        myInternalToY.insert(ele);
    }
}
void HOChecker::updateFinalY(vector<set<ThreeAddressNodePtr>>& finalY, const ThreeAddressNodePtr& callInstruction,
                               const vector<ThreeAddressNodePtr>& newReturns){
    set<ThreeAddressNodePtr > tempSaved;
    for(auto& ele : finalY) {
        set<ThreeAddressNodePtr> needTobeRemoved;
        set<ThreeAddressNodePtr> needTobeAdded;
        int numberOfCalls = 0;
        int count = 0;
        for(auto& ele1 : ele) {
            if(ele1->getOp() == ASTNode::Operator::CALL && ele1->getLhs() == callInstruction) {
                ThreeAddressNodePtr corresponding = newReturns.at(ele1->getIndexCall());
                needTobeRemoved.insert(ele1);
                needTobeAdded.insert(corresponding);
                numberOfCalls++;
            } else {
                replaceByNewReturns(ele1, callInstruction, newReturns, count, tempSaved);
            }

        }

        for(auto ele1 : needTobeRemoved) {
            ele.erase(ele1);
        }
        ele.insert(needTobeAdded.begin(), needTobeAdded.end());
    }
}

vector<ThreeAddressNodePtr> HOChecker::updateFinalOutput(vector<ThreeAddressNodePtr>& finalOutput, const ThreeAddressNodePtr& callInstruction,
                                  const vector<ThreeAddressNodePtr>& newReturns) {
    set<ThreeAddressNodePtr > tempSaved;
    int count = 0;
    vector<ThreeAddressNodePtr> res;
    for(auto& ele : finalOutput) {
        if(ele->getOp() == ASTNode::Operator::CALL && ele->getLhs() == callInstruction) {
            ThreeAddressNodePtr corresponding = newReturns.at(ele->getIndexCall());
            res.push_back(corresponding);
        } else {
            replaceByNewReturns(ele, callInstruction, newReturns, count, tempSaved);
            res.push_back(ele);
        }
    }
    return res;
}

ThreeAddressNodePtr HOChecker::instantitate(ProcedureHPtr proc, string basename, ThreeAddressNodePtr node, map<ThreeAddressNodePtr, ThreeAddressNodePtr>& formalToActual, map<string, ThreeAddressNodePtr>& saved, map<ThreeAddressNodePtr, ThreeAddressNodePtr >& savedForFunction, string path, set<ThreeAddressNodePtr>& randomBackup) {

    if(!node)
        return nullptr;

    if(formalToActual.count(node) != 0) {
        saved[node->getNodeName()] = formalToActual[node];
        return formalToActual[node];
    } else if(node->getNodeName() != "push" && saved.count(node->getNodeName()) != 0) {
        return saved[node->getNodeName()];
    } else if(node->getNodeName() == "push" && saved.count(node->getNodeName()) != 0 &&
        saved[node->getNodeName()]->getLhs()->getNodeName() == node->getLhs()->getNodeName())  {
        return saved[node->getNodeName()];
    } else if(savedForFunction.count(node) != 0) {
        return savedForFunction[node];
    }
    else {
        ThreeAddressNodePtr left = instantitate(proc, basename, node->getLhs(), formalToActual, saved, savedForFunction, path, randomBackup);
        ThreeAddressNodePtr right = instantitate(proc, basename, node->getRhs(), formalToActual, saved, savedForFunction, path, randomBackup);

        ThreeAddressNodePtr newNode = make_shared<ThreeAddressNode>(node->getNodeName() + path + "(" + basename + ")", left, right, node->getOp(), node->getNodeType());
        if(newNode->getNodeType() == NodeType::RANDOM){ //&& proc->getName() != "preshare" && proc->getName() != "sbox" && newNode->getNodeName().find("#") == newNode->getNodeName().npos) {
            randomBackup.insert(node);
        }

        if(left && left->getNodeType() == NodeType::FUNCTION)
            newNode->setIndexCall(node->getIndexCall());

        if(left)
            left->addParents(newNode);
        if(right)
            right->addParents(newNode);

        if(node->getNodeName() != "push" && saved.count(node->getNodeName()) != 0)
            assert(false);
        if(node->getNodeType() == NodeType::FUNCTION) {
            savedForFunction[node] = newNode;
        } else {
            saved[node->getNodeName()] = newNode;
        }

        return newNode;
    }
}

vector<ThreeAddressNodePtr> HOChecker::instantitateReturnByComputation(ProcedureHPtr proc, ThreeAddressNodePtr callInstruction,map<string, ThreeAddressNodePtr>& saved, map<ThreeAddressNodePtr, ThreeAddressNodePtr>& savedForFunction,
                                                                         map<ThreeAddressNodePtr, ThreeAddressNodePtr>& formalToActual, string path, set<ThreeAddressNodePtr>& backup  ){
    vector<ThreeAddressNodePtr> newReturns;
    auto returns = proc->getReturns();
    if(returns[0]->getOp() == ASTNode::Operator::CALL) {
        assert(procToOutputs.count(proc) != 0);
        returns = procToOutputs[proc];
    } else if(procToOutputs.count(proc) != 0) {
        returns = procToOutputs[proc];
    }

    for(auto ele: returns) {
        if(ele->getOp() == ASTNode::Operator::CALL) {
            assert(false);
        }
        if(saved.count(ele->getNodeName()) != 0)
            newReturns.push_back(saved[ele->getNodeName()]);
        else {
            ThreeAddressNodePtr temp = instantitate(proc, callToNodeName[callInstruction], ele, formalToActual, saved, savedForFunction,
                                                    path, backup);
            newReturns.push_back(temp);
        }
    }
    return newReturns;

}

vector<ThreeAddressNodePtr> HOChecker::instantitateBlockByComputation(ProcedureHPtr proc, ThreeAddressNodePtr callInstruction,map<string, ThreeAddressNodePtr>& saved, map<ThreeAddressNodePtr, ThreeAddressNodePtr>& savedForFunction,
                                                                        map<ThreeAddressNodePtr, ThreeAddressNodePtr>& formalToActual, string path, set<ThreeAddressNodePtr>& backup ){
    vector<ThreeAddressNodePtr> newBlock;
    assert(procaToInlinedBlock.count(proc) != 0);
    for(auto ele: procaToInlinedBlock[proc]) {
        if(ele->getNodeName() != "push" && saved.count(ele->getNodeName()) != 0)
            newBlock.push_back(saved[ele->getNodeName()]);
        else {
            ThreeAddressNodePtr temp = instantitate(proc, callToNodeName[callInstruction], ele, formalToActual, saved, savedForFunction,
                                                    path, backup);
            newBlock.push_back(temp);
        }
    }
    return newBlock;

}


void HOChecker::instantitateYg2(const ProcedureHPtr proc, const vector<ActualPara>& actualArguments, const ThreeAddressNodePtr& callInstruction,
                               vector<set<ThreeAddressNodePtr>>& newYg,
                               vector<ThreeAddressNodePtr>& newBlock, vector<ThreeAddressNodePtr>& newReturns,
                               map<string, ThreeAddressNodePtr>& saved, map<ThreeAddressNodePtr, ThreeAddressNodePtr>& savedForFunction) {
    YAndType yAndType = storedInfo[proc];
    const vector<set<ThreeAddressNodePtr>>& Yg = yAndType.first;
    string::size_type pos1 = callInstruction->getNodeName().find('@');
    string path = callInstruction->getNodeName().substr(pos1, callInstruction->getNodeName().size());
    map<ThreeAddressNodePtr, ThreeAddressNodePtr> formalToActual;
    for(int i = 0; i < proc->getParameters().size(); i++) {
        ActualPara temp = actualArguments[i];
        auto parameters = proc->getParameters()[i];
        for(int j = 0; j < parameters.size(); j++) {
            formalToActual[parameters[j]] = temp.first[j];
        }
    }

    set<ThreeAddressNodePtr> backupInYg;
    for(auto ele : Yg) {
        set<ThreeAddressNodePtr> temp;
        for(auto ele1 : ele) {
            auto node = instantitate(proc, callToNodeName[callInstruction], ele1, formalToActual, saved, savedForFunction, path, backupInYg);
            temp.insert(node);
        }
        newYg.push_back(temp);
    }


    set<ThreeAddressNodePtr> backup;
    newBlock = instantitateBlockByComputation(proc, callInstruction, saved, savedForFunction, formalToActual, path, backup);
    newReturns = instantitateReturnByComputation(proc, callInstruction, saved, savedForFunction, formalToActual, path, backup);

    set<ThreeAddressNodePtr> localRandoms;
    for(auto ele : newBlock) {
        if(ele->getNodeType() == NodeType::RANDOM)
            localRandoms.insert(ele);
    }

}
void HOChecker::instantitateYg(const ProcedureHPtr proc, const vector<ActualPara>& actualArguments, const ThreeAddressNodePtr& callInstruction,
                                 vector<set<ThreeAddressNodePtr>>& newYg,
                                 map<set<ThreeAddressNodePtr>, ThreeAddressNodePtr>& newYsetToInternal,
                                 map<ThreeAddressNodePtr, set<ThreeAddressNodePtr>>& newInternalToYset,
                                 vector<ThreeAddressNodePtr>& newBlock, vector<ThreeAddressNodePtr>& newReturns,
                                 map<string, ThreeAddressNodePtr>& saved, map<ThreeAddressNodePtr, ThreeAddressNodePtr>& savedForFunction) {
    YAndType yAndType = storedInfo[proc];
    const vector<set<ThreeAddressNodePtr>>& Yg = yAndType.first;
    string::size_type pos1 = callInstruction->getNodeName().find('@');
    string path = callInstruction->getNodeName().substr(pos1, callInstruction->getNodeName().size());
    map<ThreeAddressNodePtr, ThreeAddressNodePtr> formalToActual;
    for(int i = 0; i < proc->getParameters().size(); i++) {
        ActualPara temp = actualArguments[i];
        auto parameters = proc->getParameters()[i];
        for(int j = 0; j < parameters.size(); j++) {
            formalToActual[parameters[j]] = temp.first[j];
        }
    }
    set<ThreeAddressNodePtr> backupInYg;
    for(auto ele : Yg) {
        set<ThreeAddressNodePtr> temp;
        for(auto ele1 : ele) {
            auto node = instantitate(proc, callToNodeName[callInstruction], ele1, formalToActual, saved, savedForFunction, path, backupInYg);
            temp.insert(node);
        }
        newYg.push_back(temp);
    }
    const map<set<ThreeAddressNodePtr>, ThreeAddressNodePtr>& formalYsetToInternal = YsetToInternal[proc];
    const map<ThreeAddressNodePtr, set<ThreeAddressNodePtr>>& formalInternalToYset = internalToY[proc];

    for(auto ele : formalYsetToInternal) {
        set<ThreeAddressNodePtr> first;
        set<ThreeAddressNodePtr> backup;
        for(auto ele1 : ele.first) {
            first.insert(instantitate(proc, callToNodeName[callInstruction], ele1, formalToActual, saved, savedForFunction, path, backup));
        }
        auto second = instantitate(proc, callToNodeName[callInstruction], ele.second, formalToActual, saved, savedForFunction,path, backup);
        newYsetToInternal[first] = second;
    }

    for(auto ele : formalInternalToYset) {

        set<ThreeAddressNodePtr> backup;
        set<ThreeAddressNodePtr> second;
        for(auto ele1 : ele.second) {
            second.insert(instantitate(proc, callToNodeName[callInstruction], ele1, formalToActual, saved, savedForFunction, path , backup));
        }
        auto first = instantitate(proc, callToNodeName[callInstruction], ele.first, formalToActual, saved, savedForFunction, path, backup);
        newInternalToYset[first] = second;
    }

    set<ThreeAddressNodePtr> backup;
    newBlock = instantitateBlockByComputation(proc, callInstruction, saved, savedForFunction, formalToActual, path, backup);
    newReturns = instantitateReturnByComputation(proc, callInstruction, saved, savedForFunction, formalToActual, path, backup);

    set<ThreeAddressNodePtr> localRandoms;
    for(auto ele : newBlock) {
        if(ele->getNodeType() == NodeType::RANDOM)
            localRandoms.insert(ele);
    }

}

bool HOChecker::tryOptimization(const vector<set<ThreeAddressNodePtr>>& finalY, ThreeAddressNodePtr callInstruction) {
    bool fflag = true;
    for(auto ele : finalY) {
        map<ThreeAddressNodePtr, map<int, set<ThreeAddressNodePtr>>> temp;
        set<ThreeAddressNodePtr> tempSet;
        for(auto ele1 : ele) {
            if(ele1->getOp() != ASTNode::Operator::CALL) {
                set<ThreeAddressNodePtr> saved;
                set<ThreeAddressNodePtr> res;
                findCallInstruction(ele1, callInstruction, saved, res);
                for(auto callIns : res) {
                    temp[callIns->getLhs()][callIns->getIndexCall() / (order + 1)].insert(ele1);
                    if (temp[callIns->getLhs()][callIns->getIndexCall() / (order + 1)].size() >= 2) {
                        fflag = false;
                    }
                }
            } else {
                temp[ele1->getLhs()][ele1->getIndexCall() / (order + 1)].insert(ele1);
                if (temp[ele1->getLhs()][ele1->getIndexCall() / (order + 1)].size() >= 2) {
                    fflag = false;
                }
            }
        }
        if(!fflag)
            break;
    }
    return fflag;
}
void HOChecker::findCallInstruction(ThreeAddressNodePtr node, ThreeAddressNodePtr callInstruction, set<ThreeAddressNodePtr>& saved,
                                      set<ThreeAddressNodePtr>& res) {
    if(!node)
        return;
    if(saved.count(node) != 0)
        return;

    findCallInstruction(node->getLhs(), callInstruction, saved, res);
    findCallInstruction(node->getRhs(), callInstruction, saved, res);

    if(node->getLhs() == callInstruction)
        res.insert(node);

    saved.insert(node);


}




map<string, vector<int>> HOChecker::nameToNumberOfInputs;

bool HOChecker::isTest = false;
bool HOChecker::stopDom = false;

const map<ProcedureHPtr, YAndType> &HOChecker::getStoredInfo() const {
    return storedInfo;
}
