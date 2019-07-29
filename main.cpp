#include <QCoreApplication>

#include "minisat/core/Solver.h"
#include "appcore.h"

#include <QDebug>

using namespace Minisat;


void addVariable(Solver& s, int v)
{
    int var = abs(v)-1;
    while (var >= s.nVars()) s.newVar();
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    /*
    Solver solver;

    addVariable(solver, 1);
    addVariable(solver, 2);
    addVariable(solver, 3);
    addVariable(solver, 4);
    addVariable(solver, 5);
    addVariable(solver, 6);
    addVariable(solver, 7);
    addVariable(solver, 8);
    addVariable(solver, 9);

    //(2 || 3 || 4) && (2 || 3 || 8) && (3 || 4 || 7) && 5 && (7 || 8) && 9

    vec<Lit> lits;

    lits.clear();
    lits.push(mkLit(2-1));
    lits.push(mkLit(3-1));
    lits.push(mkLit(4-1));
    solver.addClause_(lits);


    lits.clear();
    lits.push(mkLit(2-1));
    lits.push(mkLit(3-1));
    lits.push(mkLit(8-1));
    solver.addClause_(lits);

    lits.clear();
    lits.push(mkLit(3-1));
    lits.push(mkLit(4-1));
    lits.push(mkLit(7-1));
    solver.addClause_(lits);

    lits.clear();
    lits.push(mkLit(5-1));
    solver.addClause_(lits);

    lits.clear();
    lits.push(mkLit(9-1));
    solver.addClause_(lits);

    lits.clear();
    lits.push(mkLit(7-1));
    lits.push(mkLit(8-1));
    solver.addClause_(lits);


    printf("|  Number of variables:  %12d                                         |\n", solver.nVars());
    printf("|  Number of clauses:    %12d                                         |\n", solver.nClauses());

    if ( !solver.simplify() ) {
        printf("===============================================================================\n");
        printf("Solved by unit propagation\n");
        solver.printStats();
        printf("\n");
        printf("UNSATISFIABLE\n");
        exit(20);
    }


    vec<Lit> dummy;
    lbool ret = solver.solveLimited(dummy);
    if ( ret == l_True) {
        printf("SAT!\n");
        for (int i = 0; i < solver.nVars(); i++) {
            if ( solver.model[i] != l_Undef ) {
                printf("%s%s%d", (i==0)?"":" ", (solver.model[i]==l_True)?"":"-", i+1);
            }
        }
    } else
    if (ret == l_False) {
        printf("UNSAT\n");
    } else {
        printf("INDET\n");
    }


    printf("\n");
*/
    AppCore * core = new AppCore(0);

    return a.exec();
}
