#include "packadge.h"
#include <QDebug>

Packadge::Packadge(QString name, QString version, QSettings &cnf):
    PackadgeCandidate(name, version, cnf, nullptr)
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
int Packadge::parseUpdates(QSettings &updateCnf, QHash<QString, PackadgeCandidate *> *instCandidates)
{
    qInfo()<<"Parse updates for packet"<<fullName();

    QString avVersionsStr = updateCnf.value(QString("updates/%1").arg(name())).toString();

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

    parseRels(updateCnf, instCandidates);

    return 1;
}
