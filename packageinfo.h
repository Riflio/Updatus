#ifndef PACKAGEINFO_H
#define PACKAGEINFO_H

#include <QString>
#include <QSettings>

/**
* @brief Основная информация о пакете
*/
class PackadgeInfo
{
public:
    PackadgeInfo(QString name, QString version, QSettings &cnf);

    QString path() const;
    QString pathDir() const;
    QString pathFileName() const;
    QString name() const;
    QString version() const;
    int versionInt() const;
    QString fullName() const;

    int instType() const;

    enum instTypes {
        asManual =2,
        asRels = 4
    };

    static QString makeFullName(QString name, QString version);
    static int versionStr2Int(QString version);
    static QString instTypeStr(int type);

private:
    QString _name;
    QString _path;
    QString _version;
    QString _fullName;
    int _versionInt;
    int _instType;

};

#endif // PACKAGEINFO_H
