#include "packadgecandidateupdater.h"

#include <QDir>

PackadgeCandidateUpdater::PackadgeCandidateUpdater(const PackadgeCandidate &other, QString tempDir):
    PackadgeCandidate(other), _status(US_NONE), _tempDir(tempDir)
{
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

void PackadgeCandidateUpdater::download(int downloadAttempts)
{
    _dwnProgress = 0;
    addStatus(US_REQUESTED);
    _dlMr->request(QUrl(downloadUrl()), downloadAttempts);
}

int PackadgeCandidateUpdater::downloadProgress() const
{
    return  _dwnProgress;
}

void PackadgeCandidateUpdater::onDownloadComplete(QTemporaryFile * packetFile)
{
    qInfo()<<"Candidate downloaded"<<fullName();

    //TODO: Сверять контрольные суммы первым делом

    QDir tempDir(_tempDir);
    tempDir.cd(pathDir());

    if ( !tempDir.exists() ) {
        tempDir.mkpath(tempDir.absolutePath());
    }

    if ( !tempDir.exists() ) {
        qWarning()<<"Can not create cache path"<<tempDir.absolutePath();
        emit error();
        return;
    }

    QString cfn = tempDir.filePath(pathFileName());

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
