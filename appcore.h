#ifndef APPCORE_H
#define APPCORE_H

#include <QObject>
#include <QSettings>

#include "packadge.h"
#include "downloadmanager.h"
#include "packagesatsolver.h"
#include "updater.h"

class AppCore : public QObject
{
    Q_OBJECT
public:
    explicit AppCore(QObject *parent = nullptr);
    ~AppCore();

    bool upgrade(QString mainCnfPath);

    int collectInstalledPackadges();
    int collectAvaliableUpdates();

    int goInstall(const QList<PackadgeCandidate*> & instList);


signals:
    void error();

public slots:
    void onComplete(bool newInstalled);

private slots:
    void onUpdtCnfDownloaded(QTemporaryFile * cnfFile);

private:
    QSettings * _mainCnf;
    QHash<QString, Packadge*> _instPacks; //-- Установленные пакеты
    QHash<QString, PackadgeCandidate*> _instCandidates; //-- Все возможные кондидаты на установку

    DownloadManager * _collectUpdtCnfManager;
    PackageSatSolver * _packageSatSolver;
    Updater * _updater;
};

#endif // APPCORE_H

/*
Example:
[general]
version=1.0
sendLogs=2me@pavelk.ru
lastUpdated=123

[info]
platform=linux
uuid=1239sdf124e

[servers]
main="http://suvenirus.dev/updatemanager/"

#What packadges need to be installed, after first update replaced by latest versions
[installed]
packetA="0"
packetB="0"
packetC="0"
packetD="0"

#path to install and other info
[packetA:0]
path="tests/packetA.txt"

[packetB:0]
path="tests/packetB.txt"

[packetC:0]
path="tests/packetC.txt"

[packetD:0]
path="tests/packetD.txt"

*/
