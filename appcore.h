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

    bool init(QString mainCnfPath);

    bool upgrade(QString mainCnfPath);

    int collectInstalledPackadges();
    int collectAvaliableUpdates();

    int goInstall(const QList<PackadgeCandidate*> & instList);

    void withGui();
    void autoQuit(bool st);
    void onlyShowInstall(bool si);

    void newStatus(QString msg, int mode);

    bool runAfter(QString path, QStringList arguments, QString workingDir);

    void quit(bool force=false);


signals:
    void error();
    void progress(int pr);
    void statusChanged(QString msg, int mode);

public slots:
    void onComplete(bool newInstalled);
    void onError();

private slots:
    void onUpdtCnfDownloaded(QTemporaryFile * cnfFile);
    void onUpdateCndDownloadError(QString err);
    void onProgress(int pr);

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
    bool _autoQuit;
    bool _onlyShowInstall;

};

#endif // APPCORE_H
