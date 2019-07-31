#include "packagecandidate.h"
#include <QDebug>

PackadgeCandidate::PackadgeCandidate(QString name, QString version, QSettings &cnf)
    : PackadgeInfo (name, version, cnf)
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

PackadgeCandidate::TRels PackadgeCandidate::relatives() const
{
    return _relatives;
}

/**
* @brief Выясняем зависимости
* @param updateCnf
* @return
*/
int PackadgeCandidate::parseRels(QSettings &updateCnf)
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

            PackadgeCandidate * cnd = new PackadgeCandidate(relNameVersion.at(0), relNameVersion.at(1), updateCnf);
            addRel(cnd);

            int parseSubRels = cnd->parseRels(updateCnf);
            if ( parseSubRels<0 ) return parseSubRels;

        }
        qInfo()<<"Rels for packet"<<fullName()<<" parsed.";
    } else {
        qInfo()<<"Rels empty.";
    }

    return 1;
}
