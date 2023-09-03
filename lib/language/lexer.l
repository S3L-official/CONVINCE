%{
#include <stdio.h>
#include <string>
#include "ASTNode.h"
#include "parser.h"
extern int yylineno;
void yyerror(const char* s, ...);
#define SAVE_TOKEN yylval.string = new std::string(yytext);
%}

%option noyywrap nodefault yylineno

digs (-)?[0-9]+
chars [a-zA-Z_]+


%%
"#"[^\n]*                      { /* # as comments */ }
[ \t\n\r]                      ; //ignores spaces
"for"			               { return TFOR; }
"if"			               { return TIF; }
"else"			               { return TELSE; }
"public"		               { SAVE_TOKEN; return TYPUBLIC; }
"private"		               { SAVE_TOKEN; return TYPRIVATE; }
"internal"		               { SAVE_TOKEN; return TYINTERNAL; }
"int"                          { SAVE_TOKEN; return TYINT; }
"void"                         { SAVE_TOKEN; return TYVOID; }
"share"                        { SAVE_TOKEN; return TYSHARE; }
"random"                        { SAVE_TOKEN; return TYRANDOM; }

"POW2"                        {  return TPOW2; }
"POW4"                        {  return TPOW4; }
"POW16"                        {  return TPOW16; }
"AFFINE"                        {  return TAFFINE; }
"SBOX"                        {  return TSBOX; }
"tableLUT"                        {  return TTABLELUT; }
"tableLUT4"                        {  return TTABLELUT4; }
"RCON"                          {return TRCON;}
"XTIMES"                          {return TXTIMES;}
"POL"                          {return TPOL;}
"LIN"                          {return TLIN;}

"tSI"                           {SAVE_TOKEN; return TSI;}
"tLK"                           {SAVE_TOKEN; return TLK;}
"tUF"                           {SAVE_TOKEN; return TUF;}
"tUK"                           {SAVE_TOKEN; return TUK;}

"Dom"                           { return TDOM;}
"RVar"                          { return TRVAR;}

"empset"                        { return TEMPTY;}
"assume"                        { return TASSUME;}

"extern"                       { return TEXTERN; }
"return"		               { return TRETURN; }
{digs}                         { SAVE_TOKEN; return TINTEGER;}
[a-zA-Z_][a-zA-Z0-9_]*         { SAVE_TOKEN; return TIDENTIFIER;}
"$"                            { return TNEWRANDOM;}
"+"                            { return TPLUS;}
"-"                            { return TMINUS;}
"*"                            { return TFFTIMES;}
"/"                            { return TDIVIDE;}
"**"                           { return TPOWER; }
","                            { return TCOMMA;}
":"                            { return TCOLON;}
";"                            { return TSEMICOLON; }
"="                            { return TASSIGN;}
"=="                           { return TCEQ;}
"<="                           { return TLE;}
"<<"                           { return TSHIFTL; }
">>"                           { return TSHIFTR; }
"~"                            { return TNOT;}
"("                            { return TLPAREN;}
")"                            { return TRPAREN;}
"["                            { return TLBRACKET; }
"]"                            { return TRBRACKET; }
"{"                            { return TLBRACE; }
"}"                            { return TRBRACE; }
"!="                           { return TNOTEQ;  }
"%"                           { return TMOD;  }
"&&"                            { return TLAND; }
"&"			                   {  return TAND; }
"|"			                   {  return TOR; }
"^"			                   {  return TXOR; }
"\\"                            {  return TSLASH; }
"∩"                            {  return TINTERSECT; }
.                              {  printf("Unknown token! %s\n", yytext);} /*complains about every unknown character*/

%%
void yyerror(const char* s, ...) {

}
