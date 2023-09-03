//
// Created by Pengfei Gao on 2019-09-05.
//

#include "IRCodeGen.h"

int globalCount = 0;

void IRCodeGenContext::removeInsFromInstructionSet(string functionName, ThreeAddressNodePtr node) {
    vector<ThreeAddressNodePtr>& instructions = this->getFunction(functionName)->instructions;
    for(auto it = instructions.begin(); it!=instructions.end(); ) {
        if(*it == node)
            it = instructions.erase(it);
        else
            ++it;
    }
}

void IRCodeGenContext::ToSSA(string functionName) {
    map<string, int> nameCounter;
    for(auto ins : getFunction(functionName)->instructions) {

        ins->prettyPrint();
        if(ins->getLhs() == nullptr && ins->getRhs() == nullptr && ins->getNodeType() != NodeType::RANDOM)
            continue;
        if(ins->getNodeType() == NodeType::INTERNAL || ins->getNodeType() == NodeType::PRESHARE) {
            string nodeName = ins->getNodeName();
            string lhsName = ins->getLhs()->getNodeName();

            if(nameCounter.count(lhsName) != 0 && nameCounter[lhsName] > 0) {
                ins->getLhs()->setSsaIndex(nameCounter[lhsName]);
            } else {
                ins->getLhs()->setSsaIndex(0);
                nameCounter[lhsName] = 0;
            }

            if(ins->getRhs()) {
                string rhsName = ins->getRhs()->getNodeName();
                if (nameCounter.count(rhsName) != 0 && nameCounter[rhsName] > 0) {
                    ins->getRhs()->setSsaIndex(nameCounter[rhsName]);
                } else {
                    ins->getRhs()->setSsaIndex(0);
                    nameCounter[rhsName] = 0;
                }
            }

            if(nameCounter.count(nodeName) != 0) {
                int newCount = nameCounter[nodeName] + 1;
                nameCounter[nodeName] = newCount;
                ins->setSsaIndex(newCount);
            } else {
                nameCounter[nodeName] = 0;
            }


        } else if(ins->getNodeType() == NodeType::RANDOM) {
            string nodeName = ins->getNodeName();
            if(nameCounter.count(nodeName) != 0) {
                int newCount = nameCounter[nodeName] + 1;
                nameCounter[nodeName] = newCount;
                ins->setSsaIndex(newCount);
            } else {
                nameCounter[nodeName] = 0;
            }
        }

    }
}
ThreeAddressNodePtr IRCodeGenContext::declareVariable(string type, string name) {
    ThreeAddressNodePtr re = nullptr;
    if(type == "private") {
        re = make_shared<ThreeAddressNode>(name, nullptr, nullptr, ASTNode::Operator::NULLOP, NodeType::PRIVATE);
    } else if(type == "public") {
        re = make_shared<ThreeAddressNode>(name, nullptr, nullptr, ASTNode::Operator::NULLOP, NodeType::PUBLIC);
    } else if(type == "internal") {
        re = make_shared<ThreeAddressNode>(name, nullptr, nullptr, ASTNode::Operator::NULLOP, NodeType::INTERNAL);
    } else if(type == "int") {
        re = make_shared<ThreeAddressNode>(name, nullptr, nullptr, ASTNode::Operator::NULLOP, NodeType::INT);
    } else if(type == "constant") {
        re = make_shared<ThreeAddressNode>(name, nullptr, nullptr, ASTNode::Operator::NULLOP, NodeType::CONSTANT);
    }
    else {
        re = make_shared<ThreeAddressNode>(name, nullptr, nullptr, ASTNode::Operator::NULLOP, NodeType::UNKNOWN);
    }
    return re;

}


void IRCodeGenContext::prettyPrint(string functionName) {
    cout << "Pretty Print of " << functionName << ":" << endl;
    for(auto ins : getFunction(functionName)->instructions) {
        ins->prettyPrint();
    }
}
void IRCodeGenContext::addArrayRangeAssignment(vector<ThreeAddressNodePtr> arrayValue) {
    arrayAssignmentStack.push(arrayValue);
}

vector<ThreeAddressNodePtr> IRCodeGenContext::popArrayRangeAssignment() {
    vector<ThreeAddressNodePtr> temp = arrayAssignmentStack.top();
    arrayAssignmentStack.pop();
    return temp;
}

int IRCodeGenContext::evaluateExpr(ThreeAddressNodePtr expr, IRCodeGenContext& context) {
    if(expr->getNodeType() == NodeType::CONSTANT) {
        return atoi(expr->getNodeName().c_str());
    }

    if(expr->getLhs() == nullptr && expr->getRhs() == nullptr) {
        return context.getSymbolConcreteValue(expr->getNodeName());
    }

    if(expr->getOp() == ASTNode::Operator::ADD) {
        int left = evaluateExpr(expr->getLhs(), context);
        int right = evaluateExpr(expr->getRhs(), context);
        int res = left + right;
        return res;
    }

    if(expr->getOp() == ASTNode::Operator::MINUS) {
        return evaluateExpr(expr->getLhs(), context) - evaluateExpr(expr->getRhs(), context);
    }

    if(expr->getOp() == ASTNode::Operator::FFTIMES) {
        return evaluateExpr(expr->getLhs(), context) * evaluateExpr(expr->getRhs(), context);
    }

    else {
        assert(false);
    }

}
bool IRCodeGenContext::evaluateCond(ThreeAddressNodePtr expr, IRCodeGenContext& context) {

    if(expr->getOp() == ASTNode::LE) {
        int lhs = evaluateExpr(expr->getLhs(), context);
        int rhs = evaluateExpr(expr->getRhs(), context);
        return lhs <= rhs;
    } else if(expr->getOp() == ASTNode::NE) {
        int lhs = evaluateExpr(expr->getLhs(), context);
        int rhs = evaluateExpr(expr->getRhs(), context);
        return lhs != rhs;
    }
    else if(expr->getOp() == ASTNode::LAND) {
        bool lhs = evaluateCond(expr->getLhs(), context);
        bool rhs = evaluateCond(expr->getRhs(), context);
        return lhs && rhs;
    } else {
        assert(false);
        return false;
    }


}

void IRCodeGenContext::generateCode(ASTNode::NBlock& root) {
    pushBlock();
    root.threeAddCodeGen(*this);
    popBlock();
}


vector<ThreeAddressNodePtr> IRCodeGenContext::copyInstructionSet(string functionName, int callSite,
        vector<ThreeAddressNodePtr>& parameters, vector<ThreeAddressNodePtr>& returnVariables) {

    map<string, ThreeAddressNodePtr> nodeMap;
    vector<ThreeAddressNodePtr> results;

    vector<ThreeAddressNodePtr> formalParameters = this->getFunction(functionName)->paramenters;
    for(auto ptr : formalParameters) {
        ThreeAddressNodePtr newNode = make_shared<ThreeAddressNode>(ptr, nodeMap);
        newNode->setLineNumber(callSite);
        nodeMap[ptr->getIDWithOutLineNumber()] = newNode;
        results.push_back(newNode);

        parameters.push_back(newNode);
    }


    vector<ThreeAddressNodePtr> functionInstructions = this->getFunction(functionName)->instructions;
    for(auto ins : functionInstructions) {

        ThreeAddressNodePtr newNode = make_shared<ThreeAddressNode>(ins, nodeMap);
        newNode->setLineNumber(callSite);
        nodeMap[ins->getIDWithOutLineNumber()] = newNode;
        results.push_back(newNode);
    }


    if(functionName == "preshare") {
        for(auto it = results.begin(); it + 1 != results.end(); ) {
            (*it)->prettyPrint();
            if((*it)->getOp() != ASTNode::Operator::NULLOP) {
                it = results.erase(it);
            } else
                ++it;
        }
    }

    vector<ThreeAddressNodePtr> returns = this->getFunction(functionName)->returns;
    for(auto ptr : returns) {
        returnVariables.push_back(nodeMap[ptr->getIDWithOutLineNumber()]);
    }

    return results;
}


ThreeAddressNodePtr ASTNode::NArrayIndex::threeAddCodeGen(IRCodeGenContext& context) {
    string name = this->arrayName->getName();
    cout << "Generate array index expression of " <<  this->arrayName->getName() << endl;

    string index = "";
    for(auto arraySize : *(this->dimons)) {
        if(arraySize->getTypeName() == "NIdentifier") {
            NIdentifierPtr ident = dynamic_pointer_cast<NIdentifier>(arraySize);
            index += to_string(context.getSymbolConcreteValue(ident->getName()));
        } else if(arraySize->getTypeName() == "NInteger") {
            NIntegerPtr ident = dynamic_pointer_cast<NInteger>(arraySize);
            index += to_string(ident->getValue());
        } else if(arraySize->getTypeName() == "NBinaryOperator") {
            ThreeAddressNodePtr indexExpr = arraySize->threeAddCodeGen(context);
            index += to_string(context.evaluateExpr(indexExpr, context));
        }
    }


    int start =(this->dimons)->size();
    int bound = context.arrayBounds[this->getArrayName()->getName()].size();
    string variableName = name + index;
    vector<string> resultIndexes;

    if(start != bound) {
        vector<string> arrayIndexes;
        arrayIndexes.push_back(variableName);
        for(int i = start; i < bound; i++) {
            int bound = context.arrayBounds[this->getArrayName()->getName()][i];
            vector<string> tempIndexes;
            while(arrayIndexes.size() != 0) {
                string ele = arrayIndexes.back();
                arrayIndexes.pop_back();
                for(int i = 0; i < bound; i++) {
                    string elep = ele + to_string(i);
                    tempIndexes.push_back(elep);
                }
            }
            arrayIndexes = tempIndexes;
        }

        resultIndexes = arrayIndexes;
        context.ArrayAssignmentNameStack.push(resultIndexes);


    } else {
        return context.getSymbolValue(name + index);
    }
    return nullptr;

}

ThreeAddressNodePtr ASTNode::NAssignment::threeAddCodeGen(IRCodeGenContext& context) {
    cout << "Generating assignment of " << this->LHS->getName() << " = " << endl;
    ThreeAddressNodePtr lhs = nullptr;
    vector<ThreeAddressNodePtr> types = context.getSymbolType(this->LHS->getName());
    if(types.size() == 0) {
        lhs = context.getSymbolValue(this->LHS->getName());
        if(!lhs) {
            cerr << "Undeclared variable " << this->LHS->getName() << endl;
        }
        ThreeAddressNodePtr rhs = this->RHS->threeAddCodeGen(context);

        if(rhs->getNodeType() == NodeType::CONSTANT) {
            context.setSymbolConcreteValue(this->LHS->getName(), atoi(rhs->getNodeName().c_str()));
        } else if(rhs->getNodeType() == NodeType::RANDOM || rhs->getNodeType() == NodeType::PRIVATE) {
            rhs->setNodeName(this->LHS->getName());
            context.setSymbolValueViaAss(this->LHS->getName(), rhs);
        } else {
            if(lhs->getNodeType() == NodeType::INT) {
                rhs->setNodeType(NodeType::INT);
                context.setSymbolValueViaAss(this->LHS->getName(), rhs);
            } else {
                context.setSymbolValueViaAss(this->LHS->getName(), rhs);
                rhs->setNodeName(this->LHS->getName());
            }
        }
        ThreeAddressNodePtr re = make_shared<ThreeAddressNode>(this->LHS->getName(), rhs);
        if(re->getNodeType() != NodeType::CONSTANT) {
            re->prettyPrint();
            if(re->getNodeType() != NodeType::INT)
                context.getCurrentFunction()->instructions.push_back(re);
        }
        return re;
    } else {
        ThreeAddressNodePtr rhs = this->RHS->threeAddCodeGen(context);
        vector<ThreeAddressNodePtr> arrayRhs = context.popArrayRangeAssignment();
        for(int i = 0; i < types.size(); i++) {


            ThreeAddressNodePtr re = context.declareVariable("internal", types[i]->getNodeName());
            re->setLhs(arrayRhs[i]);
            context.setSymbolValueViaAss(types[i]->getNodeName(), re);
            context.getCurrentFunction()->instructions.push_back(re);
        }
        return nullptr;
    }




}


ThreeAddressNodePtr ASTNode::NBinaryOperator::threeAddCodeGen(IRCodeGenContext& context) {
    ThreeAddressNodePtr re = nullptr;
    context.contextOfArrayIndex = 0;
    if(this->lhs->getTypeName() == "NArrayRange" && this->rhs->getTypeName() == "NArrayRange") {
        NArrayRangePtr lhs = dynamic_pointer_cast<NArrayRange>(this->lhs);
        NArrayRangePtr rhs = dynamic_pointer_cast<NArrayRange>(this->rhs);

        int from = lhs->getFrom()->getValue();
        int to = lhs->getTo()->getValue();

        string lhsName = lhs->getIdentifier()->getName();
        string rhsName = rhs->getIdentifier()->getName();

        vector<ThreeAddressNodePtr> assignResult;
        for(int i = from; i <= to; i++) {
            string lhsName1 = lhsName + to_string(i);
            string rhsName1 = rhsName + to_string(i);
            ThreeAddressNodePtr lhs1 = context.getThreeAddressNodePtrDDG(context.getCurrentFunctionName(), lhsName1);
            ThreeAddressNodePtr rhs1 = context.getThreeAddressNodePtrDDG(context.getCurrentFunctionName(), rhsName1);

            ThreeAddressNodePtr node = make_shared<ThreeAddressNode>("", lhs1, rhs1, this->getOp(), NodeType::INTERNAL);
            assignResult.push_back(node);
        }

        context.addArrayRangeAssignment(assignResult);
        return re;
    }
    ThreeAddressNodePtr L = this->lhs->threeAddCodeGen(context);
    ThreeAddressNodePtr R = this->rhs->threeAddCodeGen(context);

    if(context.getCurrentFunctionName() == "preshare") {
        re = make_shared<ThreeAddressNode>("t" + to_string(globalCount), L, R, this->getOp(), NodeType::PRESHARE);
    } else {
        re = make_shared<ThreeAddressNode>("t" + to_string(globalCount), L, R, this->getOp(), NodeType::INTERNAL);
    }
    if(re->getOp() != Operator::LE && re->getOp() != Operator::NE) {
        context.addFunctionInstructionDDG(context.getCurrentFunctionName(), "t" + to_string(globalCount), re);
        context.addOneInstruction(re);
    }
    globalCount++;
    return re;
}

ThreeAddressNodePtr ASTNode::NUnaryOperator::threeAddCodeGen(IRCodeGenContext& context) {
    ThreeAddressNodePtr re = nullptr;

    ThreeAddressNodePtr L = this->lhs->threeAddCodeGen(context);

    if(this->getOp() == Operator::TRCON) {
        int i = context.evaluateExpr(L,context);
        L = context.declareVariable("constant", to_string(i));
    }

    re = make_shared<ThreeAddressNode>("t" + to_string(globalCount), L, nullptr, this->getOp(), NodeType::PRESHARE);
    globalCount++;
    return re;

}


ThreeAddressNodePtr ASTNode::NArrayAssignment::threeAddCodeGen(IRCodeGenContext& context) {
    cout << "Generate array index assignment of " <<  this->arrayIndex->getArrayName()->getName() << endl;

    ThreeAddressNodePtr left = this->arrayIndex->threeAddCodeGen(context);

    ThreeAddressNodePtr node =  this->expression->threeAddCodeGen(context);

    if(left != nullptr) {
        vector<string> res1;
        res1.push_back(left->getNodeName());
        context.ArrayAssignmentNameStack.push(res1);

        vector<ThreeAddressNodePtr> res2;
        res2.push_back(node);
        context.addArrayRangeAssignment(res2);

    }

    if(this->expression->getTypeName() == "NArrayIndex" && node == nullptr) {
        vector<string> right = context.ArrayAssignmentNameStack.top();
        context.ArrayAssignmentNameStack.pop();
        vector<ThreeAddressNodePtr> res2;
        for(auto r : right) {
            res2.push_back(context.getSymbolValue(r));
        }
        context.addArrayRangeAssignment(res2);
    } else if(this->expression->getTypeName() == "NFunctionCall") {

    }


    vector<string> res1 = context.ArrayAssignmentNameStack.top();
    context.ArrayAssignmentNameStack.pop();
    vector<ThreeAddressNodePtr> res2 = context.popArrayRangeAssignment();

    if(res1.size() != res2.size()) {
        cout << "Unknown array variable name" << endl;
    }

    if(res1.size() == 1) {
        if(res2[0]->getNodeType() == NodeType::RANDOM || res2[0]->getOp() != NULLOP) {
            res2[0]->setNodeName(res1[0]);
            context.getCurrentFunction()->instructions.push_back(res2[0]);
            context.setSymbolValueViaAss(res1[0], res2[0]);
        } else {
            ThreeAddressNodePtr re = context.declareVariable("internal", res1[0]);
            re->setLhs(res2[0]);
            context.getCurrentFunction()->instructions.push_back(re);
            context.setSymbolValueViaAss(res1[0], re);
        }

        res2[0]->prettyPrint();

    } else {
        for (int i = 0; i < res1.size(); i++) {
            ThreeAddressNodePtr re = context.declareVariable("internal", res1[i]);
            re->setLhs(res2[i]);
            context.setSymbolValueViaAss(res1[i], re);
            context.getCurrentFunction()->instructions.push_back(re);
        }
    }

    return nullptr;
}

ThreeAddressNodePtr ASTNode::NIdentifier::threeAddCodeGen(IRCodeGenContext& context) {
    ThreeAddressNodePtr re = nullptr;

    string nodeName = this->getName();
    if(nodeName == "newrandom") {
        re = context.declareVariable("random", "newrandom");
    } else {
        re = context.getSymbolValue(nodeName);
    }

    if(this->isRandom) {
        re->setNodeType(NodeType::RANDOM);
    }
    return re;
}

ThreeAddressNodePtr ASTNode::NArrayRange::threeAddCodeGen(IRCodeGenContext& context) {
    return nullptr;
}

ThreeAddressNodePtr ASTNode::NVariableDeclaration::threeAddCodeGen(IRCodeGenContext& context) {
    cout << "Generating variable declaration of " << this->type->getName() << " " << this->id->getName() << endl;

    if(this->type->isArray) {
        ThreeAddressNodePtr temp = context.declareVariable(this->type->getName(), this->id->getName());
        context.setSymbolValueViaDecl(this->id->getName(), temp);


        vector<int> arrayBounds;
        for(auto arraySize : *(this->type->arraySize)) {
            string index = "";
            if(arraySize->getTypeName() == "NIdentifier") {
                NIdentifierPtr ident = dynamic_pointer_cast<NIdentifier>(arraySize);
                arrayBounds.push_back(context.getSymbolConcreteValue(ident->getName()));
            } else if(arraySize->getTypeName() == "NInteger") {
                NIntegerPtr ident = dynamic_pointer_cast<NInteger>(arraySize);
                arrayBounds.push_back(ident->getValue());
            }

        }

        context.arrayBounds[this->id->getName()] = arrayBounds;

        vector<string> arrayIndexes;
        arrayIndexes.push_back(this->id->getName());
        for(auto it = arrayBounds.begin(); it != arrayBounds.end(); it++) {
            vector<string> tempIndexes;
            while(arrayIndexes.size() != 0) {
                string ele = arrayIndexes.back();
                arrayIndexes.pop_back();
                for(int i = 0; i < *it; i++) {
                    string elep = ele + to_string(i);
                    tempIndexes.push_back(elep);
                }
            }
            arrayIndexes = tempIndexes;
        }

        for(string arrayIndex : arrayIndexes) {
            ThreeAddressNodePtr re = context.declareVariable(this->type->getName(), arrayIndex);
            context.setSymbolValueViaDecl(arrayIndex, re);
            context.setSymbolType(this->id->getName(), re);
            if (this->isParameter) {
                context.getCurrentFunction()->paramenters.push_back(re);
            }
        }

        if(this->getAssignmentExpr() != nullptr) {
            NAssignment assignment(this->id, this->assignmentExpr);
            assignment.threeAddCodeGen(context);
        }
    }
    else {
        string name = this->id->getName();
        ThreeAddressNodePtr re = context.declareVariable(this->type->getName(), name);
        context.setSymbolValueViaDecl(name, re);


        if(this->isParameter)
            context.getCurrentFunction()->paramenters.push_back(re);
        if(this->getAssignmentExpr() != nullptr) {
            NAssignment assignment(this->id, this->assignmentExpr);
            assignment.threeAddCodeGen(context);
        }
    }

    context.PrintSymTable();
    context.PrintConcreteValue();



    return nullptr;
}

const ASTNode::NIdentifierPtr &ASTNode::NVariableDeclaration::getId() const {
    return id;
}


ThreeAddressNodePtr ASTNode::NForStatement::threeAddCodeGen(IRCodeGenContext& context) {
    ThreeAddressNodePtr initNode = this->init->threeAddCodeGen(context);
    ThreeAddressNodePtr temp = context.getSymbolValue(initNode->getNodeName());
    int res = context.evaluateExpr(temp, context);
    context.setSymbolValueViaDecl(initNode->getNodeName(), context.declareVariable("int", initNode->getNodeName()));
    context.setSymbolConcreteValue(initNode->getNodeName(), res);

    ThreeAddressNodePtr condValue = this->condition->threeAddCodeGen(context);

    while(context.evaluateCond(condValue, context) == true) {
        context.pushBlock();

        this->block->threeAddCodeGen(context);

        context.popBlock();

        this->increase->threeAddCodeGen(context);
        int res = context.evaluateExpr(context.getSymbolValue(initNode->getNodeName()), context);
        context.setSymbolValueViaDecl(initNode->getNodeName(), context.declareVariable("int", initNode->getNodeName()));
        context.setSymbolConcreteValue(initNode->getNodeName(), res);
    }

    return nullptr;
}




ThreeAddressNodePtr ASTNode::NFunctionDeclaration::threeAddCodeGen(IRCodeGenContext& context) {
    cout << "Generating function declaration of " << this->id->getName() << endl;
    Function* function = new Function();
    string functionName = this->id->getName();
    function->name = functionName;
    context.pushFunction(function);

    context.addFunction(functionName);

    context.pushBlock();

    for(auto pa : *(this->var_list)) {
        pa->threeAddCodeGen(context);
    }

    this->block->threeAddCodeGen(context);

    context.popBlock();



    if(context.getCurrentFunctionName() != "main") {
        context.ToSSA(context.getCurrentFunctionName());
    }



    return nullptr;
}

ThreeAddressNodePtr ASTNode::NBlock::threeAddCodeGen(IRCodeGenContext& context) {
    for(auto stmt : *(this->getStmtList())) {
        stmt->threeAddCodeGen(context);
    }
    return nullptr;
}

ThreeAddressNodePtr ASTNode::NInteger::threeAddCodeGen(IRCodeGenContext& context) {
    return make_shared<ThreeAddressNode>(to_string(this->value), nullptr, nullptr, Operator::NULLOP, NodeType::CONSTANT);
}



ThreeAddressNodePtr ASTNode::NReturnStatement::threeAddCodeGen(IRCodeGenContext& context) {
    cout << "Generating return statement" << endl;

    ThreeAddressNodePtr re = this->expr->threeAddCodeGen(context);
    string id;
    if(re == nullptr) {
        NIdentifierPtr returnId = dynamic_pointer_cast<NIdentifier>(this->expr);
        id = returnId->getName();
    } else {
        id = re->getNodeName();
    }

    vector<ThreeAddressNodePtr> returns = context.getSymbolType(id);
    vector<ThreeAddressNodePtr> returnsPrime;


    for(ThreeAddressNodePtr nodePtr : returns) {
        returnsPrime.push_back(context.getSymbolValue(nodePtr->getNodeName()));
    }

    context.getCurrentFunction()->returns = returnsPrime;

    return nullptr;

}



ThreeAddressNodePtr ASTNode::NExpressionStatement::threeAddCodeGen(IRCodeGenContext &context) {
    context.contextOfArrayIndex = 0;
    return this->expr->threeAddCodeGen(context);
}

ThreeAddressNodePtr ASTNode::NFunctionCall::threeAddCodeGen(IRCodeGenContext &context) {
    cout << "Generate function call of " << this->ident->getName() << endl;

    string functionName = this->ident->getName();

    vector<ThreeAddressNodePtr> realParameters;
    for (auto arg : *(this->arguments)) {
        ThreeAddressNodePtr argPtr = arg->threeAddCodeGen(context);
        if(argPtr == nullptr) {
            vector<ThreeAddressNodePtr> res = context.getSymbolValues(context.ArrayAssignmentNameStack.top());
            context.ArrayAssignmentNameStack.pop();
            for(auto re : res)
                realParameters.push_back(re);
        } else if(context.getSymbolType(argPtr->getNodeName()).size() == 0) {
            realParameters.push_back(context.getSymbolValue(argPtr->getNodeName()));
        } else {
            for (ThreeAddressNodePtr nodePtr : context.getSymbolType(argPtr->getNodeName()))
                realParameters.push_back(context.getSymbolValue(nodePtr->getNodeName()));
        }
    }
    vector<ThreeAddressNodePtr> formalParameters = context.getFunction(functionName)->paramenters;

    if (realParameters.size() != formalParameters.size()) {
        assert(false && "size of real parameters is not equal to the size of formal parameters");
    }

    vector<ThreeAddressNodePtr> parameters;
    vector<ThreeAddressNodePtr> returnVariables;
    vector<ThreeAddressNodePtr> copiedFunctionInstruction = context.copyInstructionSet(functionName, this->callSite,
                                                                                       parameters, returnVariables);


    for (int i = 0; i < realParameters.size(); i++) {
        ThreeAddressNodePtr realP = realParameters[i];
        ThreeAddressNodePtr formP = parameters[i];
        assert(formP->getLhs() == nullptr && formP->getRhs() == nullptr);
        formP->setLhs(realP);

    }

    context.addArrayRangeAssignment(returnVariables);

    for (auto ins : copiedFunctionInstruction)
        context.addOneInstruction(ins);

    for(auto ins: copiedFunctionInstruction) {
        context.getCurrentFunction()->instructions.push_back(ins);
    }
    return nullptr;
}


ThreeAddressNodePtr ASTNode::NIFStatement::threeAddCodeGen(IRCodeGenContext &context) {
    return nullptr;
}







