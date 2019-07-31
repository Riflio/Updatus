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
    QString name() const;
    QString version() const;
    int versionInt() const;
    QString fullName() const;

    static inline QString makeFullName(QString name, QString version) { return QString("%1:%2").arg(name).arg(version); }
    static inline int versionStr2Int(QString version) { return version.replace(".", "").toInt(); }

private:
    QString _name;
    QString _path;
    QString _version;
    QString _fullName;
    int _versionInt;

};

#endif // PACKAGEINFO_H
