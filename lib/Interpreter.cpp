//
// Created by Pengfei Gao on 2020/2/17.
//

#include "Interpreter.h"


bool Interpreter::isHD = false;
int Interpreter::assumptionLevel = 0;
vector<string> Interpreter::functionName;

ValuePtr ASTNode::NBlock::compute(Interpreter &interpreter) {
    vector<ValuePtr> block;
    ValuePtr last = nullptr;
    for(auto stmt : *(this->getStmtList())) {
        if(stmt->getTypeName() == "random")
            cout << "hello" << endl;
        last = stmt->compute(interpreter);
        if(last && last->getValueType() == ValueType::VTProcValue)
            interpreter.addProc(last);
        else if(last && last->getValueType() == ValueType::VTPsiAssValue)
            interpreter.addAssumptions(last);
        block.push_back(last);
    }
    return last;
}

ValuePtr ASTNode::NVariableDeclaration::compute(Interpreter &interpreter) {
    if(this->type->isArray) {
        int arraySize = 1;
        vector<int> arrayBounds;

        bool needToDivide = false;
        for(auto bound : *(this->type->arraySize)) {
            ValuePtr ident = bound->compute(interpreter);
            if(NIdentifierPtr identifier = dynamic_pointer_cast<NIdentifier>(bound)) {
                if(identifier->getName() == "n" && !arrayBounds.empty()) {
                    needToDivide = false;
                }
            }
            int valueOfBound = interpreter.value_of(ident);
            arrayBounds.push_back(valueOfBound);
            arraySize *= valueOfBound;
        }

        if(needToDivide) {
            int newArraySize = 1;
            vector<int> newArrayBounds;
            newArrayBounds.push_back(arrayBounds[arrayBounds.size() - 1]);
            for(int i = 0; i < arrayBounds.size() - 1; i++)
                newArraySize *= arrayBounds[i];
            vector<ValuePtr> finalValues;
            for(int i = 0; i < newArraySize; i++) {
                interpreter.setArraySize(this->id->getName() + to_string(i), newArrayBounds);

                vector<ValuePtr> values;
                for (int j = 0; j < newArrayBounds[0]; j++) {
                    ValuePtr valuePtr = interpreter.getValue(this->type->getName(), this->id->getName() + to_string(i) + to_string(j),
                                                             this->isParameter);
                    values.push_back(valuePtr);
                }
                ValuePtr re = make_shared<ArrayValue>(this->id->getName() + to_string(i), values);
                interpreter.addToEnv(this->id->getName() + to_string(i), re);
                finalValues.push_back(re);

                if (this->isParameter) {
                    interpreter.addParameter(re);
                }


                if (this->assignmentExpr) {
                    NAssignment assignment(this->id, this->assignmentExpr);
                    return assignment.compute(interpreter);
                }
            }

            ValuePtr re = make_shared<ArrayValue>(this->id->getName(), finalValues);
            interpreter.addToEnv(this->id->getName(), re);

            if (this->isParameter) {
                interpreter.addParameter(re);
            }

        } else {
            interpreter.setArraySize(this->id->getName(), arrayBounds);

            vector<ValuePtr> values;
            for (int i = 0; i < arraySize; i++) {
                ValuePtr valuePtr = interpreter.getValue(this->type->getName(), this->id->getName() + to_string(i),
                                                         this->isParameter);
                values.push_back(valuePtr);
            }
            ValuePtr re = make_shared<ArrayValue>(this->id->getName(), values);
            interpreter.addToEnv(this->id->getName(), re);

            if (this->isParameter) {
                interpreter.addParameter(re);
            }


            if (this->assignmentExpr) {
                NAssignment assignment(this->id, this->assignmentExpr);
                return assignment.compute(interpreter);
            }
        }

        return nullptr;
    }
    else {
        string name = this->id->getName();
        ValuePtr value = interpreter.getValue(this->type->getName(), name, this->isParameter);
        interpreter.addToEnv(name, value);

        if(this->isParameter) {
            interpreter.addParameter(value);
        }

        if(this->type->getTypeName() == "random") {
            interpreter.addToSequence(value);
        }

        if(this->assignmentExpr) {
            assert(interpreter.getFromEnv(name) == nullptr);
            NAssignment assignment(this->id, this->assignmentExpr);
            return assignment.compute(interpreter);
        }

        return nullptr;
    }

}


ValuePtr ASTNode::NFunctionDeclaration::compute(Interpreter &interpreter) {
    string functionName = this->id->getName();

    interpreter.pushBlock();

    for(auto pa : *(this->var_list)) {
        pa->compute(interpreter);
    }

    vector<ValuePtr> parameters = interpreter.getParameter();

    if(this->id->getName() == "main") {
        assert(parameters.size() == (this->var_list)->size());
        for(int i = 0; i < (this->var_list)->size(); i++) {
            auto pa = (this->var_list)->at(i);
            if(pa->getMyType() == "private") {
                if(ArrayValuePtr arrayValuePtr = dynamic_pointer_cast<ArrayValue>(parameters[i])) {
                    for(auto ele : arrayValuePtr->getArrayValue())
                        interpreter.addNameOfKey(ele->getName());
                } else if(ParameterValuePtr parameterValue = dynamic_pointer_cast<ParameterValue>(parameters[i])){
                    interpreter.addNameOfKey(parameterValue->getName());
                } else {
                    assert(false);
                }
            } else if(pa->getMyType() == "public") {
                if(ArrayValuePtr arrayValuePtr = dynamic_pointer_cast<ArrayValue>(parameters[i])) {
                    for(auto ele : arrayValuePtr->getArrayValue())
                        interpreter.addNameOfPlain(ele->getName());
                } else if(ParameterValuePtr parameterValue = dynamic_pointer_cast<ParameterValue>(parameters[i])){
                    interpreter.addNameOfPlain(parameterValue->getName());
                } else {
                    assert(false);
                }
            }
        }
    }



    ValuePtr body = this->block->compute(interpreter);

    ValuePtr returns = interpreter.getCurrentReturnValue();

    vector<ValuePtr> assumptions = interpreter.getAssumptions();

    if(Interpreter::assumptionLevel == 1 || Interpreter::assumptionLevel == 0) {
        assumptions.clear();
    } else if(Interpreter::assumptionLevel == 3) {
    } else if(Interpreter::assumptionLevel == 2) {
        bool flag = false;
        for(auto ele : Interpreter::functionName) {
            if(ele == functionName) {
                flag = true;
                break;
            }
        }
        if(!flag) {
            assumptions.clear();
        }
    }

    if(functionName == "preshare" || functionName == "preshare_public") {
        vector<ValuePtr>& sequence = interpreter.getSequence();
        for(auto it = sequence.begin(); it != sequence.end() - 1; ) {
            if(it->get()->getValueType() == ValueType::VTInternalBinValue) {
                it = sequence.erase( it );
            } else{
                ++it;
            }
        }
    }

    ProcedurePtr proc = make_shared<Procedure>(functionName, parameters,assumptions, interpreter.getEnv(),
                                               interpreter.getSequence(), returns, interpreter.getHDSequence());

    interpreter.addToIntenal(interpreter.getSequence().size());


    return make_shared<ProcValue>(functionName, proc);
}

ValuePtr ASTNode::NReturnStatement::compute(Interpreter &interpreter) {
    ValuePtr returns = this->expr->compute(interpreter);
    interpreter.setCurrentReturnValue(returns);
    return nullptr;
}

ValuePtr ASTNode::NIdentifier::compute(Interpreter &interpreter) {
    string nodeName = this->getName();

    ValuePtr valuePtr = interpreter.getFromEnv(nodeName);

    assert(valuePtr);

    return valuePtr;

}

ValuePtr ASTNode::NExpressionStatement::compute(Interpreter &interpreter) {
    return this->expr->compute(interpreter);
}


ValuePtr ASTNode::NAssignment::compute(Interpreter &interpreter) {

    ValuePtr expValue = this->RHS->compute(interpreter);

    if(interpreter.isHD) {
        ValuePtr oldValue = interpreter.getFromEnv(this->LHS->getName());
        if (oldValue) {
            ArrayValue* arrayValueOld = dynamic_cast<ArrayValue*>(oldValue.get());
            if(arrayValueOld) {
                ArrayValue* arrayValueNew = dynamic_cast<ArrayValue*>(expValue.get());
                for(int i = 0; i < arrayValueOld->getArrayValue().size(); i++) {
                    if(arrayValueOld->getValueAt(i)) {
                        ValuePtr hdValue = make_shared<InternalBinValue>("hd", arrayValueOld->getValueAt(i), arrayValueNew->getValueAt(i), ASTNode::Operator::XOR);
                        arrayValueOld->getValueAt(i)->addParents(hdValue);
                        arrayValueNew->getValueAt(i)->addParents(hdValue);
                        interpreter.addToHDSequence(hdValue);
                    }
                }
            } else {
                if(oldValue->getValueType() != ValueType::VTConcreteNumValue) {
                    ValuePtr hdValue = make_shared<InternalBinValue>("hd", expValue, oldValue, ASTNode::Operator::XOR);
                    expValue->addParents(hdValue);
                    oldValue->addParents(hdValue);
                    interpreter.addToHDSequence(hdValue);
                }
            }
        }
    }

    interpreter.addToEnv(this->LHS->getName(), expValue);

    if(expValue->getValueType() == ValueType::VTConcreteNumValue) {
        // ConcreteNumValue
    }else if(expValue->getValueType() == ValueType::VTInternalBinValue) {
//        cout << "added in computation: " << endl;
    }
    else if(expValue->getValueType() == ValueType::VTInternalUnValue) {
//        cout << "added in computation: " << endl;
    }
    else {
        interpreter.addToSequence(expValue);
    }

    return nullptr;

}

ValuePtr ASTNode::NBinaryOperator::compute(Interpreter &interpreter) {
    ValuePtr L = this->lhs->compute(interpreter);
    assert(L);
    ValuePtr R = this->rhs->compute(interpreter);
    assert(R);

    ValuePtr res = make_shared<InternalBinValue>("", L, R, this->op);

    bool isLeftConcreteNum = L->getValueType() == VTConcreteNumValue ? true : false;
    bool isRightConcreteNum = R->getValueType() == VTConcreteNumValue ? true : false;
    if(isLeftConcreteNum && isRightConcreteNum) {
         return make_shared<ConcreteNumValue>("", res->value_of(interpreter.getEnv()));

    }

    L->addParents(res);
    R->addParents(res);

    interpreter.addToSequence(res);
    return res;
}


ValuePtr ASTNode::NUnaryOperator::compute(Interpreter &interpreter) {
    ValuePtr rand = this->lhs->compute(interpreter);
    ValuePtr res = make_shared<InternalUnValue>("", rand, this->op);
    rand->addParents(res);

    interpreter.addToSequence(res);

    return res;
}


ValuePtr ASTNode::NArrayIndex::compute(Interpreter &interpreter) {
    string name = this->arrayName->getName();

    auto sizeVec = interpreter.getArraySize(name);

    ValuePtr array = interpreter.getFromEnv(name);
    ArrayValue *arrayV = dynamic_cast<ArrayValue *>(array.get());
    assert(arrayV);

    vector<ValuePtr> indexesSymbol = interpreter.computeIndexWithSymbol(name, this->dimons);

    bool indexHasSymbol = false;
    for(auto ele : indexesSymbol) {
        if(!ValueCommon::isNoParameter(ele))
            indexHasSymbol = true;
    }
    if(!indexHasSymbol) {
        vector<int> indexes;
        for (auto ele : indexesSymbol) {
            int res = ele->value_of(interpreter.getEnv());
            indexes.push_back(res);
        }

        if(indexes.size() == 1)
            return arrayV->getValueAt(indexes[0]);
        else {
            vector<ValuePtr> res;
            string returnName = name + to_string(indexes.at(0)) + "_" + to_string(indexes.at(indexes.size() - 1));
            for(auto ele : indexes)
                res.push_back(arrayV->getValueAt(ele));
            return make_shared<ArrayValue>(returnName, res);
        }
    } else {
        if(indexesSymbol.size() == 1) {
            shared_ptr<ArrayValueIndex> res = make_shared<ArrayValueIndex>(name, array, indexesSymbol[0]);
            assert(res->getArrayValuePtr()->getValueType() == ValueType::VTArrayValue);
            return res;
        } else {
            vector<ValuePtr> res;
            string returnName = name + "symbol_symbol";
            for(auto ele : indexesSymbol) {
                shared_ptr<ArrayValueIndex> temp = make_shared<ArrayValueIndex>(name, array, ele);
                assert(temp->getArrayValuePtr()->getValueType() == ValueType::VTArrayValue);
                res.push_back(temp);
            }
            return make_shared<ArrayValue>(returnName, res);
        }

    }




}


ValuePtr ASTNode::NIFStatement::compute(Interpreter &interpreter) {
    ValuePtr pred = this->predicate->compute(interpreter);
    bool predValue = interpreter.value_of(pred);
    if(predValue) {
        this->ifBlock->compute(interpreter);
    } else {
        if(this->elseBlock) {
            this->elseBlock->compute(interpreter);
        }
    }
    return nullptr;
}

ValuePtr ASTNode::NArrayAssignment::compute(Interpreter &interpreter) {

    ValuePtr right = this->expression->compute(interpreter);
    string name = this->arrayIndex->getArrayName()->getName();
    ValuePtr array = interpreter.getFromEnv(name);
    ArrayValue* arrayV = dynamic_cast<ArrayValue*>(array.get());
    assert(arrayV);

    vector<int> indexes = interpreter.computeIndex(name, this->arrayIndex->dimons);


    vector<ValuePtr> res;
    if(indexes.size() == 1) {
        int index = indexes[0];
        right->setName(name + to_string(index));
        for(int i = 0; i < arrayV->getArrayValue().size(); i++) {
            if(i == index) {
                res.push_back(right);

                if(Interpreter::isHD) {
                    ValuePtr oldValue = arrayV->getArrayValue()[i];
                    if(oldValue) {
                        ValuePtr hdValue = make_shared<InternalBinValue>("hd", right, oldValue, ASTNode::Operator::XOR);
                        right->addParents(hdValue);
                        oldValue->addParents(hdValue);
                        interpreter.addToHDSequence(hdValue);
                    }
                }
            }
            else {
                res.push_back(arrayV->getArrayValue()[i]);
            }
        }
    } else {
        ArrayValue* rightArray = dynamic_cast<ArrayValue*>(right.get());
        assert(rightArray);
        assert(rightArray->getArrayValue().size() == indexes.size());
        for(int i = 0; i < arrayV->getArrayValue().size(); i++) {
            if(i == indexes[0]) {
                for(int j = 0; j < rightArray->getArrayValue().size(); j++) {
                    if(Interpreter::isHD) {
                        ValuePtr oldValue = arrayV->getValueAt(j);
                        if(oldValue) {
                            ValuePtr hdValue = make_shared<InternalBinValue>("hd", rightArray->getValueAt(j), oldValue, ASTNode::Operator::XOR);
                            rightArray->getValueAt(j)->addParents(hdValue);
                            oldValue->addParents(hdValue);
                            interpreter.addToHDSequence(hdValue);
                        }
                    }
                    res.push_back(rightArray->getValueAt(j));
                    i++;
                }
                i = i - 1;
            } else
                res.push_back(arrayV->getArrayValue()[i]);
        }

    }


    string returnName = name + to_string(indexes[0]) + "_" + to_string(indexes[indexes.size() - 1]);
    interpreter.addToEnv(name, make_shared<ArrayValue>(returnName, res));

    if(right->getValueType() == ValueType::VTConcreteNumValue) {
        // ConcreteNumValue
    }else if(right->getValueType() == ValueType::VTInternalBinValue) {
//        cout << "added in computation: " << endl;
    }
    else if(right->getValueType() == ValueType::VTInternalUnValue) {
//        cout << "added in computation: " << endl;
    } else {
        interpreter.addToSequence(right);
    }

    return array;
}

ValuePtr ASTNode::NArrayInitialization::compute(Interpreter &interpreter) {

    this->declaration->compute(interpreter);

    NExpressionListPtr nExpressionListPtr = this->expressionList;
    vector<ValuePtr> result;
    for(int i = 0; i < nExpressionListPtr->size(); i++) {
        ValuePtr valuePtr = nExpressionListPtr->at(i)->compute(interpreter);
        result.push_back(valuePtr);
    }


    string arrayName = this->declaration->getId()->getName();
    interpreter.addToEnv(arrayName, make_shared<ArrayValue>(arrayName, result));

    return nullptr;
}

ValuePtr ASTNode::NForStatement::compute(Interpreter &interpreter) {
    ValuePtr intiValue = this->init->compute(interpreter);
    ValuePtr cond = this->condition->compute(interpreter);
    bool cont = interpreter.value_of(cond);
    while(cont) {
        this->block->compute(interpreter);
        ValuePtr inc = this->increase->compute(interpreter);
        cond = this->condition->compute(interpreter);
        cont = interpreter.value_of(cond);
    }

    return nullptr;
}

ValuePtr ASTNode::NInteger::compute(Interpreter &interpreter) {
    return make_shared<ConcreteNumValue>("", this->value);
}


ValuePtr ASTNode::NFunctionCall::compute(Interpreter &interpreter) {

    string functionName = this->ident->getName();

    vector<ValuePtr> realParameters;
    for (auto arg : *(this->arguments)) {
        ValuePtr argPtr = arg->compute(interpreter);
        realParameters.push_back(argPtr);
    }

    ValuePtr proc = interpreter.getProc(functionName);
    ProcedurePtr procedurePtr = nullptr;
    if(ProcValue* procValue = dynamic_cast<ProcValue*>(proc.get())) {
        procedurePtr = procValue->getProcedurePtr();
    } else {
        assert(false);
    }


    vector<int> callsites;
    callsites.push_back(this->callSite);
    string callsitesString = "";
    for (auto ele : callsites) callsitesString += "@" + to_string(ele);
    ProcCallValuePtr procCallValuePtr = make_shared<ProcCallValue>(procedurePtr->getProcName() + callsitesString,
                                                                   procedurePtr, realParameters, callsites);

    const ValuePtr& returnValue = procedurePtr->getReturns();
    ArrayValue* arrayValue = dynamic_cast<ArrayValue*>(returnValue.get());
    ValuePtr res;
    if(arrayValue) {
        vector<ValuePtr> returnArray;

        for (int i = 0; i < arrayValue->getArrayValue().size(); i++) {
            ValuePtr value = make_shared<ProcCallValueIndex>(procCallValuePtr->getName() + "_" + to_string(i),
                                                             procCallValuePtr, i);
            returnArray.push_back(value);
        }

        res = make_shared<ArrayValue>(procCallValuePtr->getName() + "_ret", returnArray);


        for (auto ele : returnArray) {
            procCallValuePtr->addParents(ele);
        }
    } else {
        res = make_shared<ProcCallValueIndex>(procCallValuePtr->getName() + "_" + to_string(0), procCallValuePtr, 0);

        procCallValuePtr->addParents(res);
    }

    for (auto ele : realParameters) {
        if (ArrayValue *array = dynamic_cast<ArrayValue *>(ele.get())) {
            for (auto ele1 : array->getArrayValue()) {
                if(ele1)
                    ele1->addParents(procCallValuePtr);
            }
        } else {
            ele->addParents(procCallValuePtr);
        }
    }

    return res;
}

ValuePtr ASTNode::NAssumeStatement::compute(Interpreter &interpreter) {
    vector<ValuePtr> assumptions;
    for(auto ele : *expressionListPtr) {
        assumptions.push_back(ele->compute(interpreter));
    }
    return make_shared<PsiAssValue>("", assumptions);
}


ValuePtr ASTNode::NPredicate::compute(Interpreter &interpreter) {
    return nullptr;
}

ValuePtr ASTNode::DomPredicate::compute(Interpreter &interpreter) {
    ValuePtr firstValue = this->first->compute(interpreter);
    ValuePtr secondValue = this->second->compute(interpreter);
    return make_shared<DomAssValue>("", firstValue, secondValue);
}

ValuePtr ASTNode::RvalPredicate::compute(Interpreter &interpreter) {
    ValuePtr firstValue = this->first->compute(interpreter);
    ValuePtr secondValue = this->second->compute(interpreter);
    return make_shared<RValueAssValue>("", firstValue, secondValue);
}


ValuePtr ASTNode::TypePredicate::compute(Interpreter &interpreter) {
    ValuePtr firstValue = this->first->compute(interpreter);
    string type = this->second->getName();
    Type stype;
    if(type == "tSI") {
        stype = Type::SI;
    } else if(type == "tLK") {
        stype = Type::LK;
    } else if(type == "tUF") {
        stype = Type::UF;
    } else {
        stype = Type::UK;
    }
    return make_shared<TypeAssValue>("", firstValue, stype);

}
