#ifndef APPCORE_H
#define APPCORE_H

#include <QObject>
#include <QSettings>

#include "packadge.h"
#include "downloadmanager.h"
#include "packagesatsolver.h"
#include "updater.h"
#include "logger.h"

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
    void onError();

private slots:
    void onUpdtCnfDownloaded(QTemporaryFile * cnfFile);

private:
    QSettings * _mainCnf;
    QHash<QString, Packadge*> _instPacks; //-- Установленные пакеты
    QHash<QString, PackadgeCandidate*> _instCandidates; //-- Все возможные кондидаты на установку

    DownloadManager * _collectUpdtCnfManager;
    PackageSatSolver * _packageSatSolver;
    Updater * _updater;

    QMap<QString, QVariant> _infoVariables; //-- Собираем всё, что есть у [info] в файле конфига
};

#endif // APPCORE_H

/*
Example:

#What packadges need to be installed, after first update replaced by latest versions

#path to install and other info

*/
