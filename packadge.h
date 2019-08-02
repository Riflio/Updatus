#ifndef PACKADGE_H
#define PACKADGE_H

#include <QString>
#include <QHash>
#include <QSettings>
#include "packageinfo.h"
#include "packagecandidate.h"

/**
* @brief Установленный пакет
*/
class Packadge: public PackadgeInfo
{
public:
    Packadge(QString name, QString version, QSettings &cnf);
    ~Packadge();

    typedef QHash<QString, PackadgeCandidate*> TCandidates;
    TCandidates candidates() const;

    int parseUpdates(QSettings&updateCnf,  QHash<QString, PackadgeCandidate*> * instCandidates);
    void addCandidate(PackadgeCandidate *candidate);

    QString instType() const;

private:
    TCandidates _candidates; //--  Кандидаты на обновление
    QString _instType;
};

#endif // PACKADGE_H
