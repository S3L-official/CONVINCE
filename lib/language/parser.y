
%{
	#include <stdio.h>
	#include <stdlib.h>
    	#include "ASTNode.h"
	extern FILE *fp;
	extern FILE *yyin;
	extern int yylex();
	extern int yylineno;
	extern void yyerror(const char* s, ...);
	using namespace ASTNode;
        ASTNode::NBlock *programRoot; /* the root node of our program AST:: */
%}

%union {
	ASTNode::NIdentifier* ident;
	ASTNode::NStatement* stmt;
	ASTNode::NBlock* block;
	std::string* string;
	ASTNode::NArrayIndex* index;
	ASTNode::NArrayRange* range;
	ASTNode::NExpression* expr;
	ASTNode::NInteger* integer;
	ASTNode::NVariableList* varlist;
	ASTNode::NVariableDeclaration* var_decl;
	ASTNode::NStatementList* stmtList;
	std::vector<shared_ptr<ASTNode::NExpression>>* exprvec;
	ASTNode::NFunctionDeclaration* fundecl;

}

%token <string> TIDENTIFIER TINTEGER TYINT TYPRIVATE TYPUBLIC TYSHARE TYVOID TYINTERNAL TEXTERN TYRANDOM TSI TLK TUF TUK
%token <token>  TFOR TRETURN TIF TELSE
%token <token> TPLUS TMINUS TDIVIDE TFFTIMES TCOMMA TASSIGN TSEMICOLON TLE TCOLON TNOTEQ TMOD TPOW2 TPOW4 TPOW16 TAFFINE TSBOX TTABLELUT TTABLELUT4 TRCON TXTIMES TPOL TLIN
%token <token> TLPAREN TRPAREN TLBRACKET TRBRACKET TLBRACE TRBRACE
%token <token> TAND TOR TXOR TNOT TNEWRANDOM TSHIFTL TSHIFTR TPOWER TLAND TCEQ

%token <token>  TDOM TRVAR TSLASH TINTERSECT TEMPTY TASSUME

%type <ident> ident primary_typename array_typename typename random node_type
%type <index> array_index
%type <expr> expr assign predicate
%type <exprvec> call_args predicates
%type <integer> numeric
%type <range> array_range
%type <stmt> var_decl stmt for_stmt if_stmt assume_stmt
%type <varlist> func_decl_args
%type <block> block stmts
%type <fundecl> func_decl

%left TOR
%left TXOR
%left TAND
%left TSHIFTL
%left TPLUS TMINUS
%left TFFTIMES TDIVIDE
%nonassoc TNOT TPOW2 TPOW4 TPOW16 TAFFINE TSBOX TTABLELUT TTABLELUT4 TRCON TXTIMES

%locations
%define parse.trace
%start program



%token <integer> INT

%%
program
	: stmts {programRoot = $1;}
	;
stmts
	: stmt {$$ = new NBlock(); $$->getStmtList()->push_back(shared_ptr<NStatement>($1));}
	| stmts stmt { $1->getStmtList()->push_back(shared_ptr<NStatement>($2));}
	;

stmt
	: func_decl {$$ = $1;}
	| expr TSEMICOLON {$$ = new NExpressionStatement(shared_ptr<NExpression>($1));}
	| TRETURN expr TSEMICOLON {$$ = new NReturnStatement(shared_ptr<NExpression>($2));}
	| for_stmt{$$ = $1;}
	| var_decl TSEMICOLON{$$ = $1;}
	| if_stmt {$$ = $1;}
	| TRETURN TSEMICOLON
	| assume_stmt TSEMICOLON
	;

block
	: TLBRACE stmts TRBRACE {$$ = $2;}
	| TLBRACE TRBRACE {$$ = new NBlock();}
	;

primary_typename
	: TYINT {$$ = new NIdentifier(*$1); $$->isType = true;  delete $1;}
	| TYPRIVATE {$$ = new NIdentifier(*$1); $$->isType = true;  delete $1;}
	| TYPUBLIC {$$ = new NIdentifier(*$1); $$->isType = true;  delete $1;}
	| TYVOID {$$ = new NIdentifier(*$1); $$->isType = true;  delete $1;}
	| TYSHARE {$$ = new NIdentifier(*$1); $$->isType = true;  delete $1;}
	| TYINTERNAL {$$ = new NIdentifier(*$1); $$->isType = true;  delete $1;}
	| TYRANDOM {$$ = new NIdentifier(*$1); $$->isType = true;  delete $1;}
	;

array_typename
	: primary_typename TLBRACKET expr TRBRACKET {
		$1->isArray = true;
		$1->arraySize->push_back(shared_ptr<NExpression>($3));
		$$ = $1;
	}
	| array_typename TLBRACKET expr TRBRACKET {
		$1->arraySize->push_back(shared_ptr<NExpression>($3));
		$$ = $1;
	}
	;

typename
	: primary_typename
	| array_typename
	;

var_decl
	: typename ident {$$ = new NVariableDeclaration(shared_ptr<NIdentifier>($1), shared_ptr<NIdentifier>($2), nullptr); }
	| typename ident TASSIGN expr { $$ = new NVariableDeclaration(shared_ptr<NIdentifier>($1), shared_ptr<NIdentifier>($2), shared_ptr<NExpression>($4));}
	| typename ident TASSIGN TLBRACE call_args TRBRACE {
		 $$ = new NArrayInitialization(make_shared<NVariableDeclaration>(shared_ptr<NIdentifier>($1), shared_ptr<NIdentifier>($2), nullptr), shared_ptr<NExpressionList>($5));
	}
	;

func_decl
	: typename ident TLPAREN func_decl_args TRPAREN block {
		$$ = new NFunctionDeclaration(
			shared_ptr<NIdentifier>($1),
			shared_ptr<NIdentifier>($2),
			shared_ptr<NVariableList>($4),
			shared_ptr<NBlock>($6));
	}
	;

func_decl_args
	: {$$ = new NVariableList(); }
	| var_decl {$$ = new NVariableList(); $<var_decl>1->isParameter = true; $$->push_back(shared_ptr<NVariableDeclaration>($<var_decl>1));}
	| func_decl_args TCOMMA var_decl {$<var_decl>3->isParameter = true; $1->push_back(shared_ptr<NVariableDeclaration>($<var_decl>3));}
	;

ident
	: TIDENTIFIER {$$ = new NIdentifier(*$1); delete $1;}
	;
numeric
	: TINTEGER {$$ = new NInteger(atol($1->c_str()));}
	;
random
	: TNEWRANDOM {$$ = new NIdentifier("newrandom"); $$->isRandom = true; }
	;
array_range
	: ident TLBRACKET expr TCOLON expr TRBRACKET {
		//$$ = new NArrayRange(shared_ptr<NIdentifier>($1), shared_ptr<NInteger>($3), shared_ptr<NInteger>($5));
	}
	;
expr
	: assign { $$ = $1;}
	| ident TLPAREN call_args TRPAREN {
		$$ = new NFunctionCall(shared_ptr<NIdentifier>($1), shared_ptr<NExpressionList>($3), yylineno);
	}
	| ident {$<ident>$ = $1;}
	| numeric {$$ = $1;}
//	| TMINUS numeric {$$ = $2;}
	| random {$<ident>$ = $1;}
	| expr TPLUS expr {$$ = new NBinaryOperator(shared_ptr<NExpression>($1), Operator::ADD, shared_ptr<NExpression>($3));}
	| expr TMINUS expr {$$ = new NBinaryOperator(shared_ptr<NExpression>($1), Operator::MINUS, shared_ptr<NExpression>($3));}
	| expr TFFTIMES expr {$$ = new NBinaryOperator(shared_ptr<NExpression>($1), Operator::FFTIMES, shared_ptr<NExpression>($3));}
	| expr TDIVIDE expr {$$ = new NBinaryOperator(shared_ptr<NExpression>($1), Operator::DIVIDE, shared_ptr<NExpression>($3));}
	| expr TAND expr {$$ = new NBinaryOperator(shared_ptr<NExpression>($1), Operator::AND, shared_ptr<NExpression>($3));}
	| expr TXOR expr {$$ = new NBinaryOperator(shared_ptr<NExpression>($1), Operator::XOR, shared_ptr<NExpression>($3));}
	| expr TOR expr {$$ = new NBinaryOperator(shared_ptr<NExpression>($1), Operator::OR, shared_ptr<NExpression>($3));}
	| expr TLE expr {$$ = new NBinaryOperator(shared_ptr<NExpression>($1), Operator::LE, shared_ptr<NExpression>($3));}
	| expr TNOTEQ expr {$$ = new NBinaryOperator(shared_ptr<NExpression>($1), Operator::NE, shared_ptr<NExpression>($3));}
	| expr TSHIFTL expr {$$ = new NBinaryOperator(shared_ptr<NExpression>($1), Operator::LSH, shared_ptr<NExpression>($3));}
	| expr TSHIFTR expr {$$ = new NBinaryOperator(shared_ptr<NExpression>($1), Operator::RSH, shared_ptr<NExpression>($3));}
	| expr TMOD expr {$$ = new NBinaryOperator(shared_ptr<NExpression>($1), Operator::MOD, shared_ptr<NExpression>($3));}
	| expr TLAND expr {$$ = new NBinaryOperator(shared_ptr<NExpression>($1), Operator::LAND, shared_ptr<NExpression>($3));}
	| expr TCEQ expr {$$ = new NBinaryOperator(shared_ptr<NExpression>($1), Operator::EQ, shared_ptr<NExpression>($3));}
	| TNOT expr {$$ = new NUnaryOperator(shared_ptr<NExpression>($2), Operator::NOT); }
//	| TMINUS expr {$$ = new NUnaryOperator(shared_ptr<NExpression>($2), Operator::MINUS); }
	| TPOW2 expr {$$ = new NUnaryOperator(shared_ptr<NExpression>($2), Operator::POW2); }
	| TPOW4 expr {$$ = new NUnaryOperator(shared_ptr<NExpression>($2), Operator::POW4); }
	| TPOW16 expr {$$ = new NUnaryOperator(shared_ptr<NExpression>($2), Operator::POW16); }
	| TAFFINE expr {$$ = new NUnaryOperator(shared_ptr<NExpression>($2), Operator::AFFINE); }
	| TSBOX expr {$$ = new NUnaryOperator(shared_ptr<NExpression>($2), Operator::SBOX); }
	| TTABLELUT expr {$$ = new NUnaryOperator(shared_ptr<NExpression>($2), Operator::TABLELUT); }
	| TTABLELUT4 expr {$$ = new NUnaryOperator(shared_ptr<NExpression>($2), Operator::TABLELUT4); }
	| TRCON expr {$$ = new NUnaryOperator(shared_ptr<NExpression>($2), Operator::TRCON); }
	| TXTIMES expr {$$ = new NUnaryOperator(shared_ptr<NExpression>($2), Operator::XTIMES); }
	| TPOL expr {$$ = new NUnaryOperator(shared_ptr<NExpression>($2), Operator::POL); }
	| TLIN expr {$$ = new NUnaryOperator(shared_ptr<NExpression>($2), Operator::LIN); }

	| TLPAREN expr TRPAREN {$$ = $2;}
	| array_index { $$ = $1; }
	| array_range { $$ = $1;}
	;

array_index
	: ident TLBRACKET expr TRBRACKET {
		$$ = new NArrayIndex(shared_ptr<NIdentifier>($1));
		$$->dimons->push_back(shared_ptr<NExpression>($3));
	}
	| array_index TLBRACKET expr TRBRACKET {
		$1->dimons->push_back(shared_ptr<NExpression>($3));
		$$ = $1;
	}
	;
assign
	: ident TASSIGN expr {
		$$ = new NAssignment(shared_ptr<NIdentifier>($1), shared_ptr<NExpression>($3));
	}
	| array_index TASSIGN expr {
		$$ = new NArrayAssignment(shared_ptr<NArrayIndex>($1), shared_ptr<NExpression>($3));
	}
	| expr TASSIGN expr
	;
call_args
	: {$$ = new NExpressionList();}
	| expr {$$ = new NExpressionList(); $$->push_back(shared_ptr<NExpression>($1));}
	| call_args TCOMMA expr {$1->push_back(shared_ptr<NExpression>($3));}
	;

for_stmt
	: TFOR TLPAREN expr TSEMICOLON expr TSEMICOLON expr TRPAREN block {
		$$ = new NForStatement(shared_ptr<NExpression>($3), shared_ptr<NExpression>($5), shared_ptr<NExpression>($7), shared_ptr<NBlock>($9) );
	}
	;

if_stmt : TIF TLPAREN expr TRPAREN block { $$ = new NIFStatement(shared_ptr<NExpression>($3), shared_ptr<NBlock>($5), nullptr); }
	| TIF TLPAREN expr TRPAREN block TELSE block { $$ = new NIFStatement(shared_ptr<NExpression>($3), shared_ptr<NBlock>($5), shared_ptr<NBlock>($7)); }
	;


assume_stmt
    : TASSUME TLPAREN predicates TRPAREN { $$ = new NAssumeStatement(shared_ptr<NExpressionList>($3));}
    ;
predicates
    : predicate {$$ = new NExpressionList(); $$->push_back(shared_ptr<NExpression>($1));}
    | predicates TLAND predicate {$1->push_back(shared_ptr<NExpression>($3));}
    ;
predicate
    : TDOM TLPAREN array_index TRPAREN TSLASH TRVAR TLPAREN array_index TRPAREN TNOTEQ TEMPTY
        { $$ = new DomPredicate(shared_ptr<NArrayIndex>($3), shared_ptr<NArrayIndex>($8)); }
    | TRVAR TLPAREN array_index TRPAREN  TINTERSECT TRVAR TLPAREN array_index TRPAREN TCEQ TEMPTY
        { $$ = new RvalPredicate(shared_ptr<NArrayIndex>($3), shared_ptr<NArrayIndex>($8)); }
    | array_index TCOLON node_type
        { $$ = new TypePredicate(shared_ptr<NArrayIndex>($1), shared_ptr<NIdentifier>($3)); }
    ;
node_type
    : TSI {$$ = new NIdentifier(*$1); $$->isType = true;  delete $1;}
    | TLK {$$ = new NIdentifier(*$1); $$->isType = true;  delete $1;}
    | TUF {$$ = new NIdentifier(*$1); $$->isType = true;  delete $1;}
    | TUK {$$ = new NIdentifier(*$1); $$->isType = true;  delete $1;}
    ;
%%
