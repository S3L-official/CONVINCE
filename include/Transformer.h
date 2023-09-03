//
// Created by Pengfei Gao on 2022/2/24.
//

#ifndef COMAVE_TRANSFORMER_H
#define COMAVE_TRANSFORMER_H

#include "Value.h"
#include "list"
#include "ProcedureH.h"





class Transformer {
private:
    vector<ProcValuePtr> procedures;
    vector<ProcedureHPtr> procedureHs;
public:
    const vector<ProcedureHPtr> &getProcedureHs() const;

private:
    map<string, ProcedureHPtr> nameToProc;

public:
    Transformer(const vector<ProcValuePtr>& procedures) {
        this->procedures = procedures;
    }

    void transformProcedures();
    ThreeAddressNodePtr
    transformRandom(RandomValuePtr randomValuePtr, map<ValuePtr, ThreeAddressNodePtr> &saved,
                    map<string, int> &nameCount);

    ThreeAddressNodePtr
    transformInternalBinForShare(InternalBinValuePtr internalBinValuePtr, map<ValuePtr, ThreeAddressNodePtr> &saved,
                                 map<string, int> &nameCount);

    ThreeAddressNodePtr
    transformInternalBin(InternalBinValuePtr internalBinValuePtr, map<ValuePtr, ThreeAddressNodePtr> &saved,
                         map<string, int> &nameCount);

    ThreeAddressNodePtr
    transformInternalUn(InternalUnValuePtr internalUnValuePtr, map<ValuePtr, ThreeAddressNodePtr> &saved,
                        map<string, int> &nameCount);

    vector<ThreeAddressNodePtr>
    transformArrayValue(ArrayValuePtr arrayValuePtr, map<ValuePtr, ThreeAddressNodePtr> &saved,
                        map<string, int> &nameCount, bool&);

    ProcedureHPtr transform(ProcValuePtr procValuePtr);

    string getCount(string name, map<string, int> &nameCount);
};


#endif //COMAVE_TRANSFORMER_H
