#include "packageinfo.h"
#include <QDebug>

PackadgeInfo::PackadgeInfo(QString name, QString version, QSettings &cnf)
    : _name(name), _version(version)
{
    _fullName = makeFullName(_name, _version);
    _versionInt = versionStr2Int(_version);

    _path = cnf.value(QString("%1/path").arg(fullName())).toString();
}

QString PackadgeInfo::path() const
{
    return _path;
}

QString PackadgeInfo::pathDir() const
{
    return path().left(path().lastIndexOf("/")+1);
}

QString PackadgeInfo::pathFileName() const
{
    return path().right(path().length()-path().lastIndexOf("/")-1);
}

QString PackadgeInfo::name() const
{
    return _name;
}

QString PackadgeInfo::version() const
{
    return _version;
}

int PackadgeInfo::versionInt() const
{
    return _versionInt;
}

QString PackadgeInfo::fullName() const
{
    return  _fullName;
}
