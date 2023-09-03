//
// Created by Pengfei Gao on 2022/4/4.
//

#include "Inliner.h"

void Inliner::inlineProcedures() {
    for(auto ele : procedures) {
        if(ele->isSimple1()) {
            nameToInlinedProcedure[ele->getName()] = ele;
            nameToNumberOfProcedureCalls[ele->getName()] = 1;
        } else {
            vector<ThreeAddressNodePtr> functionCall;
            map<ThreeAddressNodePtr, vector<ActualPara>> mapCallToArguments;
            map<ThreeAddressNodePtr, vector<ThreeAddressNodePtr>> mapCallToReturnValues;
            map<ThreeAddressNodePtr, set<ThreeAddressNodePtr>> mapFuncCallArgumentsToCalls;
            map<ActualPara, set<ThreeAddressNodePtr>> mapInputArgumentsToCalls;

            map<ThreeAddressNodePtr, ThreeAddressNodePtr> mapReturnToInlined;
            for(auto ele1 : ele->getParameters()) {
                for(auto ele2 : ele1) {
                    mapReturnToInlined[ele2] = ele2;
                }
            }

            vector<ThreeAddressNodePtr> inlinedBlock;
            vector<ThreeAddressNodePtr> inlinedReturn;

            int numberOfProcedureCalls = 0;

            ele->arrangeArguments(functionCall, mapCallToArguments, mapFuncCallArgumentsToCalls, mapInputArgumentsToCalls, mapCallToReturnValues);
            auto callToNodeName = ele->getCallToNodeName();

            for (int p = 0; p < functionCall.size(); p++) {

                auto callInstruction = functionCall[p];
                string nodeName = callInstruction->getNodeName();

                string::size_type pos1 = callInstruction->getNodeName().find("@");
                string calleeName = callInstruction->getNodeName().substr(0, pos1);
                ProcedureHPtr callee = nameToInlinedProcedure[calleeName];
                assert(nameToNumberOfProcedureCalls.count(callee->getName()) != 0);
                numberOfProcedureCalls += nameToNumberOfProcedureCalls[callee->getName()];



                map<string, ThreeAddressNodePtr> saved;
                map<ThreeAddressNodePtr, ThreeAddressNodePtr> savedForFunction;
                map<ThreeAddressNodePtr, ThreeAddressNodePtr> savedForCall;
                vector<ThreeAddressNodePtr> newBlock;
                vector<ThreeAddressNodePtr> newReturn;

                auto actualArguments = mapCallToArguments[callInstruction];
                for(auto& ele : actualArguments) {
                    vector<ThreeAddressNodePtr> temp;
                    for(auto ele1 : ele.first) {
                        if(mapReturnToInlined.count(ele1)!=0) {
                            temp.push_back(mapReturnToInlined[ele1]);
                        } else {
                            temp.push_back(ele1);
                            if(callee->getName() == "XOR_pub") {
                                inlinedBlock.push_back(ele1);
                            }
                        }
                    }
                    ele.first = temp;
                }

                instantitateProcedure(callee, actualArguments, callInstruction,
                                newBlock, newReturn, saved, savedForFunction, savedForCall, callToNodeName);

                for(int i = 0; i < mapCallToReturnValues[callInstruction].size(); i++){
                    mapReturnToInlined[mapCallToReturnValues[callInstruction][i]] = newReturn[i];
                }
                inlinedBlock.insert(inlinedBlock.end(), newBlock.begin(), newBlock.end());

                mapCallToInlinedNode[callInstruction] = savedForCall;

            }

            if(ele->getName() != "main") {
                for (auto ele1: ele->getReturns()) {
                    assert(mapReturnToInlined.count(ele1) != 0);
                    inlinedReturn.push_back(mapReturnToInlined[ele1]);

                }

                if (inlinedReturn.size() != ele->getReturns().size()) {
                    assert(false);
                }
            }


            auto newProc = ProcedureHPtr(new ProcedureH(ele->getName(), ele->getParameters(),
                                                        inlinedBlock, inlinedReturn, ele->getNameToProc()));


            nameToInlinedProcedure[ele->getName()] = newProc;

            nameToNumberOfProcedureCalls[ele->getName()] = numberOfProcedureCalls + 1;

        }

    }

}


void Inliner::instantitateProcedure(const ProcedureHPtr proc, const vector<ActualPara>& actualArguments, const ThreeAddressNodePtr& callInstruction,
                                vector<ThreeAddressNodePtr>& newBlock, vector<ThreeAddressNodePtr>& newReturn,
                                map<string, ThreeAddressNodePtr>& saved, map<ThreeAddressNodePtr, ThreeAddressNodePtr>& savedForFunction,
                                map<ThreeAddressNodePtr, ThreeAddressNodePtr>& savedForCall,
                                const map<ThreeAddressNodePtr, string>& callToNodeName) {
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


    set<ThreeAddressNodePtr> backup;
    for(auto ele: nameToInlinedProcedure[proc->getName()]->getBlock()) {
        if(ele->getNodeName() != "push" && saved.count(ele->getNodeName()) != 0)
            newBlock.push_back(saved[ele->getNodeName()]);
        else {
            ThreeAddressNodePtr temp = instantitateNode(proc, callToNodeName.at(callInstruction), ele, formalToActual, saved, savedForFunction, savedForCall,
                                                    path, backup);
            newBlock.push_back(temp);
        }
    }
    for(auto ele: nameToInlinedProcedure[proc->getName()]->getReturns()) {
        if(ele->getNodeName() != "push" && saved.count(ele->getNodeName()) != 0)
            newReturn.push_back(saved[ele->getNodeName()]);
        else {
            ThreeAddressNodePtr temp = instantitateNode(proc, callToNodeName.at(callInstruction), ele, formalToActual, saved, savedForFunction, savedForCall,
                                                        path, backup);
            newReturn.push_back(temp);
        }
    }

}

ThreeAddressNodePtr Inliner::instantitateNode(ProcedureHPtr proc, string basename, ThreeAddressNodePtr node,
                                              map<ThreeAddressNodePtr, ThreeAddressNodePtr>& formalToActual, map<string, ThreeAddressNodePtr>& saved, map<ThreeAddressNodePtr, ThreeAddressNodePtr >& savedForFunction,
                                              map<ThreeAddressNodePtr, ThreeAddressNodePtr>& savedForCall,
                                              string path, set<ThreeAddressNodePtr>& randomBackup) {
    if(!node)
        return nullptr;

    if(formalToActual.count(node) != 0) {
        saved[node->getNodeName()] = formalToActual[node];
        savedForCall[node] = formalToActual[node];
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
        ThreeAddressNodePtr left = instantitateNode(proc, basename, node->getLhs(), formalToActual, saved, savedForFunction, savedForCall, path, randomBackup);
        ThreeAddressNodePtr right = instantitateNode(proc, basename, node->getRhs(), formalToActual, saved, savedForFunction, savedForCall, path, randomBackup);

        ThreeAddressNodePtr newNode = make_shared<ThreeAddressNode>(node->getNodeName() + path + "(" + basename + ")", left, right, node->getOp(), node->getNodeType());
        if(newNode->getNodeType() == NodeType::RANDOM){
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
            savedForCall[node] = newNode;
        }

        return newNode;
    }
}

const map<string, ProcedureHPtr> &Inliner::getNameToInlinedProcedure() const {
    return nameToInlinedProcedure;
}

const map<ThreeAddressNodePtr, map<ThreeAddressNodePtr, ThreeAddressNodePtr>> &
Inliner::getMapCallToInlinedNode() const {
    return mapCallToInlinedNode;
}

vector<vector<ThreeAddressNodePtr>> Inliner::getInlinedParameters(ProcedureHPtr callee, ThreeAddressNodePtr callInstruction) {
    vector<vector<ThreeAddressNodePtr>> realParameters;
    for(auto ele : callee->getParameters()) {
        vector<ThreeAddressNodePtr> temp;
        for(auto ele1 : ele) {
            temp.push_back(mapCallToInlinedNode.at(callInstruction).at(ele1));
        }
        realParameters.push_back(temp);
    }
    return realParameters;
}

vector<ThreeAddressNodePtr> Inliner::getInlinedReturns(ProcedureHPtr callee, ThreeAddressNodePtr callInstruction) {
    vector<ThreeAddressNodePtr> key;
    for(auto ele : nameToInlinedProcedure.at(callee->getName())->getReturns()) {
        key.push_back(mapCallToInlinedNode.at(callInstruction).at(ele));
    }

    return key;
}

void Inliner::printPython() {
    vector<string> res;
    set<string> randoms;
    for(auto ele : nameToInlinedProcedure) {
        if(ele.first == "main") {
            for(auto ele1 : ele.second->getBlock()) {
                string temp = ele1->prettyPrint4();
                replace(temp.begin(), temp.end(), '@', '_');
                replace(temp.begin(), temp.end(), '(', '_');
                replace(temp.begin(), temp.end(), ')', '_');
                res.push_back(temp);
            }
        }
    }

    int i = 0;
    int index = 0;
    int share_count = 0;
    int numberOfInput = 32;
    int numberOfVerif = 1;
    map<string, int> variable_table;

    while(i < res.size() && i < numberOfInput * (order + 1)) {
        int j = 0;
        vector<string> shares;
        string keyName = "key_" + to_string(share_count);
        cout << keyName << " = symbol('" << keyName + "', 'S', 8)" << endl;
        while(j < order + 1) {
            vector<string> tokens = Utils::split(res[i], ' ');
            if(tokens.size() == 1) {
                // random shares
                cout << tokens[0] << " = symbol('" << tokens[0] << "', 'M', 8)" << endl;
                shares.push_back(tokens[0]);
            } else if(tokens.size() == 5) {
                // the last share
                cout << tokens[0] << " = " << shares[0];
                for(int i = 1; i < shares.size(); i++) {
                    cout << " ^ " << shares[i];
                }
                cout << " ^ " << keyName;
                cout << endl;
            } else {
                assert(false);
            }
            i = i + 1;
            index = index + 1;
            j = j + 1;
        }
        share_count = share_count + 1;
    }

    int total = 0;
    int tempi = i;
    while(tempi < res.size()) {
        vector<string> tokens = Utils::split(res[tempi], ' ');
        if(tokens.size() == 1) {
            tempi = tempi + 1;
        } else {
            total++;
            tempi = tempi + 1;
        }
    }

    while(i < res.size()) {
        vector<string> tokens = Utils::split(res[i], ' ');
        if(tokens.size() == 1) {
            if(randoms.count(tokens[0]) == 0) {
                cout << tokens[0] << " = symbol('" << tokens[0] << "', 'M', 8)" << endl;
                randoms.insert(tokens[0]);
            }
            index++;
            i = i + 1;
        } else {
            if(tokens.size() == 4) {
                if(tokens[2] == "POW2") {
                    cout << tokens[0] << " = " << tokens[3] << " * " << tokens[3] << endl;
                    cout << tokens[0] << " = " << "verif(" << tokens[0] << ", " << numberOfVerif << ", " << total << ")" << endl;
                } else if(tokens[2] == "POW4"){
                    cout << tokens[0] << " = " << tokens[3];
                    for(int i = 0; i < 3; i++) {
                        cout << " * " << tokens[3];
                    }
                    cout << endl;
                    cout << tokens[0] << " = " << "verif(" << tokens[0] << ", " << numberOfVerif << ", " << total << ")" << endl;
                } else if(tokens[2] == "POW16") {
                    cout << tokens[0] << " = " << tokens[3];
                    for(int i = 0; i < 15; i++) {
                        cout << " * " << tokens[3];
                    }
                    cout << endl;
                    cout << tokens[0] << " = " << "verif(" << tokens[0] << ", " << numberOfVerif << ", " << total << ")" << endl;
                } else if(tokens[2] == "AFFINE"){
                    cout << tokens[0] << " = " << tokens[3] << endl;
                    cout << tokens[0] << " = " << "verif(" << tokens[0] << ", " << numberOfVerif << ", " << total << ")" << endl;
                } else if(tokens[2] == "XTIMES"){
                    cout << tokens[0] << " = " << tokens[3] << endl;
                    cout << tokens[0] << " = " << "verif(" << tokens[0] << ", " << numberOfVerif << ", " << total << ")" << endl;
                } else if(tokens[2] == "TRCON"){
                    cout << tokens[0] << " = constant(1, 8)" << endl;
                } else {
                    assert(false);
                }
            } else if(tokens.size() == 5){
                if (tokens[3] == "^" || tokens[3] == "*") {
                    cout << tokens[0] << " = " << tokens[2] << " " << tokens[3] << " " << tokens[4] << endl;
                    cout << tokens[0] << " = " << "verif(" << tokens[0] << ", " << numberOfVerif << ", " << total << ")" << endl;
                } else {
                    assert(false);
                }
            } else {
                assert(false);
            }
            variable_table[tokens[0]] = index;
            index++;
            i++;
            numberOfVerif++;
        }
    }
    cout << "print_results()" << endl;
}

//string Inliner::toStringMaskVerif(map<ValuePtr, string>& nameMap, ValuePtr value) {
//    if(ParameterValue* ptr = dynamic_cast<ParameterValue*>(value.get())) {
//        return nameMap[value];
//    } else if(RandomValue* random = dynamic_cast<RandomValue*>(value.get())) {
//        return nameMap[value] + " = $distr;";
//    } else if(InternalBinValue* bin = dynamic_cast<InternalBinValue*>(value.get())) {
//        if(bin->getOp() == ASTNode::Operator::ADD)
//            return  nameMap[value] + " = ![" + nameMap[bin->getLeft()] + " + " + nameMap[bin->getRight()] + "];";
//        else if(bin->getOp() == ASTNode::Operator::XOR)
//            return nameMap[value] + " = " + nameMap[bin->getLeft()] + " ^ " + nameMap[bin->getRight()] + ";";
//        else if(bin->getOp() == ASTNode::Operator::FFTIMES)
//            return nameMap[value] + " := " + nameMap[bin->getLeft()] + " * " + nameMap[bin->getRight()] + ";";
//        else
//            assert(false);
//    } else {
//        return "null";
//    }
//}

void Inliner::printNetlist() {
    vector<string> res;
    for(auto ele : nameToInlinedProcedure) {
        if(ele.first == "main") {
            for(auto ele1 : ele.second->getBlock()) {
                res.push_back(ele1->prettyPrint4());
            }
        }
    }

    int i = 0;
    int index = 0;
    int share_count = 0;
    int numberOfInput = 8;
    map<string, int> variable_table;

    while(i < res.size() && i < numberOfInput * (order + 1)) {
        int j = 0;
        while(j < order + 1) {
            vector<string> tokens = Utils::split(res[i], ' ');
            cout << "in " << index << " " << share_count << "_" << j << endl;
            i = i + 1;
            variable_table[tokens[0]] = index;
            index = index + 1;
            j = j + 1;
        }
        share_count = share_count + 1;
    }
    while(i < res.size()) {
        vector<string> tokens = Utils::split(res[i], ' ');
        if(tokens.size() == 1) {
            variable_table[tokens[0]] = index;
            cout << "ref " << index << endl;
            index++;
            i = i + 1;
        } else {
            if(tokens.size() == 4) {
                if(tokens[2] == "~") {
                    cout << "not " << variable_table[tokens[3]] << endl;
                } else {
                    assert(false);
                }
            } else {
                if (tokens[3] == "^") {
                    cout << "xor " << variable_table[tokens[2]] << " " << variable_table[tokens[4]] << endl;
                } else if (tokens[3] == "*") {
                    cout << "and " << variable_table[tokens[2]] << " " << variable_table[tokens[4]] << endl;
                } else {
                    assert(false);
                }
            }
            variable_table[tokens[0]] = index;
            index++;
            i++;
        }
    }
}

void Inliner::printMaskverif() {
    for(auto ele : nameToInlinedProcedure) {
        if(ele.first == "main") {
            for(auto ele1 : ele.second->getBlock()) {
                string temp = ele1->prettyPrint4();
                replace(temp.begin(), temp.end(), '@', '_');
                replace(temp.begin(), temp.end(), '(', '_');
                replace(temp.begin(), temp.end(), ')', '_');
                if(ele1->getNodeType() == NodeType::RANDOM) {
                    cout << temp << " = $distr;" << endl;
                } else {
                    cout << temp << ";" << endl;
                }
            }
        }
    }
}