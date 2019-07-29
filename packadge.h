#ifndef PACKADGE_H
#define PACKADGE_H

#include <QString>
#include <QHash>

/**
* @brief Основная информация о пакете
*/
class PackadgeInfo
{
public:
    PackadgeInfo(QString name, QString path, int version);

    QString path() const;
    QString name() const;
    int version() const;

private:
    QString _name;
    QString _path;
    int _version;

};

/**
* @brief Кандидат на замену
*/
class PackadgeCandidate: public PackadgeInfo
{
public:
    PackadgeCandidate(QString name, QString path, int version);

    typedef QMultiHash<QString, QString> TRels;

    void addRel(QString packetName, QString version);
    TRels relatives() const;

private:
    TRels _relatives;
};

/**
* @brief Установленный пакет
*/
class Packadge: public PackadgeInfo
{
public:
    Packadge(QString name, QString path, int version);

    typedef QHash<int, PackadgeCandidate*> TCandidates;

    void addCandidate(PackadgeCandidate *candidate);
    TCandidates candidates() const;

private:
    TCandidates _candidates; //-- Возможные обнвления

};

#endif // PACKADGE_H
