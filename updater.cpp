#include "updater.h"
#include <QDebug>
#include <QDir>

PackadgeCandidateUpdater::PackadgeCandidateUpdater(const PackadgeCandidate &other):
    PackadgeCandidate(other), _status(US_NONE)
{
    _dlMr = new DownloadManager();
    connect(_dlMr, &DownloadManager::answerReady, this, &PackadgeCandidateUpdater::onDownloadComplete);
}

void PackadgeCandidateUpdater::addStatus(int status)
{
    _status |= status;
}

int PackadgeCandidateUpdater::status() const
{
    return _status;
}

void PackadgeCandidateUpdater::download()
{
    _dlMr->request(QUrl(downloadUrl()));
    addStatus(US_REQUESTED);
}

void PackadgeCandidateUpdater::onDownloadComplete(QTemporaryFile * packetFile)
{
    qInfo()<<"Candidate downloaded"<<fullName();
    addStatus(US_DOWNLOADED);
    emit packageDownloaded(this, packetFile);
}


//========================================================================================================

updater::updater(QObject *parent, QSettings * mainCnf)
    : QObject(parent), _mainCnf(mainCnf)
{
    //TODO: Проверять существование tempDir и доступность для записи/чтения
}

/**
* @brief Устанавливаем
* @param
* @return
*/
int updater::goInstall(const QList<PackadgeCandidate *> & instList)
{
    qInfo()<<"Go install";
    foreach(PackadgeCandidate * cnd, instList) {
        qInfo()<<"To install"<<cnd->fullName();

        PackadgeCandidateUpdater * pcu = new PackadgeCandidateUpdater(*cnd);
        _updaterPackages.insert(cnd->fullName(), pcu);

        connect(pcu, &PackadgeCandidateUpdater::packageDownloaded, this, &updater::onPacketDownloaded);
        pcu->download();
    }

    return 1;
}

/**
* @brief Как только пакет скачался переносим его в папку кэша
* @param pack
* @param packetFile
*/
void updater::onPacketDownloaded(PackadgeCandidateUpdater *pack, QTemporaryFile *packetFile)
{
    //TODO: Сверять контрольные суммы первым делом

    QString cachePath = _mainCnf->value("tempDir").toString()+pack->pathDir();
    QDir dir;

    if ( !dir.exists(cachePath) ) {
        dir.mkpath(cachePath);
    }

    if ( !dir.exists(cachePath) ) {
        qWarning()<<"Can not create cache path"<<cachePath;
        emit error();
        return;
    }

    QString cacheFileName = cachePath+pack->pathFileName();

    if ( !QFile::exists(cacheFileName)) {
        if ( !packetFile->copy(cacheFileName) ) {
            qWarning()<<"Can not move temp file to cache dir"<<cacheFileName;
            emit error();
            return;
        }
    }

    packetFile->close();

    foreach(PackadgeCandidateUpdater * up, _updaterPackages) {
        if ( !(up->status() & PackadgeCandidateUpdater::US_DOWNLOADED) ) return;
    }

    allPacketsDownloaded();
}

/**
* @brief Все пакеты загружены
*/
void updater::allPacketsDownloaded()
{
    qInfo()<<"All packages downloaded!";
    removePrevPacks();
}

/**
* @brief Удаляем предыдущие установленные пакеты, к которым скачали обновления
*/
void updater::removePrevPacks()
{
    qInfo()<<"Begin remove prev packs";
    foreach(PackadgeCandidateUpdater * up, _updaterPackages) {
        Packadge * orPack = up->originalPackage();
        qDebug()<<"OR PACH"<<orPack;
        //if ( orPack==nullptr ) continue;
        //qDebug()<<"REM"<<orPack->fullName();
    }
}







