#ifndef PACKAGESATSOLVER_H
#define PACKAGESATSOLVER_H

#include "packadge.h"
#include "minisat/core/Solver.h"
#include <QList>
#include <QString>

using namespace Minisat;

/**
* @brief Выясняем, кто будет установлен
*/
class PackageSatSolver
{
public:
    PackageSatSolver();

    typedef QList<QList< int> > TDisjuctiveCond;
    typedef QList<QList< int> > TConjuctiveCond;
    TConjuctiveCond conjuction(TDisjuctiveCond disj);

    typedef QList<QList<QList<int> > > TCondPacket;
    int makePackConds(PackadgeCandidate * cnd, QList<QString> &indexes, QHash<QString, QList<QString> > & packetVersions, QHash<QString, TCondPacket> &conds);

    int prepareInstList(const QHash<QString, Packadge*> & instPacks, QList<PackadgeCandidate *> &toInstPacks);

protected:
    QList<PackadgeCandidate*> makeInstallList(PackadgeCandidate *cnd, const QList<QString> &indexes, QList<bool> &toInstIndexes);

};

#endif // PACKAGESATSOLVER_H
