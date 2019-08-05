#include "packagesatsolver.h"
#include <QDebug>

PackageSatSolver::PackageSatSolver()
{

}

/**
* @brief Преобразуем из (a&&b) || (c&&d) в (a || c) && (a || d) && (b||c) && (b||d)
* @return
*/
PackageSatSolver::TConjuctiveCond PackageSatSolver::conjuction(PackageSatSolver::TDisjuctiveCond disj)
{
    QList<int> regs;
    for(int i=0; i<disj.count(); ++i) regs.append(0);

    TConjuctiveCond conjs;
    QList<int> conj;
    while ( regs[0]<disj[0].count() ) {
        for (int i=0; i<regs.count(); ++i) {
            conj.append(disj[i][regs[i]]);
        }
        conjs.append(conj);
        conj.clear();
        regs[regs.count()-1]++;
        for (int i=regs.count()-1; i>=0; --i) {
            if ( (regs[i]>= disj[i].count()) && (i-1>=0) ) {
                regs[i]=0;
                regs[i-1]++;
            }
        }
    }
    return  conjs;
}


/**
* @brief Создаём условия по каждому пакету и его версии
* @param cnd
* @param indexes
* @param packetVersions
* @param conds
* @return
*/
int PackageSatSolver::makePackConds(PackadgeCandidate *cnd, QList<QString> &indexes, QHash<QString, QList<QString> > &packetVersions, QHash<QString, PackageSatSolver::TCondPacket> &conds, QList<QString> &excludes)
{
    if ( packetVersions[cnd->name()].contains(cnd->fullName()) ) { return  1; }

    qInfo()<<"Make pack conds"<<cnd->fullName();

    packetVersions[cnd->name()].append(cnd->fullName());

    QList<QList<int> > packRelsIndexes;

    int packIndex = indexes.indexOf(cnd->fullName());
    if ( packIndex==-1 ) {
        indexes.append(cnd->fullName());
        packIndex = indexes.count()-1;
    }

    packRelsIndexes.append(QList<int>({packIndex}));

    PackadgeCandidate::TRels rels = cnd->relatives();
    QStringList relsPacketNames = rels.keys().toSet().toList(); //-- Удаляем дубликаты

    foreach(QString rName, relsPacketNames) {

        //-- Соберём возможные версии
        QList<int> relIndexes;
        QList<PackadgeCandidate*> relCnds = rels.values(rName);

        foreach(PackadgeCandidate * relCnd, relCnds) {            

            excludes.removeAll(relCnd->fullName()); //-- Если пакет у кого-то в зависимостях, то убираем его из нежелательных


            int relVerIndex = indexes.indexOf(relCnd->fullName());
            if ( relVerIndex==-1 ) {
                indexes.append(relCnd->fullName());
                relVerIndex = indexes.count()-1;
            }
            relIndexes.append(relVerIndex);

            int subPackConds = makePackConds(relCnd, indexes, packetVersions, conds, excludes);
            if ( subPackConds<0 ) return subPackConds;

        }

        if ( relIndexes.count()>0 ) packRelsIndexes.append(relIndexes);
    }

    if ( packRelsIndexes.count()>1 ) conds[cnd->name()].append(packRelsIndexes);

    return  1;
}



/**
* @brief Самое весёлое. Решаем зависимости, т.е. что будем устанавливать в итоге
* @param instPacks - установленные на данный момент пакеты
* @param toInstPacks - отвечаем, какие пакеты следует установить
* @return
*/
int PackageSatSolver::prepareInstList(const QHash<QString, Packadge*> & instPacks, QList<PackadgeCandidate*> &toInstPacks)
{
    qInfo()<<"Solve relatives";

    //-- Пробегаемся по всем кандидатам на обновление и каждому присваиваем индекс для SAT solver и создаём условия
    QList<QString> indexes;
    QHash<QString, TCondPacket> conds; //-- Имя пакета, список ИЛИ список И список ИЛИ
    QHash<QString, QList<QString> > packetVersions; //-- Для каждого имени пакета запомним все возможные версии

    //-- Укажем, что установленные версии пакетов, у которых есть кондидаты на обновления, не желательны
    QList<QString> excludes;
    foreach(Packadge * instPack, instPacks) {
        if ( instPack->candidates().count()==0 ) continue;
        excludes.append(instPack->fullName());
    }

    qDebug()<<"Excludes before"<<excludes;
    foreach(Packadge * pack, instPacks) {

        makePackConds(pack, indexes, packetVersions, conds, excludes);

        Packadge::TCandidates cnds =  pack->candidates();
        foreach(PackadgeCandidate * cnd, cnds) {
            makePackConds(cnd, indexes, packetVersions, conds, excludes);
        }
    }


    //-- Подготавливаем всё к решению
    Solver solver;

    qInfo()<<"Variables:";

    //-- Создаём переменные
    for(int i=0; i<indexes.count(); ++i) {
        qInfo()<<"Var"<<indexes.at(i)<<i;
        solver.newVar();
    }

    //-- Делаем условие в формате conjunctive normal form (Всего вариантов будет количество переменных * количество ИЛИ)
    qInfo()<<"Conditions:";
    foreach(TCondPacket cond, conds) {
        qInfo()<<"Cond"<<cond;

        TDisjuctiveCond packetCondDj; //-- Собираем ИЛИ по каждой версии пакета

        foreach(TDisjuctiveCond disj, cond) {
            TConjuctiveCond packetVerCondCj =  conjuction(disj);
            packetCondDj.append( packetVerCondCj );
        }

        TConjuctiveCond packetCondCj = conjuction(packetCondDj); //TODO: Упрощать

        foreach(QList<int> condCjOr, packetCondCj) {
            vec<Lit> lits;
            foreach(int lit, condCjOr) {
                lits.push(mkLit(lit));
            }
            solver.addClause_(lits);
        }
    }

    qInfo()<<"Excludes"<<excludes;
    //-- Добавляем отметку о нежелательных
    foreach(QString exc, excludes) { //TODO: Отптимизировать и условия по такому пакету не собирать
        vec<Lit> lits;
        int excIndx = indexes.indexOf(exc);
        lits.push(~mkLit(excIndx));
        solver.addClause_(lits);
    }

    //-- В условия добавим, что от каждого пакета нам нужна лишь одна версия
    foreach(QList<QString> vers, packetVersions) {

        //-- Cопоставим каждой версии индекс
        QList<int> versionsIndx;
        foreach(QString packVer, vers) {
            versionsIndx.append(indexes.indexOf(packVer));
        }
        //-- Создаём условия, к примеру у пакета есть версии 1 2 3 4, то что бы был только один должно быть: -1 -2, -1 -3, -1 -4, -2 -3, -2 -4, -3 -4
        vec<Lit> lits;
        foreach(int ver, versionsIndx) {
            lits.push(mkLit(ver));
        }
        solver.addClause_(lits);
        for(int i=0; i< versionsIndx.count(); ++i) {
            int litF = versionsIndx.at(i);
            for(int j=i+1; j<versionsIndx.count(); ++j) {
                int litN = versionsIndx.at(j);
                vec<Lit> lits;
                lits.push(~mkLit(litF));
                lits.push(~mkLit(litN));
                solver.addClause_(lits);
            }
        }
    }
    qInfo()<<"Variables:"<<solver.nVars();
    qInfo()<<"Clauses:"<<solver.nClauses();

    if ( !solver.simplify() ) {
        qWarning()<<"Not simplify!";
        return -2;
    }

    QList<bool> toInstIndexes;
    vec<Lit> dummy;
    lbool ret = solver.solveLimited(dummy);
    if ( ret == l_True) {
        qInfo()<<"SAT!";
        for (int i=0; i<solver.nVars(); ++i) {
            toInstIndexes.append(solver.model[i]==l_True);
            qInfo()<<((solver.model[i]==l_True)?"+":"-")<<i;
        }
    } else
    if (ret == l_False) {
        qWarning()<<"UNSAT";
        return -3;
    } else {
        qWarning()<<"INDET";
        return -4;
    }

    //-- Создаём список для установки
    foreach(Packadge * pack, instPacks) {
        Packadge::TCandidates cnds =  pack->candidates();        
        foreach(PackadgeCandidate * cnd, cnds) {
            toInstPacks.append(makeInstallList(cnd, indexes, instPacks, toInstIndexes));
        }
    }

    return 1;
}


/**
* @brief Создаём список кандидатов на установку
* @param cnd
* @param indexes
* @param toInstIndexes
* @return
*/
QList<PackadgeCandidate *> PackageSatSolver::makeInstallList(PackadgeCandidate * cnd, const QList<QString> &indexes, const QHash<QString, Packadge*> & instPacks, QList<bool> &toInstIndexes)
{
    QList<PackadgeCandidate *> instList;
    int cndIndex = indexes.indexOf(cnd->fullName());
    if ( !toInstIndexes[cndIndex] ) return instList;
    toInstIndexes[cndIndex] = false; //-- Что бы ещё раз не заинсталлили
    if ( !instPacks.contains(cnd->fullName()) ) {
        instList.append(cnd);
    }
    foreach(PackadgeCandidate* relCnd, cnd->relatives()) {
        instList.append( makeInstallList(relCnd, indexes, instPacks, toInstIndexes) );
    }
    return  instList;
}
