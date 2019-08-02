#include "packadge.h"
#include <QDebug>

Packadge::Packadge(QString name, QString version, QSettings &cnf):
    PackadgeInfo (name, version, cnf)
{
    _instType = cnf.value(QString("%1/instType").arg(fullName())).toString();
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

/**
* @brief Отдаём каким способом мы установлены (вручную или как зависимость кого-либо)
* @return
*/
QString Packadge::instType() const
{
    return _instType;
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
int Packadge::parseUpdates(QSettings &updateCnf, QHash<QString, PackadgeCandidate *> *instCandidates)
{
    qInfo()<<"Parse updates for packet"<<fullName();

    QString avVersionsStr = updateCnf.value(QString("installed/%1").arg(name())).toString();

    if ( !avVersionsStr.isEmpty() ) {
        QStringList avVersions = avVersionsStr.split(";");

        if ( avVersions.count()==0 ) {
            qWarning()<<"Wrong avaliable versions format"<<avVersionsStr;
            return -1;
        }

        foreach(QString avVersion, avVersions) {

            if ( PackadgeInfo::versionStr2Int(avVersion) <= versionInt() ) continue; //-- Учитываем только более новые версии

            qInfo()<<"--new version avaliable"<<avVersion;

            QString cndFullName = PackadgeCandidate::makeFullName(name(), avVersion);
            PackadgeCandidate * candidate = nullptr;
            if ( !instCandidates->contains(cndFullName) ) {
                candidate = new PackadgeCandidate(name(), avVersion, updateCnf, this);
                instCandidates->insert(cndFullName, candidate);
                candidate->parseRels(updateCnf, instCandidates);
            } else {
                candidate = instCandidates->value(cndFullName);
                candidate->setOriginalPackage(this); //-- На случай, если уже был распаршен зависимостями
            }

            addCandidate(candidate);
        }
    }

    return 1;
}
