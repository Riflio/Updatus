#include "appcore.h"
#include <QDebug>


AppCore::AppCore(QObject *parent) : QObject(parent)
{    
    _mainCnf = new QSettings("./updateManager.cnf", QSettings::IniFormat, this);
    _collectUpdtCnfManager = new DownloadManager(this);
    _updater = new updater(this, _mainCnf);

    connect(_collectUpdtCnfManager, &DownloadManager::answerReady, this, &AppCore::onUpdtCnfDownloaded);

    collectInstalledPackadges();

    collectAvaliableUpdates();
}

AppCore::~AppCore()
{
    qDeleteAll(_instPacks);
    delete _packageSatSolver;
}

/**
* @brief Собираем инфу о текущих установленных файлах
*/
int AppCore::collectInstalledPackadges()
{
    qInfo()<<"Collect info about installed packadges...";

    //-- Узнаем, какие пакеты установлены и их версии
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
    _packageSatSolver->prepareInstList(_instPacks, toInstList);

    _updater->goInstall(toInstList);
}


