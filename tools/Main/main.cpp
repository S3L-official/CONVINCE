#include <iostream>
#include "ASTNode.h"
#include "Interpreter.h"
#include "HOChecker.h"
#include "Transformer.h"
#include "Inliner.h"

using namespace std;
extern int yyparse();
extern int yydebug;
extern FILE *yyin;
extern int yylineno;
extern shared_ptr<ASTNode::NBlock> programRoot;


int main(int argc, char **argv) {

    setenv("DEBUG", "1", 1);

    int order = stoi(argv[3]);
    string checktype(argv[5]);
    int isTest = stoi(argv[6]);
    string procName(argv[4]);
    string benchname(argv[2]);
    string pathFrom(argv[1]);
    string path = pathFrom + "/order" + to_string(order) + "/" + benchname;
    cout << path << endl;
    yyin = fopen(path.c_str(), "r");
    yylineno = 1;
    vector<ProcValuePtr> res;
    Interpreter interpreter;
    if (!yyparse()) {
        cout << "Parsing complete\n" << endl;
        interpreter.assumptionLevel = 3;
        interpreter.generateCode(*programRoot);
        res = interpreter.getProcs();
    } else {
        assert(false);
    }


    Transformer transformer(res);
    transformer.transformProcedures();

    Inliner inliner(transformer.getProcedureHs(), order);
    inliner.inlineProcedures();

//    inliner.printMaskverif();

//    for(auto ele : inliner.getNameToInlinedProcedure().at("main")->getBlock()) {
//        cout << ele->prettyPrint4() << endl;
//    }



    HOChecker checkHigh(transformer.getProcedureHs(), order, inliner);
    checkHigh.nameOfKey = interpreter.getNameOfKey();
    checkHigh.nameOfPlain = interpreter.getNameOfPlain();
    HOChecker::isTest = isTest;

    clock_t s1 = clock();
    if(checktype == "sni") {
        try {
            checkHigh.checkSNI(procName, order);
        } catch (set<ThreeAddressNodePtr> O) {
            cout << "This masked implementation is *not* SNI" << endl;
        }
    } else if(checktype == "ni") {
        try {
            checkHigh.checkNI(procName, order);
            cout << "This masked implementation is NI" << endl;
        } catch (set<ThreeAddressNodePtr> O) {
            cout << "This masked implementation is *not* NI" << endl;
        }
    } else if(checktype == "prob") {
        checkHigh.checkProbing(order);
    } else {
        cout << "unknown" << endl;
    }

    clock_t s2 = clock();
    cout << "Time: " << ((double) (s2 - s1)) / CLOCKS_PER_SEC << endl;

    return 0;
}
