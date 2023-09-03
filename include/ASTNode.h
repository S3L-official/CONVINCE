//
// Created by Pengfei Gao on 2019-08-31.
//

#ifndef TESTBISON_ASTNODE_H
#define TESTBISON_ASTNODE_H

#include <iostream>
#include <vector>
#include "json/json.h"
#include <memory>
#include <cassert>

using namespace std;

class IRCodeGenContext;
class ThreeAddressNode;
class CodeGenContext;
class Interpreter;
class Value;
typedef shared_ptr<Value> ValuePtr;
typedef shared_ptr<ThreeAddressNode> ThreeAddressNodePtr;
typedef weak_ptr<ThreeAddressNode> ThreeAddressNodeWeakPtr;


namespace ASTNode {
    enum Operator {
        MINUS,
        FFTIMES,
        POWER,
        LSL,
        LSR,
        AND,
        XOR,
        OR,
        MOD,
        LE,
        LAND,
        NOT,
        ADD,
        MUL,
        DIVIDE,
        LSH,
        RSH,
        SBOX,
        NULLOP,
        POW2,
        POW4,
        POW16,
        AFFINE,
        TABLELUT,
        TABLELUT4,
        TRCON,
        XTIMES,
        NE,
        EQ,
        POL,
        LIN,
        CALL,
        PUSH
    };
    class Node {
    public:
        virtual string getTypeName() const = 0;
        virtual Json::Value jsonGen() const { return Json::Value();}
        virtual ThreeAddressNodePtr threeAddCodeGen(IRCodeGenContext& context) {return nullptr;}
        // 将ASTNode转化成Value
        virtual ValuePtr compute(Interpreter& interpreter) {return nullptr;}

    };

    class NExpression : public Node {
    public:
        string getTypeName() const {
            return "NExpression";
        }
        Json::Value jsonGen() const {
            Json::Value root;
            root["name"] = getTypeName();
            return root;
        }

    };

    typedef shared_ptr<NExpression> NExpressionPtr;
    typedef vector<NExpressionPtr> NExpressionList;
    typedef shared_ptr<vector<NExpressionPtr>> NExpressionListPtr;
    typedef std::shared_ptr<Node> ASTNodePtr;

    class NStatement : public Node {
    public:
        string getTypeName() const {
            return "NStatement";
        }
        Json::Value jsonGen() const {
            Json::Value root;
            root["name"] = getTypeName();
            return root;
        }

    };
    typedef shared_ptr<NStatement> NStatementPtr;
    typedef vector<shared_ptr<NStatement>> NStatementList;
    typedef shared_ptr<NStatementList> NStatementListPtr;


    class NIdentifier : public NExpression {
    private:
        std::string name;
    public:
        const std::string &getName() const {
            return name;
        }

        void setName(const std::string &name) {
            NIdentifier::name = name;
        }

        NIdentifier(std::string name) : name(name) {
            arraySize = make_shared<NExpressionList>();
        }
        bool isType = false;
        bool isArray = false;
        bool isRandom = false;
        bool isParameter = false;
        NExpressionListPtr arraySize;

        string getTypeName() const override{
            if(isType) return name;
            return "NIdentifier";
        }
        Json::Value jsonGen() const override {
            Json::Value root;
            root["name"] = getTypeName() + ":" + name + (isArray? "(Array)":"");
            for(auto it = arraySize->begin(); it != arraySize->end(); it++) {
                root["children"].append((*it)->jsonGen());
            }
            return root;
        }

        virtual ThreeAddressNodePtr threeAddCodeGen(IRCodeGenContext& context) override ;
        virtual ValuePtr compute(Interpreter& interpreter) override;

    };
    typedef shared_ptr<NIdentifier> NIdentifierPtr;



    class NArrayIndex : public NExpression {
    private:
        NIdentifierPtr arrayName;
    public:
        NExpressionListPtr dimons;
        NArrayIndex(NIdentifierPtr name)
                : arrayName(name){
            dimons = make_shared<NExpressionList>();
        }

        string getTypeName() const override {
            return "NArrayIndex";
        }

        NIdentifierPtr getArrayName() {
            return arrayName;
        }
        Json::Value jsonGen() const override {
            Json::Value root;
            root["name"] = getTypeName();
            root["children"].append(arrayName->jsonGen());
            for(auto it = dimons->begin(); it != dimons->end(); it++){
                root["children"].append((*it)->jsonGen());
            }
            return root;
        }

        virtual ThreeAddressNodePtr threeAddCodeGen(IRCodeGenContext& context) override;
        virtual ValuePtr compute(Interpreter& interpreter) override;

    };
    typedef shared_ptr<NArrayIndex> NArrayIndexPtr;


    class NAssignment : public NExpression {
    private:
        NIdentifierPtr LHS;
        NExpressionPtr RHS;
    public:
        NAssignment(NIdentifierPtr lhs, NExpressionPtr rhs) : LHS(lhs), RHS(rhs) {}

        string getTypeName() const override{
            return "NAssignment";
        }
        Json::Value jsonGen() const override{
            Json::Value root;
            root["name"] = getTypeName();
            root["children"].append(LHS->jsonGen());
            root["children"].append(RHS->jsonGen());
            return root;
        }

        virtual ThreeAddressNodePtr threeAddCodeGen(IRCodeGenContext& context) override;



        virtual ValuePtr compute(Interpreter& interpreter) override;
    };

    class NFunctionCall : public NExpression {
    private:
        NIdentifierPtr ident;
        NExpressionListPtr arguments;
        int callSite;
    public:
        NFunctionCall(NIdentifierPtr id, NExpressionListPtr argus, int lineNumber) : ident(id), arguments(argus), callSite(lineNumber){}

        string getTypeName() const override{
            return "NFunctionCall";
        }
        Json::Value jsonGen() const override{
            Json::Value root;
            root["name"] = getTypeName();
            root["children"].append(ident->jsonGen());
            for(auto exp : *arguments)
                root["children"].append(exp->jsonGen());
            return root;
        }

        virtual ThreeAddressNodePtr threeAddCodeGen(IRCodeGenContext& context) override;
        virtual ValuePtr compute(Interpreter& interpreter) override;
    };


    class NArrayAssignment : public NExpression {
    private:
        NArrayIndexPtr arrayIndex;
        NExpressionPtr expression;
    public:
        NArrayAssignment(NArrayIndexPtr lhs, NExpressionPtr rhs) : arrayIndex(lhs), expression(rhs) {}
        string getTypeName() const override {
            return "NArrayAssignment";
        }
        Json::Value jsonGen() const override{
            Json::Value root;
            root["name"] = getTypeName();
            root["children"].append(arrayIndex->jsonGen());
            root["children"].append(expression->jsonGen());
            return root;
        }

        virtual ThreeAddressNodePtr threeAddCodeGen(IRCodeGenContext& context) override;
        virtual ValuePtr compute(Interpreter& interpreter) override;
    };

    class NBinaryOperator : public NExpression {
    private:
        Operator op;
        NExpressionPtr lhs;
        NExpressionPtr rhs;

    public:
        NBinaryOperator(NExpressionPtr lhs, Operator op, NExpressionPtr rhs) : op(op), lhs(lhs), rhs(rhs){}
        string getTypeName() const override{
            return "NBinaryOperator";
        }
        Json::Value jsonGen() const override {
            Json::Value root;
            root["name"] = getTypeName();
            root["children"].append(lhs->jsonGen());
            root["children"].append(rhs->jsonGen());
            return root;
        }

        Operator getOp() const {
            return op;
        }


        virtual ThreeAddressNodePtr threeAddCodeGen(IRCodeGenContext& context) override ;


        virtual ValuePtr compute(Interpreter& interpreter) override;
    };

    class NUnaryOperator : public NExpression {
    private:
        Operator op;
        NExpressionPtr lhs;

    public:
        NUnaryOperator(NExpressionPtr lhs, Operator op) : op(op), lhs(lhs) {}
        string getTypeName() const override{
            return "NUnaryOperator";
        }
        Json::Value jsonGen() const override {
            Json::Value root;
            root["name"] = getTypeName();
            root["children"].append(lhs->jsonGen());
            return root;
        }

        Operator getOp() const {
            return op;
        }


        virtual ThreeAddressNodePtr threeAddCodeGen(IRCodeGenContext& context) override ;
        virtual ValuePtr compute(Interpreter& interpreter) override;
    };

    class NInteger : public NExpression {
    private:
        int value;
    public:
        NInteger(int val) : value(val) {}
        string getTypeName() const override{
            return "NInteger";
        }
        Json::Value jsonGen() const override{
            Json::Value root;
            root["name"] = getTypeName() + std::to_string(value);
            return root;
        }

        int getValue() const {
            return value;
        }

        void setValue(int value) {
            NInteger::value = value;
        }
        virtual ThreeAddressNodePtr threeAddCodeGen(IRCodeGenContext&) override;
        virtual ValuePtr compute(Interpreter& interpreter) override;
    };
    typedef shared_ptr<NInteger> NIntegerPtr;

    class NArrayRange : public NExpression {
    private:
        NIdentifierPtr arrayName;
        NIntegerPtr from;
        NIntegerPtr to;
    public:
        NArrayRange(NIdentifierPtr name, NIntegerPtr from, NIntegerPtr to) : arrayName(name), from(from), to(to) {}
        string getTypeName() const override{
            return "NArrayRange";
        }
        Json::Value jsonGen() const override{
            Json::Value root;
            root["name"] = getTypeName() ;
            root["children"].append(arrayName->jsonGen());
            root["children"].append(from->jsonGen());
            root["children"].append(to->jsonGen());
            return root;
        }

        NIntegerPtr getFrom() {
            return from;
        }

        NIntegerPtr getTo() {
            return to;
        }

        NIdentifierPtr getIdentifier() {
            return arrayName;
        }


        virtual ThreeAddressNodePtr threeAddCodeGen(IRCodeGenContext& context) override;
    };
    typedef shared_ptr<NArrayRange> NArrayRangePtr;






    class NVariableDeclaration : public NStatement {
    private:
        const NIdentifierPtr type;
        NIdentifierPtr id;
        NExpressionPtr assignmentExpr = nullptr;

    public:
        bool isParameter = false;
        NVariableDeclaration(){}

        const NIdentifierPtr &getId() const;

        NVariableDeclaration(const shared_ptr<NIdentifier> type, shared_ptr<NIdentifier> id, shared_ptr<NExpression> assignmentExpr = NULL)
                : type(type), id(id), assignmentExpr(assignmentExpr) {
//            cout << "isArray = " << type->isArray << endl;
            assert(type->isType);
        }

        string getTypeName() const override {
            return "NVariableDeclaration";
        }

        string getMyType() {
            return type->getName();
        }

        NExpressionPtr getAssignmentExpr() {
            return assignmentExpr;
        }

        Json::Value jsonGen() const override{
            Json::Value root;
            root["name"] = getTypeName() ;
            root["children"].append(type->jsonGen());
            root["children"].append(id->jsonGen());
            if(assignmentExpr != nullptr)
                root["children"].append(assignmentExpr->jsonGen());
            return root;
        }
//
        virtual ThreeAddressNodePtr threeAddCodeGen(IRCodeGenContext& context) override;
        virtual ValuePtr compute(Interpreter& interpreter) override;
    };
    typedef shared_ptr<NVariableDeclaration> NVariableDeclarationPtr;
    typedef std::vector<NVariableDeclarationPtr> NVariableList;
    typedef shared_ptr<NVariableList> NVariableListPtr;


    class NArrayInitialization: public NStatement {
    private:
        NVariableDeclarationPtr declaration;
        NExpressionListPtr expressionList = make_shared<NExpressionList>();
    public:

        NArrayInitialization() {}


        NArrayInitialization(shared_ptr<NVariableDeclaration> dec, shared_ptr<NExpressionList> list)
                : declaration(dec), expressionList(list) {

        }

        string getTypeName() const override{
            return "NArrayInitialization";
        }
        Json::Value jsonGen() const override{
            Json::Value root;
            root["name"] = getTypeName() ;
            root["children"].append(declaration->jsonGen());
            for(auto exp : *expressionList)
                root["children"].append(exp->jsonGen());
            return root;
        }

//        virtual ThreeAddressNode* threeAddCodeGen(IRCodeGenContext& context) override;

        virtual ValuePtr compute(Interpreter& interpreter) override;
    };

    class NBlock : public NStatement {
    private:
        NStatementListPtr stmt_list;
    public:
        const NStatementListPtr &getStmtList() const {
            return stmt_list;
        }

        void setStmtList(const NStatementListPtr &stmtList) {
            stmt_list = stmtList;
        }

        NBlock() {
            stmt_list = make_shared<NStatementList>();
        }
        string getTypeName() const override{
            return "NBlock";
        }
        Json::Value jsonGen() const override{
            Json::Value root;
            root["name"] = getTypeName() ;
            cout << getTypeName() << endl;
            for(auto exp : *stmt_list)
                root["children"].append(exp->jsonGen());
            return root;
        }


        virtual ThreeAddressNodePtr threeAddCodeGen(IRCodeGenContext& context) override;
        virtual ValuePtr compute(Interpreter& interpreter) override;

    };
    typedef shared_ptr<NBlock> NBlockPtr;


    class NFunctionDeclaration : public NStatement {
    private:
        NIdentifierPtr type;
        NIdentifierPtr id;
        NVariableListPtr var_list;
        NBlockPtr block;
    public:
        NFunctionDeclaration(NIdentifierPtr type, NIdentifierPtr id,
                NVariableListPtr var_list, NBlockPtr block) : type(type), id(id), var_list(var_list), block(block) {}
        string getTypeName() const override{
            return "NFunctionDeclaration";
        }
        Json::Value jsonGen() const override{
            Json::Value root;
            root["name"] = getTypeName() ;
            root["children"].append(type->jsonGen());
            root["children"].append(id->jsonGen());
            for(auto exp : *var_list)
                root["children"].append(exp->jsonGen());
            root["children"].append(block->jsonGen());
            return root;
        }
        const NIdentifierPtr &getId() const {
            return id;
        }

        void setId(const NIdentifierPtr &id) {
            NFunctionDeclaration::id = id;
        }

        virtual ThreeAddressNodePtr threeAddCodeGen(IRCodeGenContext& context)override;
        virtual ValuePtr compute(Interpreter& interpreter) override;
    };



    class NExpressionStatement : public NStatement {
    private:
        NExpressionPtr expr;
    public:
        NExpressionStatement(NExpressionPtr expr) : expr(expr) {}
        string getTypeName() const override{
            return "NExpressionStatement";
        }
        Json::Value jsonGen() const override{
            Json::Value root;
            root["name"] = getTypeName() ;
            root["children"].append(expr->jsonGen());
            return root;
        }

        virtual ThreeAddressNodePtr threeAddCodeGen(IRCodeGenContext& context) override;


        virtual ValuePtr compute(Interpreter& interpreter) override;
    };

    class NReturnStatement : public NStatement {
    private:
        NExpressionPtr expr;
    public:
        NReturnStatement(NExpressionPtr expr) : expr(expr) {}
        string getTypeName() const override{
            return "NReturnStatement";
        }
        Json::Value jsonGen() const override{
            Json::Value root;
            root["name"] = getTypeName() ;
            root["children"].append(expr->jsonGen());
            return root;
        }

        virtual ThreeAddressNodePtr threeAddCodeGen(IRCodeGenContext& context) override;
        virtual ValuePtr compute(Interpreter& interpreter) override;
    };

    class NForStatement : public NStatement {
    private:
        NExpressionPtr init;
        NExpressionPtr condition;
        NExpressionPtr increase;
        NBlockPtr block;
        int from;
        int to;
        string name;
    public:
        NForStatement(const ASTNode::NExpressionPtr &init, const ASTNode::NExpressionPtr &condition,
                                              const ASTNode::NExpressionPtr &increase, const ASTNode::NBlockPtr &block) : init(
                init), condition(condition), increase(increase), block(block) {}
        string getTypeName() const override{
            return "NForStatement";
        }
        Json::Value jsonGen() const override{
            Json::Value root;
            root["name"] = getTypeName() ;
            root["children"].append(init->jsonGen());
            root["children"].append(condition->jsonGen());
            root["children"].append(increase->jsonGen());
            root["children"].append(block->jsonGen());
            return root;
        }

        virtual ThreeAddressNodePtr threeAddCodeGen(IRCodeGenContext& context) override;
        virtual ValuePtr compute(Interpreter& interpreter) override;
    };

    class NIFStatement : public NStatement {
    private:
        NExpressionPtr predicate;
        NBlockPtr ifBlock;
        NBlockPtr elseBlock;

    public:
        NIFStatement(ASTNode::NExpressionPtr pre, ASTNode::NBlockPtr ifblock, ASTNode::NBlockPtr elseblock) :
            predicate(pre), ifBlock(ifblock), elseBlock(elseblock) {
        }

        string getTypeName() const override{
            return "NIfStatement";
        }
        Json::Value jsonGen() const override{
            Json::Value root;
            root["name"] = getTypeName() ;
            root["children"].append(predicate->jsonGen());
            root["children"].append(ifBlock->jsonGen());
            root["children"].append(elseBlock->jsonGen());
            return root;
        }

        virtual ThreeAddressNodePtr threeAddCodeGen(IRCodeGenContext& context) override;
        virtual ValuePtr compute(Interpreter& interpreter) override;
    };

    class NPredicate : public NExpression {
    public:
        string getTypeName() const override{
            return "NPredicate";
        }
        Json::Value jsonGen() const override{
            Json::Value root;
            root["name"] = getTypeName() ;
            return root;
        }
        virtual ValuePtr compute(Interpreter& interpreter) override;
    };
    typedef shared_ptr<NPredicate> NPredicatePtr;

    class DomPredicate : public NPredicate {
    private:
        NArrayIndexPtr first;
        NArrayIndexPtr second;
    public:
        DomPredicate(const NArrayIndexPtr &first, const NArrayIndexPtr &second) : first(first), second(second) {}

        string getTypeName() const override{
            return "DomPredicate";
        }
        Json::Value jsonGen() const override{
            Json::Value root;
            root["name"] = getTypeName() ;
            root["children"].append(first->jsonGen());
            root["children"].append(second->jsonGen());
            return root;
        }
        virtual ValuePtr compute(Interpreter& interpreter) override;
    };

    class RvalPredicate : public NPredicate {
    private:
        NArrayIndexPtr first;
        NArrayIndexPtr second;
    public:
        RvalPredicate(const NArrayIndexPtr &first, const NArrayIndexPtr &second) : first(first), second(second) {}
        string getTypeName() const override{
            return "RvalPredicate";
        }
        Json::Value jsonGen() const override{
            Json::Value root;
            root["name"] = getTypeName() ;
            root["children"].append(first->jsonGen());
            root["children"].append(second->jsonGen());
            return root;
        }

        virtual ValuePtr compute(Interpreter& interpreter) override;
    };

    class TypePredicate : public NPredicate {
    private:
        NArrayIndexPtr first;
        NIdentifierPtr second;
    public:
        TypePredicate(const NArrayIndexPtr &first, const NIdentifierPtr &second) : first(first), second(second) {}
        string getTypeName() const override{
            return "TypePredicate";
        }
        Json::Value jsonGen() const override{
            Json::Value root;
            root["name"] = getTypeName() ;
            root["children"].append(first->jsonGen());
            root["children"].append(second->jsonGen());
            return root;
        }

        virtual ValuePtr compute(Interpreter& interpreter) override;
    };

    class NAssumeStatement : public NStatement {
    private:
        NExpressionListPtr expressionListPtr;

    public:
        NAssumeStatement(const NExpressionListPtr &expressionListPtr) : expressionListPtr(expressionListPtr) {}

        string getTypeName() const override{
            return "NAssumeStatement";
        }
        Json::Value jsonGen() const override{
            Json::Value root;
            root["name"] = getTypeName() ;
            for(auto ele : *expressionListPtr)
                root["children"].append(ele->jsonGen());
            return root;
        }

        virtual ValuePtr compute(Interpreter& interpreter) override;

    };


}


#endif //TESTBISON_ASTNODE_H
