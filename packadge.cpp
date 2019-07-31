#include "packadge.h"
#include <QDebug>

Packadge::Packadge(QString name, QString version, QSettings &cnf):
    PackadgeInfo (name, version, cnf)
{

}

Packadge::~Packadge()
{
    qDeleteAll(_candidates);
}

void Packadge::addCandidate(PackadgeCandidate *candidate)
{
    qInfo()<<"Add candidate to"<<fullName()<<" avaliable version"<<candidate->version();
    _candidates.insert(candidate->version(), candidate);
}

Packadge::TCandidates Packadge::candidates() const
{
    return _candidates;
}

/**
* @brief Находим для себя обновления
* @param updateCnf
* @return
*/
int Packadge::parseUpdates(QSettings &updateCnf)
{
    qInfo()<<"Parse updates for packet"<<fullName();

    QString avVersionsStr = updateCnf.value(QString("installed/%1").arg(name())).toString();
    QString updtServer = updateCnf.value("servers/main").toString();

    if ( !avVersionsStr.isEmpty() ) {
        QStringList avVersions = avVersionsStr.split(";");

        if ( avVersions.count()==0 ) {
            qWarning()<<"Wrong avaliable versions format"<<avVersionsStr;
            return -1;
        }

        foreach(QString avVersion, avVersions) {

            if ( PackadgeInfo::versionStr2Int(avVersion) <= versionInt() ) continue; //-- Учитываем только более новые версии

            qInfo()<<"--new version avaliable"<<avVersion;

            PackadgeCandidate * candidate = new PackadgeCandidate(name(), avVersion, updateCnf);

            candidate->parseRels(updateCnf);

            addCandidate(candidate);
        }
    }

    return 1;
}
