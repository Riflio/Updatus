#ifndef PACKADGECANDIDATEUPDATER_H
#define PACKADGECANDIDATEUPDATER_H

#include <QObject>
#include <QTemporaryFile>
#include "packagecandidate.h"
#include "downloadmanager.h"

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

    void download(int downloadAttempts);

    int downloadProgress() const;

private slots:
    void onDownloadComplete(QTemporaryFile * packetFile);
    void onDownloadError(QString err);
    void onDownloadProgress(int pr, int bytes);

signals:
    void error();
    void packageDownloaded(PackadgeCandidateUpdater * pack);

private:
    int _status;
    DownloadManager * _dlMr;
    QString _cachePacketPath;
    QString _tempDir;
    int _dwnProgress;

};

#endif // PACKADGECANDIDATEUPDATER_H
