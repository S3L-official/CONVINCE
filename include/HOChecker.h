//
// Created by Pengfei Gao on 2021/9/19.
//

#ifndef COMAVE_HOCHECKER_H
#define COMAVE_HOCHECKER_H

#include "ThreeAddressNode.h"
#include "list"
#include "StateH.h"
#include "exception"
#include "Transformer.h"
#include "Inliner.h"
#include "WLVerifier.h"
#include "GPUCounter.h"



typedef pair<bool, vector<set<ThreeAddressNodePtr>>> CheckResult;
typedef pair<vector<set<ThreeAddressNodePtr>>, CheckType> YAndType;
typedef pair<vector<set<ThreeAddressNodePtr>>, vector<set<ThreeAddressNodePtr>>> IORelation;
typedef pair<vector<set<ThreeAddressNodePtr>>, Type> YType;
typedef pair<vector<ThreeAddressNodePtr>, bool> ActualPara;
typedef map<vector<ThreeAddressNodePtr>, set<vector<ThreeAddressNodePtr>>> SourceOfRandomMap;
typedef pair<string, SourceOfRandomMap> NameAndSourceOfRandom;

class HOChecker {
public:

    const map<ProcedureHPtr, YAndType> &getStoredInfo() const;
    static map<string, vector<int>> nameToNumberOfInputs;
    set<set<ThreeAddressNodePtr>> cannotCheck;

    set<string> nameOfKey;

    set<string> nameOfPlain;

    static set<string> complexProcedureNames;
    static bool isTest;
    static bool stopDom;

    YAndType gadget(shared_ptr <ProcedureH> f, int d);
    YAndType gadget2(shared_ptr <ProcedureH> f, int d);


    void testTransfer(ProcedureHPtr f, SourceOfRandomMap& sourceOfRandomMap);

    void testSelfGenerate(ProcedureHPtr f, SourceOfRandomMap& sourceOfRandomMap);

    bool checkWhetherDom(ThreeAddressNodePtr newReturnNode, const set<ThreeAddressNodePtr>& actuals);

    void dealWithYt(vector<set<ThreeAddressNodePtr>> &Yset);
    vector<ThreeAddressNodePtr> updateFinalOutput(vector<ThreeAddressNodePtr>& finalOutput, const ThreeAddressNodePtr& callInstruction,
                                                             const vector<ThreeAddressNodePtr>& newReturns);

    void findCallInstruction(ThreeAddressNodePtr node, ThreeAddressNodePtr callInstruction, set<ThreeAddressNodePtr>& saved,
                                        set<ThreeAddressNodePtr>& res);

    bool tryOptimization(const vector<set<ThreeAddressNodePtr>>& finalY, ThreeAddressNodePtr callInstruction);

    vector<set<ThreeAddressNodePtr>> generateY(int i, const vector<vector<ThreeAddressNodePtr>>& inputs);
    void checkProbing(int d);


    void instantiateNewYg(YAndType& yAndType, ThreeAddressNodePtr callInstruction, vector<set<ThreeAddressNodePtr>>& newYg);


    set<ThreeAddressNodePtr> checkSingle(ThreeAddressNodePtr outputNode, const vector<set<ThreeAddressNodePtr>>& Y);

    IORelation gadgetFO(ProcedureHPtr f, SourceOfRandomMap& sourceOfRandom);

    void checkInlinedNode(ThreeAddressNodePtr node, map<string, ThreeAddressNodePtr>& nodeMap, set<ThreeAddressNodePtr>& saved);


    IORelation sgadget(ProcedureHPtr f);

    bool haveNoSupportsIn(const set<ThreeAddressNodePtr>& eleOfY, const set<ThreeAddressNodePtr>& returns);

    bool allhaveSupportsIn(const set<ThreeAddressNodePtr>& eleOfY, const set<ThreeAddressNodePtr>& returns);

    HOChecker(const vector<ProcedureHPtr> &procedures, int order, Inliner& inliner) :
            procedureHs(procedures), order(order), inliner(inliner) {

        for(auto ele : procedures) {
            nameToProc[ele->getName()] = ele;
        }

        nameToNumberOfInputs["KeyExpansion"] = vector<int>{16};
        nameToNumberOfInputs["AddRoundKey"] = vector<int>{16, 16};
        nameToNumberOfInputs["ShiftRows"] = vector<int>{16};
        nameToNumberOfInputs["MixColumns"] = vector<int>{16};
        nameToNumberOfInputs["Cipher"] = vector<int>{16, 16};

    }

    void checkNI(string, int d);
    void checkSNI(string, int d);
    void getCartesian(const vector<vector<ThreeAddressNodePtr>>& parameters, vector<set<ThreeAddressNodePtr>>& res);
    void checkProbingFOForProc(string name);

    void setNumberOfShares(int numberOfShares) {
        this->numberOfShares = numberOfShares;
    }


private:
    map<ProcedureHPtr, map<set<ThreeAddressNodePtr>, ThreeAddressNodePtr>> YsetToInternal;
    map<ProcedureHPtr, map<ThreeAddressNodePtr, set<ThreeAddressNodePtr>>> internalToY;
    map<ProcedureHPtr, vector<ThreeAddressNodePtr>> procToOutputs;
    map<string, ProcedureHPtr> nameToProc;
    vector<ProcedureHPtr> procedureHs;
    map<ProcedureHPtr, YAndType> storedInfo;

    Inliner inliner;


private:
    map<ThreeAddressNodePtr, string> callToNodeName;
    map<ProcedureHPtr, set<ThreeAddressNodePtr>> globalSourceOfRandomFromSelf;
    map<ProcedureHPtr, vector<ThreeAddressNodePtr>> procaToInlinedBlock;

    map<NameAndSourceOfRandom, IORelation> nameAndSourceOfRandomToIORelation;
    map<ThreeAddressNodePtr, vector<ThreeAddressNodePtr>> callInstructionToComputationOfOutputs;
    map<NameAndSourceOfRandom, SourceOfRandomMap> nameAndSourceOfRandomToOutputSourceOfRandom;
    int totalChecked = 0;
    int compChecked = 0;
    int singleChecked = 0;
    int numberOfShares;
    map<ProcedureHPtr, set<set<ThreeAddressNodePtr>>> weakSNIToCannotCheck;


    int order;

    void arrangeArguments(list<ThreeAddressNodePtr> &nodelist, const ProcedureHPtr &proc,
                          vector<ThreeAddressNodePtr> &functionCall,
                          map<ThreeAddressNodePtr, vector<ActualPara>> &mapCallToArguments,
                          map<ThreeAddressNodePtr, set<ThreeAddressNodePtr>> &mapFuncCallArgumentsToCalls,
                          map<ActualPara, set<ThreeAddressNodePtr>>& mapInputArgumentsToCalls,
                          map<ThreeAddressNodePtr, vector<ThreeAddressNodePtr>>& mapCallToReturnValues);

    void updateFinalY(vector<set<ThreeAddressNodePtr>> &finalY, const ThreeAddressNodePtr &callInstruction,
                      const vector<ThreeAddressNodePtr> &newReturns);

    YAndType sgadget(ProcedureHPtr f, int d, CheckType t);
    YAndType sgadget2(ProcedureHPtr f, int d, CheckType t);

    set<set<ThreeAddressNodePtr>> choose(const WorkList& worklist);

    bool check(int d, StateH &state, const set<string>& Ynames);

    void extend(StateH& state, WorkList worklist);

    vector<bool> emptyCheck(const vector<ActualPara>& realParas, const SourceOfRandomMap& sourceOfRandomMap);

    vector<bool> emptyCheckForSimple(const vector<ActualPara>& realParas, const SourceOfRandomMap& sourceOfRandomMap);

    vector<bool> emptyCheckForComplex(const vector<ActualPara>& realParas, const SourceOfRandomMap& sourceOfRandomMap);

    void updateSourceRandom(ProcedureHPtr callee, const vector<ActualPara>& actualParas, const vector<ThreeAddressNodePtr>& key, SourceOfRandomMap& sourceOfRandomMap, const SourceOfRandomMap&);
    void updateSourceRandomForSimple(ProcedureHPtr callee, const vector<ActualPara>& actualParas, const vector<ThreeAddressNodePtr>& key, SourceOfRandomMap& sourceOfRandomMap, const SourceOfRandomMap& updatedSourceOfRandomMap );
    void updateSourceRandomForComplex(ProcedureHPtr callee, const vector<ActualPara>& actualParas, const vector<ThreeAddressNodePtr>& key, SourceOfRandomMap& sourceOfRandomMap, const SourceOfRandomMap& updatedSourceOfRandomMap );

    void directlyUpdateInputForSimple(ProcedureHPtr callee, const vector<bool>& emptyCheckRes, IORelation& newIORelation, ThreeAddressNodePtr callInstruction,
                                const vector<ActualPara>& actualParas, const map<ThreeAddressNodePtr, set<ThreeAddressNodePtr>>& mapFuncCallArgumentsToCalls, const SourceOfRandomMap& sourceOfRandom, SourceOfRandomMap&);

    void directlyUpdateInputForComplex(ProcedureHPtr callee, const vector<bool>& emptyCheckRes, IORelation& newIORelation, ThreeAddressNodePtr callInstruction,
                             const vector<ActualPara>& actualParas, const map<ThreeAddressNodePtr, set<ThreeAddressNodePtr>>& mapFuncCallArgumentsToCalls, const SourceOfRandomMap& sourceOfRandom, SourceOfRandomMap&);

    void recomputeAndUpdateInput(ProcedureHPtr callee, const vector<bool>& emptyCheckRes, IORelation& newIORelation, ThreeAddressNodePtr callInstruction,
                                        const vector<ActualPara>& actualParas, const vector<ThreeAddressNodePtr>& key, const map<ThreeAddressNodePtr, set<ThreeAddressNodePtr>>& mapFuncCallArgumentsToCalls,
                                        const SourceOfRandomMap& oldSourceOfRandom, SourceOfRandomMap&);

    void recomputeAndUpdateInputForSimple(ProcedureHPtr callee, const vector<bool>& emptyCheckRes, IORelation& newIORelation, ThreeAddressNodePtr callInstruction,
                                 const vector<ActualPara>& actualParas, const vector<ThreeAddressNodePtr>& key, const map<ThreeAddressNodePtr, set<ThreeAddressNodePtr>>& mapFuncCallArgumentsToCalls,
                                 const SourceOfRandomMap& oldSourceOfRandom, SourceOfRandomMap&);

    void recomputeAndUpdateForComplex(ProcedureHPtr callee, const vector<bool> &emptyCheckRes,
                                                 IORelation &newIORelation, ThreeAddressNodePtr callInstruction,
                                                 const vector<ActualPara> &actualParas,
                                                 const vector<ThreeAddressNodePtr>& key,
                                                 const map<ThreeAddressNodePtr, set<ThreeAddressNodePtr>> &mapFuncCallArgumentsToCalls,
                                                 const SourceOfRandomMap& oldSourceOfRandom, SourceOfRandomMap& updatedSourceOfRandom);

    bool explore(ProcedureHPtr f , int d, WorkList workList, vector<set<ThreeAddressNodePtr>>& Y, CheckType t,
                                             map<set<ThreeAddressNodePtr>, ThreeAddressNodePtr> &YToInternal,
                                             map<ThreeAddressNodePtr, set<ThreeAddressNodePtr>> &internalToY, int& count,
                                             const set<set<ThreeAddressNodePtr>>& fromY) ;

    bool
    exploreAux(ProcedureHPtr f ,WorkList workList, vector<set<ThreeAddressNodePtr>>& Y, vector<Split> splits, vector<int> &enumration,
               WorkList &newWorklist, WorkList &temp, int &count, CheckType t, int d,
               map<set<ThreeAddressNodePtr>, ThreeAddressNodePtr> &YToInternal,
               map<ThreeAddressNodePtr, set<ThreeAddressNodePtr>> &internalToY,const set<set<ThreeAddressNodePtr>>& fromY);


    YAndType gadgetCalls(ProcedureHPtr proc, int d);
    YAndType gadgetCalls2(ProcedureHPtr proc, int d);
    YAndType gadgetCalls3(ProcedureHPtr proc, int d);



    IORelation gadgetCalls(ProcedureHPtr proc, SourceOfRandomMap& sourceOfRandom);

    void instantitateIO(const IORelation& oldIO, const ProcedureHPtr proc, const vector<ActualPara>& actualArguments, const ThreeAddressNodePtr& callInstruction,
                                   IORelation& newIORelation, const vector<bool> & emptyCheckRes, set<set<ThreeAddressNodePtr>>& newInput,
                                   map<string, ThreeAddressNodePtr>& saved,  map<ThreeAddressNodePtr, ThreeAddressNodePtr>& savedForFunction, const SourceOfRandomMap& sourceOfRandomMap, SourceOfRandomMap& sourceOfRandomMapUpdated);
    void instantitateIOForSimple(const IORelation& oldIO, const ProcedureHPtr proc, const vector<ActualPara>& actualArguments, const ThreeAddressNodePtr& callInstruction,
                                            IORelation& newIORelation, const vector<bool> & emptyCheckRes, set<set<ThreeAddressNodePtr>>& newInput,
                                            map<string, ThreeAddressNodePtr>& saved, map<ThreeAddressNodePtr, ThreeAddressNodePtr>& savedForFunction, const SourceOfRandomMap& sourceOfRandomMap, SourceOfRandomMap& sourceOfRandomMapUpdated);
    void instantitateIOForComplex(const IORelation& oldIO, const ProcedureHPtr proc, const vector<ActualPara>& actualArguments, const ThreeAddressNodePtr& callInstruction,
                                 IORelation& newIORelation, const vector<bool> & emptyCheckRes, set<set<ThreeAddressNodePtr>>& newInput,
                                 map<string, ThreeAddressNodePtr>& saved, map<ThreeAddressNodePtr, ThreeAddressNodePtr>& savedForFunction, const SourceOfRandomMap& sourceOfRandomMap, SourceOfRandomMap& sourceOfRandomMapUpdated);


    set<ThreeAddressNodePtr> instantitateIOForSet(ProcedureHPtr proc, string basename, ThreeAddressNodePtr node, map<ThreeAddressNodePtr, set<ThreeAddressNodePtr>>& formalToActual, map<string, ThreeAddressNodePtr>& saved, string path, set<ThreeAddressNodePtr>& randomBackup);

    void instantitateYg(const ProcedureHPtr proc, const vector<ActualPara> &actualArguments,
                        const ThreeAddressNodePtr &callInstruction, vector<set<ThreeAddressNodePtr>> &newYg,
                        map<set<ThreeAddressNodePtr>, ThreeAddressNodePtr> &newYsetToInternal,
                        map<ThreeAddressNodePtr, set<ThreeAddressNodePtr>> &newInternalToYset,
                        vector<ThreeAddressNodePtr> &newBlock, vector<ThreeAddressNodePtr> &newReturns,
                        map<string, ThreeAddressNodePtr> &saved, map<ThreeAddressNodePtr, ThreeAddressNodePtr>& savedForFunction);
    void instantitateYg2(const ProcedureHPtr proc, const vector<ActualPara> &actualArguments,
                        const ThreeAddressNodePtr &callInstruction, vector<set<ThreeAddressNodePtr>> &newYg,
                        vector<ThreeAddressNodePtr> &newBlock, vector<ThreeAddressNodePtr> &newReturns,
                        map<string, ThreeAddressNodePtr> &saved, map<ThreeAddressNodePtr, ThreeAddressNodePtr>& savedForFunction);
    vector<ThreeAddressNodePtr> instantitateBlockByComputation(ProcedureHPtr proc, ThreeAddressNodePtr callInstruction,map<string, ThreeAddressNodePtr>& saved, map<ThreeAddressNodePtr, ThreeAddressNodePtr>& savedForFunction,map<ThreeAddressNodePtr, ThreeAddressNodePtr>& formalToActual, string path, set<ThreeAddressNodePtr>& backup );
    vector<ThreeAddressNodePtr> instantitateReturnByComputation(ProcedureHPtr proc, ThreeAddressNodePtr callInstruction,map<string, ThreeAddressNodePtr>& saved, map<ThreeAddressNodePtr, ThreeAddressNodePtr>& savedForFunction,
                                                                           map<ThreeAddressNodePtr, ThreeAddressNodePtr>& formalToActual, string path, set<ThreeAddressNodePtr>& backup);
    ThreeAddressNodePtr instantitate(ProcedureHPtr proc, string basename, ThreeAddressNodePtr node, map<ThreeAddressNodePtr, ThreeAddressNodePtr>& formalToActual, map<string, ThreeAddressNodePtr>& saved,map<ThreeAddressNodePtr, ThreeAddressNodePtr >& savedForFunction, string path, set<ThreeAddressNodePtr>& randomBackup);
    void replaceByNewReturns(ThreeAddressNodePtr node, ThreeAddressNodePtr callInstruction, const vector<ThreeAddressNodePtr>& newReturns, int& count, set<ThreeAddressNodePtr>& tempSaved);
    vector<vector<ThreeAddressNodePtr>> dealWithPushArguments(const vector<ThreeAddressNodePtr> & actualArgument, ThreeAddressNodePtr callNode);
    bool isShare(ProcedureHPtr proc, vector<ThreeAddressNodePtr>& actualPara, map<ThreeAddressNodePtr, set<ThreeAddressNodePtr>>& rootcauseOfMine, set<ThreeAddressNodePtr>& savedShare);

    bool isSNIShare(ProcedureHPtr proc, vector<ThreeAddressNodePtr>& actualPara, map<ThreeAddressNodePtr, set<ThreeAddressNodePtr>>& rootcauseOfMine, set<ThreeAddressNodePtr>& savedShare);
    void buildActualParas(ProcedureHPtr proc, vector<ActualPara>& actualParas, const vector<ThreeAddressNodePtr>& actualArguments,
                                     ThreeAddressNodePtr first, map<ThreeAddressNodePtr, set<ThreeAddressNodePtr>>& rootcauseOfMine) ;

    void extendMain(StateH& state, WorkList worklist, const vector<vector<ThreeAddressNodePtr>>& Para);
    bool exploreMain(int d, WorkList workList, const vector<vector<ThreeAddressNodePtr>>& Para, const set<ThreeAddressNodePtr>&,CheckType t, int& count);

    bool exploreAuxMain(WorkList workList, const vector<vector<ThreeAddressNodePtr >>& Y, const set<ThreeAddressNodePtr >& keys, vector<Split> splits, vector<int>& enumration, WorkList& newWorklist, WorkList& temp, int& count, CheckType t,int d);
    bool checkMain(int d, StateH& state, const vector<vector<ThreeAddressNodePtr>>& Paras, CheckType checkType);

    ProcedureHPtr getProcedureFromCallInstruction(ThreeAddressNodePtr callInstruction);
    void updateInternalToYAndYsetToInternal(const ThreeAddressNodePtr& callInstruction,
                                                       const vector<ThreeAddressNodePtr>& newReturns,
                                                       map<set<ThreeAddressNodePtr>, ThreeAddressNodePtr>& myYsetToInternal,
                                                       map<ThreeAddressNodePtr, set<ThreeAddressNodePtr>>& myInternalToY,
                                                       map<set<ThreeAddressNodePtr>, ThreeAddressNodePtr>& newYsetToInternal,
                                                       map<ThreeAddressNodePtr, set<ThreeAddressNodePtr>>& newInternalToYset);


    ProcedureHPtr reconstructProc(ProcedureHPtr oldProc,ThreeAddressNodePtr callInstruction, const vector<ActualPara>& realParameters);
    void instantiateNewYgRandom(YAndType& yAndType, ThreeAddressNodePtr callInstruction, vector<set<ThreeAddressNodePtr>>& newYg);
    void replaceByNewReturnsByName(ThreeAddressNodePtr node, string path, const map<string, ThreeAddressNodePtr>& newReturns, int& count, set<ThreeAddressNodePtr>& tempSaved, map<ThreeAddressNodePtr, ThreeAddressNodePtr>& record);

    };





#endif //COMAVE_HOCHECKER_H
