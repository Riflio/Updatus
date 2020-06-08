#ifndef UPDATER_H
#define UPDATER_H

#include <QObject>
#include <QSettings>
#include <QTimer>

#include "packadge.h"
#include "downloadmanager.h"
#include "packadgecandidateupdater.h"

/**
* @brief Обновляем/устанавливаем пакеты физически
*/

class Updater : public QObject
{
    Q_OBJECT
public:
    explicit Updater(QObject *parent, QSettings * mainCnf);
    int goInstall(const QList<PackadgeCandidate*> &instList);

    void goError();
    void goComplete(bool newInstalled);

    static bool checkFileAccess(QString path);
    static bool checkFolderAccess(QString path);

signals:
    void error();
    void completed(bool newInstalled);
    void progress(int pr);

public slots:

private slots:
    void onPacketDownloaded(PackadgeCandidateUpdater * pack);
    void onPacketDownloadError();
    void recalcDownloadProgress();

private:
    void allPacketsDownloaded();
    void removeOldInstallNew();

private:
    typedef QHash<QString, PackadgeCandidateUpdater*> TUpdaterPackages;
    TUpdaterPackages _updaterPackages;
    QSettings * _mainCnf;
    bool _hasError;
    QTimer _recalcDwnPrTr;
    long _totalProgress;
    int _downloadStreams;
    int _downloadAttempts;

};

#endif // UPDATER_H
