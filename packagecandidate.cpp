#include "packagecandidate.h"
#include "packadge.h"

#include <QDebug>

PackadgeCandidate::PackadgeCandidate(QString name, QString version, QSettings &cnf, Packadge * originalPackage)
    : PackadgeInfo (name, version, cnf), _originalPackage(originalPackage)
{
    _updtServer = cnf.value("servers/main").toString();
    if (_updtServer.right(1)!="/") _updtServer.append("/");
}

PackadgeCandidate::~PackadgeCandidate()
{
    qDeleteAll(_relatives);
}

void PackadgeCandidate::addRel(PackadgeCandidate * cnd)
{
    qInfo()<<"Add relative"<<cnd->fullName();
    _relatives.insertMulti(cnd->name(), cnd);
}

QString PackadgeCandidate::downloadUrl() const
{
    return  _updtServer+path();
}

/**
* @brief Отдаём пакет, которому мы на замену
* @return
*/
Packadge *PackadgeCandidate::originalPackage() const
{
    return  _originalPackage;
}

PackadgeCandidate::TRels PackadgeCandidate::relatives() const
{
    return _relatives;
}

/**
* @brief Выясняем зависимости
* @param updateCnf
* @return
*/
int PackadgeCandidate::parseRels(QSettings &updateCnf, QHash<QString, PackadgeCandidate *> *instCandidates)
{
    qInfo()<<"Parse packet rels"<<fullName();

    QString relsStr = updateCnf.value(QString("%1/rels").arg(fullName())).toString();
    if ( !relsStr.isEmpty() ) {
        QStringList rels = relsStr.split(";");
        foreach(QString rel, rels) {

            QStringList relNameVersion = rel.split(":"); //TODO: Добавить возможность учитывать диапазон, а не только конкретную версию
            if ( relNameVersion.count()!=2 ) {
                qWarning()<<"Wrong format rels"<<relNameVersion;
                continue;
            }

            QString cndFullName = PackadgeCandidate::makeFullName(relNameVersion.at(0), relNameVersion.at(1));
            PackadgeCandidate * cnd = nullptr;

            if ( !instCandidates->contains(cndFullName) ) {
                cnd = new PackadgeCandidate(relNameVersion.at(0), relNameVersion.at(1), updateCnf, nullptr);
                int parseSubRels = cnd->parseRels(updateCnf, instCandidates);
                instCandidates->insert(cndFullName, cnd);
                if ( parseSubRels<0 ) return parseSubRels;
            } else {
                cnd = instCandidates->value(cndFullName);
            }

            addRel(cnd);
        }
        qInfo()<<"Rels for packet"<<fullName()<<" parsed.";
    } else {
        qInfo()<<"Rels empty.";
    }

    return 1;
}
