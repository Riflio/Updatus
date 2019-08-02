#ifndef UPDATER_H
#define UPDATER_H

#include <QObject>
#include <QSettings>

#include "packadge.h"
#include "downloadmanager.h"

/**
* @brief Обновляем/устанавливаем пакеты физически
*/


class PackadgeCandidateUpdater: public QObject, public PackadgeCandidate
{
    Q_OBJECT
public:
    explicit PackadgeCandidateUpdater(const PackadgeCandidate&other);

    enum UpdateStatus {
        US_NONE=2,
        US_REQUESTED=4,
        US_DOWNLOADED=8,
        US_INSTALLED=16
    };

    void addStatus(int status);
    int status() const;

    void download();

private slots:
    void onDownloadComplete(QTemporaryFile * packetFile);

signals:
    void packageDownloaded(PackadgeCandidateUpdater * pack, QTemporaryFile * packetFile);

private:
    int _status;
    DownloadManager * _dlMr;

};


class updater : public QObject
{
    Q_OBJECT
public:
    explicit updater(QObject *parent, QSettings * mainCnf);
    int goInstall(const QList<PackadgeCandidate*> &instList);


signals:
    void error();

public slots:

private slots:
    void onPacketDownloaded(PackadgeCandidateUpdater * pack, QTemporaryFile * packetFile);

private:
    void allPacketsDownloaded();
    void removePrevPacks();

private:    
    QHash<QString, PackadgeCandidateUpdater*> _updaterPackages;
    QSettings * _mainCnf;
};

#endif // UPDATER_H
