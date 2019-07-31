#include "updater.h"

updater::updater(QObject *parent) : QObject(parent)
{
    _dlMr = new DownloadManager();
    connect(_dlMr, &DownloadManager::answerReady, this, &updater::onDownloadComplete);
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

        _dlMr->request(QUrl(cnd->downloadUrl()));
    }

    return 1;
}



void updater::onDownloadComplete(QTemporaryFile *cnfFile)
{
    qInfo()<<"Candidate downloaded";
}

