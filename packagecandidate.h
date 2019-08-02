#ifndef PACKAGECANDIDATE_H
#define PACKAGECANDIDATE_H

#include <QString>
#include <QSettings>
#include "packageinfo.h"

/**
* @brief Кандидат на замену/установку
*/
class Packadge;
class PackadgeCandidate: public PackadgeInfo
{
public:
    explicit PackadgeCandidate(QString name, QString version, QSettings &cnf, Packadge * originalPackage);
    ~PackadgeCandidate();

    typedef QMultiHash<QString, PackadgeCandidate*> TRels;
    TRels relatives() const;

    int parseRels(QSettings&updateCnf, QHash<QString, PackadgeCandidate *> *instCandidates);
    void addRel(PackadgeCandidate *cnd);

    QString downloadUrl() const;
    Packadge * originalPackage() const;

private:
    TRels _relatives;
    QString _updtServer;
    Packadge * _originalPackage;

};
#endif // PACKAGECANDIDATE_H
