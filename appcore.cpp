#include "appcore.h"
#include "defines.h"
#include <QDebug>
#include <QApplication>
#include <QProcess>

AppCore::AppCore(QObject *parent)
    :QObject(parent), _mainWindow(nullptr), _hasError(false), _autoQuit(false)
{
    Logger::instance();
    _packageSatSolver = new PackageSatSolver();    
    qInfo()<<tr("Welcome to Updatus version %1 - update manager for our programs.").arg(VERSION);
}

AppCore::~AppCore()
{
    qDeleteAll(_instPacks);
    delete _packageSatSolver;
    if ( !!_mainWindow ) delete _mainWindow;
}

bool AppCore::upgrade(QString mainCnfPath)
{
    newStatus(tr("Start upgrade from %1.").arg(mainCnfPath), 1);

    if ( !QFile::exists(mainCnfPath) ) {
        newStatus(tr("Configuration file not exists!"), -1);
        return false;
    }

    _mainCnf = new QSettings(mainCnfPath, QSettings::IniFormat, this);

    _collectUpdtCnfManager = new DownloadManager(this);
    _updater = new Updater(this, _mainCnf);

    connect(_collectUpdtCnfManager, &DownloadManager::answerReady, this, &AppCore::onUpdtCnfDownloaded);
    connect(_collectUpdtCnfManager, &DownloadManager::error, this, &AppCore::onUpdateCndDownloadError);
    connect(_updater, &Updater::completed, this, &AppCore::onComplete);
    connect(_updater, &Updater::error, this, &AppCore::onError);
    connect(_updater, &Updater::progress, this, &AppCore::onProgress);

    _mainCnf->beginReadArray("info");
        QStringList infoKeys = _mainCnf->allKeys();
        foreach(QString infoKey, infoKeys) {
            _infoVariables[infoKey] = _mainCnf->value(infoKey);
        }
    _mainCnf->endArray();

    int collectInstRes = collectInstalledPackadges();

    if ( collectInstRes<0 ) {
        newStatus(tr("Unable collect install packages. %1").arg(collectInstRes), -1);
        return false;
    }

    int collectAvRes = collectAvaliableUpdates();
    if ( collectAvRes<0 ) {
        newStatus(tr("Unable collect avaliable packages. %1").arg(collectAvRes), -1);
        return false;
    }

    return true;
}

/**
* @brief Собираем инфу о текущих установленных файлах
*/
int AppCore::collectInstalledPackadges()
{
    newStatus(tr("Collect info about installed packadges..."), 1);

    _mainCnf->beginReadArray("installed");
        QStringList instPacksNames = _mainCnf->allKeys();
    _mainCnf->endArray();

    foreach(QString pName, instPacksNames) {
        QString version = _mainCnf->value(QString("installed/%1").arg(pName)).toString();

        Packadge * instPack = new Packadge(pName, version, *_mainCnf);

        newStatus(tr("Packet installed %1").arg(instPack->fullName()), 1);
        _instPacks.insert(instPack->fullName(), instPack);
    }

    return 1;
}

/**
* @brief Подключаемся к серверам, проверяем обновления для установленных пакетов
* @return
*/
int AppCore::collectAvaliableUpdates() //TODO: Сделать возможность указывать несколько серверов
{    
    QString server = _mainCnf->value("servers/main").toString();    
    newStatus(tr("Try collect updates cnf from server %1").arg(server), 1);

    _collectUpdtCnfManager->request(QUrl(server+"repository.cnf"));

    return 1;
}

/**
* @brief Нужен интерфейс, отображаем
*/
void AppCore::withGui()
{
    _mainWindow = new MainWindow(nullptr, this);
    _mainWindow->show();
}

/**
* @brief Отмечаем, нужно ли сразу выходить после завершения
* @param st
*/
void AppCore::autoQuit(bool st)
{
    _autoQuit = st;
}

void AppCore::newStatus(QString msg, int mode)
{
    switch (mode) {
        case 1:     qInfo()<<msg; break;
        case -1:    qWarning()<<msg; break;
        default:    qDebug()<<msg; break;
    }

    emit statusChanged(msg, mode);
}

/**
* @brief Запускаем указанную программу после проверки обновлений
* @param path
*/
bool AppCore::runAfter(QString path)
{
    if ( path.isEmpty() ) return false;
    qInfo()<<"Run after"<<path;

    bool st = QProcess::startDetached(path);

    return st;
}

/**
* @brief Выходим
*/
void AppCore::quit()
{
    runAfter(_mainCnf->value("runAfter").toString());

    _mainCnf->sync();

    Logger::instance().sendToServer(QUrl(_mainCnf->value("sendLogs").toString()), _infoVariables);

    if ( _autoQuit || !_mainWindow ) QApplication::quit();
}

void AppCore::onComplete(bool newInstalled)
{
    newStatus(tr("Complete!"), 1);
    onProgress(100);

    //-- Записываем дату и время последнего обновления, когда что-то было установлено
    if ( newInstalled ) {
        _mainCnf->setValue("lastUpdated", QDateTime::currentDateTime().toString("dd.MM.yy hh:mm"));
    }

    _infoVariables["status"] = "OK";
    _infoVariables["hasNewInstalled"] = (newInstalled)? "yes" : "no";

    quit();
}

void AppCore::onError()
{
    if ( _hasError ) return; //-- Нам и одной уже достаточно
    _hasError = true;
    newStatus(tr("Errors occurred during the upgrade process. Execution aborted."), -1);
    _infoVariables["status"] = "has errors";

    quit();
}

/**
* @brief Загружен очередной файл информации о возможных обновлениях
* @param cnfFile
*/
void AppCore::onUpdtCnfDownloaded(QTemporaryFile *cnfFile)
{
    newStatus(tr("Downloaded update cnf file!"), 1);
    newStatus(tr("Parse cnf file"), 1);

    QSettings cnfUpdates(cnfFile->fileName(), QSettings::IniFormat, this);

    //-- Заносим для каждого установленного пакета возможные новые версии и их зависимости
    foreach(Packadge * pack, _instPacks) {
        int res = pack->parseUpdates(cnfUpdates, &_instCandidates);
        if ( res<0 ) newStatus(tr("Parse cnf file problem %1").arg(pack->fullName()), -1);
    }

    cnfFile->close();

    newStatus(tr("All cnf files downloaded and parsed"), 1);

    QList<PackadgeCandidate*> toInstList;

    int prepInst = _packageSatSolver->prepareInstList(_instPacks, toInstList);

    if ( prepInst<0 ) {        
        newStatus(tr("Unable build versions three"), -1);
        onError();
        return;
    }

    if ( toInstList.count()==0 ) {
        newStatus(tr("Has no updates..."), 1);
        onComplete(false);
        return;
    }

    newStatus(tr("Download packages..."), 1);
    _updater->goInstall(toInstList);
}

void AppCore::onUpdateCndDownloadError(QString err)
{
    newStatus(tr("Unable download update cnf %1.").arg(err), -1);
    onError();
}

void AppCore::onProgress(int pr)
{
    emit progress(pr);
}


