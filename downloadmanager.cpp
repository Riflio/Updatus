#include "downloadmanager.h"

#include "defines.h"
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
    request.setRawHeader("User-Agent", QString("Updatys %1").arg(VERSION).toUtf8());
    QNetworkReply *reply = _naManager->get(request);

    connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error), this, &DownloadManager::onNetworkError);
    connect(reply, &QNetworkReply::sslErrors, this, &DownloadManager::onNetworkErrorSsl);

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

void DownloadManager::onNetworkError(QNetworkReply::NetworkError err)
{
    emit error(QString("Network error: %1").arg(err));
}

void DownloadManager::onNetworkErrorSsl(const QList<QSslError> &errors)
{
    emit error(QString("Network ssl error"));
}
