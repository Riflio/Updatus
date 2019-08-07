#include "appcore.h"
#include "defines.h"
#include <QDebug>
#include <QCoreApplication>

AppCore::AppCore(QObject *parent) : QObject(parent)
{
    Logger::instance();
    qInfo()<<"Welcome to Updatus - update manager for our programs."<<VERSION;
}

AppCore::~AppCore()
{
    qDeleteAll(_instPacks);
    delete _packageSatSolver;
}

bool AppCore::upgrade(QString mainCnfPath)
{
    qInfo()<<"Start upgrade from"<<mainCnfPath;

    if ( !QFile::exists(mainCnfPath) ) {
        qWarning()<<"Configuration file not exists!";
        return false;
    }

    _mainCnf = new QSettings(mainCnfPath, QSettings::IniFormat, this);

    _collectUpdtCnfManager = new DownloadManager(this);
    _updater = new Updater(this, _mainCnf);

    connect(_collectUpdtCnfManager, &DownloadManager::answerReady, this, &AppCore::onUpdtCnfDownloaded);
    connect(_updater, &Updater::completed, this, &AppCore::onComplete);
    connect(_updater, &Updater::error, this, &AppCore::onError);


    _mainCnf->beginReadArray("info");
        QStringList infoKeys = _mainCnf->allKeys();
        foreach(QString infoKey, infoKeys) {
            _infoVariables[infoKey] = _mainCnf->value(infoKey);
        }
    _mainCnf->endArray();

    int collectInstRes = collectInstalledPackadges();

    if ( collectInstRes<0 ) {
        qWarning()<<"Unable collect install packages."<<collectInstRes;
        return false;
    }

    int collectAvRes = collectAvaliableUpdates();
    if ( collectAvRes<0 ) {
        qWarning()<<"Unable collect avaliable packages."<<collectAvRes;
        return false;
    }

    return true;
}

/**
* @brief Собираем инфу о текущих установленных файлах
*/
int AppCore::collectInstalledPackadges()
{
    qInfo()<<"Collect info about installed packadges...";

    _mainCnf->beginReadArray("installed");
        QStringList instPacksNames = _mainCnf->allKeys();
    _mainCnf->endArray();

    foreach(QString pName, instPacksNames) {
        QString version = _mainCnf->value(QString("installed/%1").arg(pName)).toString();

        Packadge * instPack = new Packadge(pName, version, *_mainCnf);

        qInfo()<<"Packet installed"<<instPack->fullName();
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
    qInfo()<<"Try collect updates cnf from server:"<<server;

    _collectUpdtCnfManager->request(QUrl(server+"repository.cnf"));

    return 1;
}

void AppCore::onComplete(bool newInstalled)
{
    //-- Записываем дату последнего обновления и время и отсылаем лог
    if ( newInstalled ) {
        _mainCnf->setValue("lastUpdated", QDateTime::currentDateTime().toString("dd.MM.yy hh:mm"));
    }

    _mainCnf->sync();
    qInfo()<<"Complete!";    
    _infoVariables["status"]="OK";
    _infoVariables["hasNewInstalled"]=(newInstalled)? "yes" : "no";

    Logger::instance().sendToServer(QUrl(_mainCnf->value("sendLogs").toString()), _infoVariables);
    QCoreApplication::quit();
}

void AppCore::onError()
{
    qWarning()<<"Errors occurred during the upgrade process. Execution aborted.";
    _infoVariables["status"]="has errors";
    Logger::instance().sendToServer(QUrl(_mainCnf->value("sendLogs").toString()), _infoVariables);
    QCoreApplication::quit();
}

/**
* @brief Загружен очередной файл информации о возможных обновлениях
* @param cnfFile
*/
void AppCore::onUpdtCnfDownloaded(QTemporaryFile *cnfFile)
{
    qInfo()<<"Downloaded update cnf file";

    qInfo()<<"Parse cnf file";

    QSettings cnfUpdates(cnfFile->fileName(), QSettings::IniFormat, this);

    //-- Заносим для каждого установленного пакета возможные новые версии и их зависимости
    foreach(Packadge * pack, _instPacks) {
        int res = pack->parseUpdates(cnfUpdates, &_instCandidates);
        if ( res<0 ) qWarning()<<"Unvaliable parse update for packet"<<pack->fullName();
    }

    cnfFile->close();

    qInfo()<<"All cnf files downloaded and parsed";

    QList<PackadgeCandidate*> toInstList;

    int prepInst = _packageSatSolver->prepareInstList(_instPacks, toInstList);

    if ( prepInst<0 ) {
        qWarning()<<"Unable build versions three";
        onError();
        return;
    }

    if ( toInstList.count()==0 ) {
        qInfo()<<"Has no updates...";
        onComplete(false);
        return;
    }

    _updater->goInstall(toInstList);
}


