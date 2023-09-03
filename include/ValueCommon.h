//
// Created by Pengfei Gao on 2020/4/11.
//

#ifndef SAFELANG_VALUECOMMON_H
#define SAFELANG_VALUECOMMON_H

#include "Value.h"

namespace ValueCommon {

    bool isFullComputation(ValuePtr valuePtr);

    bool isNoParameter(ValuePtr valuePtr);
    void isNoParameterHelper(ValuePtr valuePtr, bool& noPara, set<ValuePtr>& visited);

    bool isNoRandoms(ValuePtr valuePtr);
    void isNoRandomsHelper(ValuePtr valuePtr, bool& noRands, set<ValuePtr>& visited);

    bool isNoProcCall(ValuePtr valuePtr);
    void isNoProcCallHelper(ValuePtr valuePtr, bool& noProcCall, set<ValuePtr>& visited);

    bool isNoArrayIndex(ValuePtr valuePtr);
    void isNoArrayIndexHelper(ValuePtr valuePtr, bool& noProcCall, set<ValuePtr>& visited);

    ValuePtr compactExpression(ValuePtr valuePtr);


    bool containsProcCall(ValuePtr valuePtr, ValuePtr& res);

    void containsProcCallHelper(ValuePtr valuePtr, bool& flag, ValuePtr& res, set<ValuePtr>& visited);


    void isNoSameFunctionCall(ValuePtr valuePtr, ValuePtr target, bool& noSameFunctionCall, set<ValuePtr>& visited);


    void getProcCallNPara(ValuePtr valuePtr, set<ValuePtr>& visited, set<ProcedurePtr>& procs, set<ValuePtr>& parameters);

    bool noSameCall(ValuePtr left, ValuePtr right);
}

#endif //SAFELANG_VALUECOMMON_H
