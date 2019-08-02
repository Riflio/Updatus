#include "updater.h"
#include <QDir>
#include "qzipreader_p.h"

#include <QDebug>

PackadgeCandidateUpdater::PackadgeCandidateUpdater(const PackadgeCandidate &other, QString tempDir):
    PackadgeCandidate(other), _status(US_NONE), _tempDir(tempDir)
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

QString PackadgeCandidateUpdater::cachePacketPath() const
{
    return _cachePacketPath;
}

void PackadgeCandidateUpdater::download()
{
    _dlMr->request(QUrl(downloadUrl()));
    addStatus(US_REQUESTED);
}

void PackadgeCandidateUpdater::onDownloadComplete(QTemporaryFile * packetFile)
{
    qInfo()<<"Candidate downloaded"<<fullName();

    //TODO: Сверять контрольные суммы первым делом

    QString cachePath = _tempDir+pathDir();
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

        PackadgeCandidateUpdater * pcu = new PackadgeCandidateUpdater(*cnd, _mainCnf->value("tempDir").toString());
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
void updater::onPacketDownloaded(PackadgeCandidateUpdater *pack)
{
    Q_UNUSED(pack);

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
    removeOldInstallNew();
}

/**
* @brief Удаляем предыдущие установленные пакеты, к которым скачали обновления
*/
void updater::removeOldInstallNew()
{
    qInfo()<<"Begin remove old packs and install new";

    QString defaultInstPath = _mainCnf->value("defaultInstPath").toString();

    //TODO: Удаляем пакеты, которые были установлены как зависимости и больше не требуются

    foreach(PackadgeCandidateUpdater * up, _updaterPackages) {

        Packadge * orPack = up->originalPackage();
        if ( orPack!=nullptr ) {
            QString instPath = _mainCnf->value(QString("%1/cachePath").arg(orPack->fullName())).toString();
            if ( !instPath.isEmpty() ) { //-- Удаляем файлы прежних версий
            }

            //-- Удаляем запись о пакете
            _mainCnf->beginGroup(orPack->fullName());
            _mainCnf->remove("");
            _mainCnf->endGroup();
            _mainCnf->remove(QString("installed/%1").arg(orPack->name()));
        }

        //-- Ставим новые
        QZipReader zip(up->cachePacketPath(), QIODevice::ReadOnly);
        if( !zip.exists() ) {
            qWarning()<<"Package not exist O_o";
            return;
        }

        QList<QZipReader::FileInfo> allFiles = zip.fileInfoList();

        QDir instDir = (orPack!=nullptr && !orPack->path().isEmpty())? orPack->path() : QDir(defaultInstPath);

        //-- Создаём пути для файлов и устанавливаем права
        foreach (QZipReader::FileInfo fi, allFiles) {
            if ( !fi.isDir ) continue;
            QString absPath = instDir.path()+QDir::separator()+fi.filePath;
            if ( !instDir.mkpath(fi.filePath) ) { emit error(); return; }
            if ( !QFile::setPermissions(absPath, fi.permissions) ) { emit error(); return; }
        }

        //-- По путям закидываем файлы
        foreach (QZipReader::FileInfo fi, allFiles) {
            if ( !fi.isFile) continue;

            QString absPath = instDir.path()+QDir::separator()+fi.filePath;
            qInfo()<<"Unpack new file"<<absPath;

            QFile file(absPath);
            if( !file.open(QFile::WriteOnly) ) { emit error(); return; }

            file.write(zip.fileData(fi.filePath), zip.fileData(fi.filePath).size());
            file.setPermissions(fi.permissions);
            file.close();
        }

        zip.close();

        //-- Записываем инфу
        _mainCnf->setValue(QString("%1/cachePath").arg(up->fullName()), up->cachePacketPath());
        _mainCnf->setValue(QString("%1/instPath").arg(up->fullName()), instDir.path());
        _mainCnf->setValue(QString("%1/instType").arg(up->fullName()), ((orPack!=nullptr)? orPack->instType() : "asRel") );
        _mainCnf->setValue(QString("installed/%1").arg(up->name()), up->version());
        _mainCnf->sync();
    }
}







