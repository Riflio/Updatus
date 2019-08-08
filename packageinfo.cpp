#include "packageinfo.h"
#include <QDebug>

PackadgeInfo::PackadgeInfo(QString name, QString version, QSettings &cnf)
    : _name(name), _version(version)
{
    _fullName = makeFullName(_name, _version);
    _versionInt = versionStr2Int(_version);

    _path = cnf.value(QString("%1/instPath").arg(fullName())).toString();

    QString instTypeStr = cnf.value(QString("%1/instType").arg(fullName()), "").toString();

    if ( instTypeStr=="asManual" ) {
        _instType = asManual;
    } else
    if ( instTypeStr=="asRels" ) {
        _instType = asRels;
    } else {
        _instType = asManual;
    }

    _fileSize = cnf.value(QString("%1/size").arg(fullName()), -1).toInt();
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


/**
* @brief Отдаём каким способом мы установлены (вручную или как зависимость кого-либо)
* @return
*/
int PackadgeInfo::instType() const
{
    return _instType;
}

long PackadgeInfo::fileSize() const
{
    return _fileSize;
}

QString PackadgeInfo::makeFullName(QString name, QString version)
{
    return QString("%1:%2").arg(name).arg(version);
}

int PackadgeInfo::versionStr2Int(QString version)
{
    return version.replace(".", "").toInt();
}

QString PackadgeInfo::instTypeStr(int type)
{
    switch (type) {
        case asRels: return  "asRels";
        case asManual: return "asManual";
        default: return "asManual";
    }
}
