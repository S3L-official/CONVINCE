//
// Created by Pengfei Gao on 2022/4/4.
//

#ifndef COMAVE_INLINER_H
#define COMAVE_INLINER_H
#include "ProcedureH.h"

class Inliner {
private:
    vector<ProcedureHPtr> procedures;
    vector<ProcedureHPtr> inlinedProcedure;
    map<string, ProcedureHPtr> nameToInlinedProcedure;
    map<string, int> nameToNumberOfProcedureCalls;
    map<ThreeAddressNodePtr, map<ThreeAddressNodePtr, ThreeAddressNodePtr>> mapCallToInlinedNode;
    int order;

public:
    const map<ThreeAddressNodePtr, map<ThreeAddressNodePtr, ThreeAddressNodePtr>> &getMapCallToInlinedNode() const;
    const map<string, ProcedureHPtr> &getNameToInlinedProcedure() const;
    Inliner(const vector<ProcedureHPtr>& procedures, int order) : procedures(procedures), order(order){}
    void inlineProcedures();
    vector<ThreeAddressNodePtr> instantitateBlockByComputation(ProcedureHPtr proc, ThreeAddressNodePtr callInstruction,map<string, ThreeAddressNodePtr>& saved, map<ThreeAddressNodePtr, ThreeAddressNodePtr>& savedForFunction,
                                                                          map<ThreeAddressNodePtr, ThreeAddressNodePtr>& formalToActual, string path, set<ThreeAddressNodePtr>& backup );


    void instantitateProcedure(const ProcedureHPtr proc, const vector<ActualPara>& actualArguments, const ThreeAddressNodePtr& callInstruction,
                                        vector<ThreeAddressNodePtr>& newBlock, vector<ThreeAddressNodePtr>& newReturn,
                                        map<string, ThreeAddressNodePtr>& saved, map<ThreeAddressNodePtr, ThreeAddressNodePtr>& savedForFunction,
                                        map<ThreeAddressNodePtr, ThreeAddressNodePtr>& savedForCall,
                                        const map<ThreeAddressNodePtr, string>& callToNodeName);

    ThreeAddressNodePtr instantitateNode(ProcedureHPtr proc, string basename, ThreeAddressNodePtr node, map<ThreeAddressNodePtr, ThreeAddressNodePtr>& formalToActual, map<string, ThreeAddressNodePtr>& saved, map<ThreeAddressNodePtr, ThreeAddressNodePtr >& savedForFunction,
                                         map<ThreeAddressNodePtr, ThreeAddressNodePtr>& savedForCall,
                                         string path, set<ThreeAddressNodePtr>& randomBackup);

    vector<vector<ThreeAddressNodePtr>> getInlinedParameters(ProcedureHPtr callee, ThreeAddressNodePtr callInstruction);

    vector<ThreeAddressNodePtr> getInlinedReturns(ProcedureHPtr callee, ThreeAddressNodePtr callInstruction);


    void printNetlist();

    int getNumberOfProcs() {
        return procedures.size();
    }

    int getNumberOfProcCalls() {
        return nameToNumberOfProcedureCalls["main"];
    }

    void printPython();

    void printMaskverif();

    map<ThreeAddressNodePtr, map<ThreeAddressNodePtr, ThreeAddressNodePtr>> &getMutableMapCallToInlinedNode() {
        return mapCallToInlinedNode;
    }

};


#endif //COMAVE_INLINER_H
