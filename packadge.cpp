#include "packadge.h"
#include <QDebug>

PackadgeInfo::PackadgeInfo(QString name, QString path, int version)
    : _name(name), _path(path), _version(version)
{

}

QString PackadgeInfo::path() const
{
    return _path;
}

QString PackadgeInfo::name() const
{
    return _name;
}

int PackadgeInfo::version() const
{
    return _version;
}

//==========================================================================================================================

PackadgeCandidate::PackadgeCandidate(QString name, QString path, int version)
    : PackadgeInfo (name, path, version)
{

}

void PackadgeCandidate::addRel(QString packetName, QString version)
{
    qInfo()<<"Add relative to"<<name()<<"rel"<<packetName<<version;
    _relatives.insertMulti(packetName, version);
}

PackadgeCandidate::TRels PackadgeCandidate::relatives() const
{
    return _relatives;
}

//==========================================================================================================================

Packadge::Packadge(QString name, QString path, int version):
    PackadgeInfo (name, path, version)
{

}

void Packadge::addCandidate(PackadgeCandidate *candidate)
{
    qInfo()<<"Add candidate to"<<name()<<", current version"<<version()<<": avaliable version"<<candidate->version();
    _candidates.insert(candidate->version(), candidate);
}

Packadge::TCandidates Packadge::candidates() const
{
    return _candidates;
}
