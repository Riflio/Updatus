#include "downloadmanager.h"

#include <QNetworkRequest>
#include <QFile>

DownloadManager::DownloadManager(QObject *parent) : QObject(parent)
{
    _naManager = new QNetworkAccessManager(this);
    connect(_naManager, &QNetworkAccessManager::finished, this, &DownloadManager::onNetworkAnswer);
}

int DownloadManager::request(QUrl url)
{
    qDebug()<<"Send request"<<url.toString();
    QNetworkRequest request;
    request.setUrl(url);
    _naManager->get(request);
    return  1;
}

/**
* @brief Получили ответ
* @param reply
*/
void DownloadManager::onNetworkAnswer(QNetworkReply *reply)
{
    if(reply->error()){
        emit error(reply->errorString());
        return;
    }

    QTemporaryFile * tmpFile = new QTemporaryFile(this);

    if( !tmpFile->open() ){
        emit error("File not opened!");
        return;
    }

    tmpFile->write(reply->readAll());
    tmpFile->flush();

    emit answerReady(tmpFile);
}
