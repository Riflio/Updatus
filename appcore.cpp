#include "appcore.h"
#include <QDebug>


AppCore::AppCore(QObject *parent) : QObject(parent)
{    
    _mainCnf = new QSettings("./updateManager.cnf", QSettings::IniFormat, this);
    _collectUpdtCnfManager = new DownloadManager(this);

    connect(_collectUpdtCnfManager, &DownloadManager::answerReady, this, &AppCore::onUpdtCnfDownloaded);

    collectInstalledPackadges();

    collectAvaliableUpdates();

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
        int version = _mainCnf->value(QString("installed/%1").arg(pName)).toString().replace(",","").toInt();
        QString fullName = QString("%1:%2").arg(pName).arg(version);
        QString path = _mainCnf->value(QString("%1/path").arg(fullName)).toString();
        qInfo()<<"Packet installed"<<fullName;
        _instPacks.insert(fullName, new Packadge(pName, path, version));
    }

    qInfo()<<"OK";
    return 1;
}

/**
* @brief Подключаемся к серверам, проверяем обновления для установленных пакетов
* @return
*/
int AppCore::collectAvaliableUpdates()
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

    QString updtServer = cnfUpdates.value("servers/main").toString();

    //-- Заносим для каждого пакета возможные новые версии и их зависимости
    foreach(Packadge * pack, _instPacks) {
        //qDebug()<<"Ins"<<pack->path();

        QString avVersionsStr = cnfUpdates.value(QString("installed/%1").arg(pack->name())).toString();

        if ( !avVersionsStr.isEmpty() ) {
            QStringList avVersions = avVersionsStr.split(";");
            if ( avVersions.count()==0 ) {
                qWarning()<<"Wrong avaliable versions format"<<avVersionsStr;
                continue;
            }

            foreach(QString avVersion, avVersions) {

                int ver = QString(avVersion).replace(".", "").toInt();
                if ( ver<=pack->version() ) continue;

                QString fullPath = QString("%1:%2").arg(pack->name()).arg(avVersion);
                QString path = cnfUpdates.value(QString("%1/path").arg(fullPath)).toString();

                PackadgeCandidate * candidate = new PackadgeCandidate(pack->name(), updtServer+path, ver);

                QString relsStr = cnfUpdates.value(QString("%1/rels").arg(fullPath)).toString();
                if ( !relsStr.isEmpty() ) {
                    QStringList rels = relsStr.split(";");
                    foreach(QString rel, rels) {

                        QStringList relNameVersion = rel.split(":");
                        if ( relNameVersion.count()!=2 ) {
                            qWarning()<<"Wrong format rels"<<relNameVersion;
                            continue;
                        }
                        candidate->addRel(relNameVersion.at(0),  relNameVersion.at(1));
                    }
                }


                pack->addCandidate(candidate);
            }
        }
    }

    cnfFile->close();


    qInfo()<<"All cnf files downloaded";

    solveRelatives();
}

/**
* @brief Решаем зависимости и что будем качать
* @return
*/
int AppCore::solveRelatives()
{
    qInfo()<<"Solve relatives";

    //-- Составляем по каждому установленному пакету и его версии условия зависимостей
    //-- Полному имени должен соответстовать индекс для SAT solver

    foreach(Packadge * pack, _instPacks) {

        Packadge::TCandidates cnds =  pack->candidates();

        foreach(PackadgeCandidate * cnd, cnds) {
            qDebug()<<"-->"<<cnd->name()<<cnd->version();
            PackadgeCandidate::TRels rels = cnd->relatives();
            QStringList relsPacketNames = rels.keys().toSet().toList(); //-- Удаляем дубликаты

            foreach(QString rName, relsPacketNames) {
                QStringList rVersions = rels.values(rName);
                qDebug()<<"rel"<<rName<<rVersions;
            }

        }

    }

    return 1;
}












