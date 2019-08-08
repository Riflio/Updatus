#ifndef APPCORE_H
#define APPCORE_H

#include <QObject>
#include <QSettings>

#include "packadge.h"
#include "downloadmanager.h"
#include "packagesatsolver.h"
#include "updater.h"
#include "logger.h"
#include "mainwindow.h"

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

    void withGui();

signals:
    void error();

public slots:
    void onComplete(bool newInstalled);
    void onError();

private slots:
    void onUpdtCnfDownloaded(QTemporaryFile * cnfFile);
    void onUpdateCndDownloadError(QString err);

private:
    QSettings * _mainCnf;
    QHash<QString, Packadge*> _instPacks; //-- Установленные пакеты
    QHash<QString, PackadgeCandidate*> _instCandidates; //-- Все возможные кондидаты на установку

    DownloadManager * _collectUpdtCnfManager;
    PackageSatSolver * _packageSatSolver;
    Updater * _updater;

    QMap<QString, QVariant> _infoVariables; //-- Собираем всё, что есть у [info] в файле конфига

    MainWindow * _mainWindow;
    bool _hasError;

};

#endif // APPCORE_H

/*
Example:
updateManager.cnf

[General]
defaultInstPath=tests #install directory if not set instPath about every packet
lastUpdated=07.08.19 01:23 #date and time last seccessful update
sendLogs=http://site.dev/updatus/handler/logHandler.php #url to send logs (see ServerHandler directory)
tempDir=./updateCache/ #dir to store updates
version=1.0 #this version

[servers] #Servers list, with updates (see repository.cnf)
main=http://suvenirus.dev/updatemanager/ #At now version, only one avaliable

[info] #All values will be send to server log handler
platform=linux
uuid=1239sdf124e

[installed] #What packadges need to be installed, after first update replaced by latest versions
packetA=2.0
packetB=1.9
packetC=4.0
packetD=9.0
packetF=22.4

[packetA:2.0]
cachePath=./updateCache/hcsterminal/linux/packetA/2.0/packetA.zip #Where placed this version
instPath=tests #Where should unpack this packet (and all new versions)
instType=asManual #Installation type. asRels may be deleted, if nobody has used hes
rels=  #Relatives, format:  "name1:version1;name2:version2"

[packetB:1.9]
cachePath=./updateCache/hcsterminal/linux/packetB/1.9/packetB.zip
instPath=tests
instType=asManual
rels=packetF:22.4

[packetC:4.0]
cachePath=./updateCache/hcsterminal/linux/packetC/4.0/packetC.zip
instPath=tests
instType=asManual
rels="packetA:3.0;packetA:2.0;packetB:1.9"

[packetD:9.0]
cachePath=./updateCache/hcsterminal/linux/packetD/9.0/packetD.zip
instPath=tests
instType=asManual
rels="packetA:2.0;packetA:1.0;packetC:4.1;packetC:4.0"

[packetF:2.4]
cachePath=./updateCache/hcsterminal/linux/packetF/22.4/packetF.zip
instPath=tests
instType=asRels
rels=packetA:2.0

*/
