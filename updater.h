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
    explicit PackadgeCandidateUpdater(const PackadgeCandidate&other, QString tempDir);

    enum UpdateStatus {
        US_NONE=2,
        US_REQUESTED=4,
        US_DOWNLOADED=8,
        US_INSTALLED=16
    };

    void addStatus(int status);
    int status() const;

    QString cachePacketPath() const;

    void download();

private slots:
    void onDownloadComplete(QTemporaryFile * packetFile);

signals:
    void error();
    void packageDownloaded(PackadgeCandidateUpdater * pack);

private:
    int _status;
    DownloadManager * _dlMr;
    QString _cachePacketPath;
    QString _tempDir;

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
    void onPacketDownloaded(PackadgeCandidateUpdater * pack);

private:
    void allPacketsDownloaded();
    void removeOldInstallNew();

private:    
    QHash<QString, PackadgeCandidateUpdater*> _updaterPackages;
    QSettings * _mainCnf;
};

#endif // UPDATER_H
