#ifndef PACKAGECANDIDATE_H
#define PACKAGECANDIDATE_H

#include <QString>
#include <QSettings>
#include "packageinfo.h"

/**
* @brief Кандидат на замену/установку
*/
class PackadgeCandidate: public PackadgeInfo
{
public:
    explicit PackadgeCandidate(QString name, QString version, QSettings &cnf);
    ~PackadgeCandidate();

    typedef QMultiHash<QString, PackadgeCandidate*> TRels;
    TRels relatives() const;

    int parseRels(QSettings&updateCnf);
    void addRel(PackadgeCandidate *cnd);

    QString downloadUrl() const;

private:
    TRels _relatives;
    QString _updtServer;

};
#endif // PACKAGECANDIDATE_H
