#include "updater.h"
#include <QDir>
#include "qzipreader_p.h"

#include <QDebug>

PackadgeCandidateUpdater::PackadgeCandidateUpdater(const PackadgeCandidate &other, QString tempDir):
    PackadgeCandidate(other), _status(US_NONE), _tempDir(tempDir)
{
    if ( _tempDir.right(1)=="/" ) _tempDir = _tempDir.left(_tempDir.length()-1);

    _dlMr = new DownloadManager();
    connect(_dlMr, &DownloadManager::answerReady, this, &PackadgeCandidateUpdater::onDownloadComplete);
    connect(_dlMr, &DownloadManager::error, this, &PackadgeCandidateUpdater::onDownloadError);    
    connect(_dlMr, &DownloadManager::progress, this, &PackadgeCandidateUpdater::onDownloadProgress);

}

void PackadgeCandidateUpdater::addStatus(int status)
{
    _status |= status;
}

int PackadgeCandidateUpdater::status() const
{
    return _status;
}

QString PackadgeCandidateUpdater::cachePacketPath() const
{
    return _cachePacketPath;
}

void PackadgeCandidateUpdater::download()
{
    _dwnProgress = 0;
    _dlMr->request(QUrl(downloadUrl()));
    addStatus(US_REQUESTED);
}

int PackadgeCandidateUpdater::downloadProgress() const
{
    return  _dwnProgress;
}

void PackadgeCandidateUpdater::onDownloadComplete(QTemporaryFile * packetFile)
{
    qInfo()<<"Candidate downloaded"<<fullName();

    //TODO: Сверять контрольные суммы первым делом

    QString cachePath = _tempDir+QDir::separator()+pathDir();
    QDir dir;

    if ( !dir.exists(cachePath) ) {
        dir.mkpath(cachePath);
    }

    if ( !dir.exists(cachePath) ) {
        qWarning()<<"Can not create cache path"<<cachePath;
        emit error();
        return;
    }

    QString cfn = cachePath+pathFileName();

    if ( !QFile::exists(cfn)) {
        if ( !packetFile->copy(cfn) ) {
            qWarning()<<"Can not move temp file to cache dir"<<cfn;
            emit error();
            return;
        }
    }

    _cachePacketPath = cfn;
    packetFile->close();
    addStatus(US_DOWNLOADED);

    emit packageDownloaded(this);
}

void PackadgeCandidateUpdater::onDownloadError(QString err)
{
    qWarning()<<"Error downloading package"<<fullName()<<":"<<err;
    emit error();
}

void PackadgeCandidateUpdater::onDownloadProgress(int pr, int bytes)
{
    Q_UNUSED(pr);
    _dwnProgress = bytes;
}


//========================================================================================================

Updater::Updater(QObject *parent, QSettings * mainCnf)
    : QObject(parent), _mainCnf(mainCnf), _hasError(false)
{
    //TODO: Проверять существование tempDir и доступность для записи/чтения

    _recalcDwnPrTr.setInterval(1000);
    connect(&_recalcDwnPrTr, &QTimer::timeout, this, &Updater::recalcDownloadProgress);
}

/**
* @brief Устанавливаем
* @param
* @return
*/
int Updater::goInstall(const QList<PackadgeCandidate *> & instList)
{
    qInfo()<<"Go install";

    _updaterPackages.clear();
    _totalProgress = 0;

    foreach(PackadgeCandidate * cnd, instList) {

        if ( _hasError ) return  -1;

        qInfo()<<"To install"<<cnd->fullName();

        PackadgeCandidateUpdater * pcu = new PackadgeCandidateUpdater(*cnd, _mainCnf->value("tempDir").toString());
        _updaterPackages.insert(cnd->fullName(), pcu);

        connect(pcu, &PackadgeCandidateUpdater::packageDownloaded, this, &Updater::onPacketDownloaded);
        connect(pcu, &PackadgeCandidateUpdater::error, this, &Updater::onPacketDownloadError);        

        _totalProgress += pcu->fileSize();
    }

    _recalcDwnPrTr.start();

    foreach(PackadgeCandidateUpdater * pcu, _updaterPackages) {
        pcu->download();
    }

    return 1;
}

void Updater::goComplete(bool newInstalled)
{
    emit completed(newInstalled);
}

void Updater::goError()
{
    _recalcDwnPrTr.stop();
    _hasError = true;
    emit error();
}

/**
* @brief Как только пакет скачался переносим его в папку кэша
* @param pack
* @param packetFile
*/
void Updater::onPacketDownloaded(PackadgeCandidateUpdater *pack)
{
    Q_UNUSED(pack);

    if ( _hasError ) return;

    foreach(PackadgeCandidateUpdater * up, _updaterPackages) {
        if ( !(up->status() & PackadgeCandidateUpdater::US_DOWNLOADED) ) return;
    }

    allPacketsDownloaded();
}

void Updater::onPacketDownloadError()
{
    emit error();
}

/**
* @brief Пересчитаем прогресс скачивания
*/
void Updater::recalcDownloadProgress()
{
    long current = 0;
    foreach(PackadgeCandidateUpdater * pcu, _updaterPackages) {
        current += pcu->downloadProgress();
    }


    double pr = static_cast<double>(current)*100.0/_totalProgress;
    int prR = qRound(pr);
    if ( prR<pr ) prR++;

    emit progress(prR);
}

/**
* @brief Все пакеты загружены
*/
void Updater::allPacketsDownloaded()
{
    qInfo()<<"All packages downloaded!";
    _recalcDwnPrTr.stop();
    emit progress(100);
    removeOldInstallNew();
}

/**
* @brief Удаляем предыдущие установленные пакеты, к которым скачали обновления
*/
void Updater::removeOldInstallNew()
{
    qInfo()<<"Begin remove old packs and install new";

    QString defaultInstPath = _mainCnf->value("defaultInstPath").toString();

    //TODO: Удаляем пакеты, которые были установлены как зависимости и больше не требуются

    foreach(PackadgeCandidateUpdater * up, _updaterPackages) {

        Packadge * orPack = up->originalPackage();
        QDir instDir = (orPack!=nullptr && !orPack->path().isEmpty())? orPack->path() : QDir(defaultInstPath);

        if ( orPack!=nullptr ) {
            qInfo()<<"Remove old version"<<orPack->fullName();
            QString cachePath = _mainCnf->value(QString("%1/cachePath").arg(orPack->fullName())).toString();
            if ( !cachePath.isEmpty() ) { //-- Удаляем файлы прежних версий

                QZipReader zipRem(cachePath, QIODevice::ReadOnly);
                if ( zipRem.exists() ) {
                    QList<QZipReader::FileInfo> oldFiles = zipRem.fileInfoList();
                    foreach (QZipReader::FileInfo fi, oldFiles) {
                        if ( fi.isDir ) instDir.remove(fi.filePath);
                        if ( fi.isFile ) QFile::remove(instDir.path()+QDir::separator()+fi.filePath);
                    }
                    zipRem.close();
                }

            }

            //-- Удаляем запись о пакете
            _mainCnf->beginGroup(orPack->fullName());
            _mainCnf->remove("");
            _mainCnf->endGroup();
            _mainCnf->remove(QString("installed/%1").arg(orPack->name()));
        } else {
            qInfo()<<"Has no old version"<<up->fullName();
        }


        //-- Ставим новые
        qInfo()<<"Unpacking"<<up->fullName();

        QZipReader zip(up->cachePacketPath(), QIODevice::ReadOnly);
        if( !zip.exists() ) {
            qWarning()<<"Package not exist O_o";
            return;
        }

        QList<QZipReader::FileInfo> allFiles = zip.fileInfoList();

        //-- Создаём пути для файлов и устанавливаем права
        foreach (QZipReader::FileInfo fi, allFiles) {
            if ( !fi.isDir ) continue;
            QString absPath = instDir.path()+QDir::separator()+fi.filePath;
            if ( !instDir.mkpath(fi.filePath) ) { emit error(); return; }
            //if ( !QFile::setPermissions(absPath, fi.permissions) ) { emit error(); return; }
        }

        //-- По путям закидываем файлы
        foreach (QZipReader::FileInfo fi, allFiles) {
            if ( !fi.isFile) continue;

            QString absPath = instDir.path()+QDir::separator()+fi.filePath;
            qInfo()<<"Unpack file"<<absPath;

            QFile file(absPath);
            if( !file.open(QFile::WriteOnly) ) { emit error(); return; }

            file.write(zip.fileData(fi.filePath), zip.fileData(fi.filePath).size());
            //file.setPermissions(fi.permissions);
            file.close();
        }

        zip.close();

        //-- Записываем инфу
        _mainCnf->setValue(QString("%1/cachePath").arg(up->fullName()), up->cachePacketPath());
        _mainCnf->setValue(QString("%1/instPath").arg(up->fullName()), instDir.path());
        _mainCnf->setValue(QString("%1/instType").arg(up->fullName()), PackadgeInfo::instTypeStr( ( (orPack!=nullptr)? orPack->instType() : PackadgeInfo::asRels)));
        _mainCnf->setValue(QString("%1/rels").arg(up->fullName()), up->relativesStr());
        _mainCnf->setValue(QString("installed/%1").arg(up->name()), up->version());
        _mainCnf->sync();
    }

    goComplete(true);
}
