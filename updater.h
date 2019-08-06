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
    void onDownloadError(QString err);

signals:
    void error();
    void packageDownloaded(PackadgeCandidateUpdater * pack);

private:
    int _status;
    DownloadManager * _dlMr;
    QString _cachePacketPath;
    QString _tempDir;

};


class Updater : public QObject
{
    Q_OBJECT
public:
    explicit Updater(QObject *parent, QSettings * mainCnf);
    int goInstall(const QList<PackadgeCandidate*> &instList);

    void goError();
    void goComplete(bool newInstalled);

signals:
    void error();
    void completed(bool newInstalled);

public slots:

private slots:
    void onPacketDownloaded(PackadgeCandidateUpdater * pack);
    void onPacketDownloadError();

private:
    void allPacketsDownloaded();
    void removeOldInstallNew();

private:    
    QHash<QString, PackadgeCandidateUpdater*> _updaterPackages;
    QSettings * _mainCnf;
    bool _hasError;

};

#endif // UPDATER_H
