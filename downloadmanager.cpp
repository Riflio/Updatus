#include "downloadmanager.h"
#include "defines.h"

#include <QNetworkRequest>
#include <QFile>
#include <QSslError>
#include <QDebug>

DownloadManager::DownloadManager(QObject *parent) : QObject(parent), _curAttempt(0)
{
    _naManager = new QNetworkAccessManager(this);
    connect(_naManager, &QNetworkAccessManager::finished, this, &DownloadManager::onNetworkAnswer);


    _request.setRawHeader("User-Agent", QString("Updatys %1").arg(VERSION).toUtf8());
}

/**
* @brief Делаем запрос
* @param url
* @param attempts
* @return
*/
int DownloadManager::request(QUrl url, int attempts)
{
    qInfo()<<"Send request"<<url.toString();
    _attempts = attempts;
    _curAttempt = 0;

    _request.setUrl(url);
    if ( !sendRequest() ) { return 0; }

    return  1;
}

/**
* @brief Отправляем запрос
* @param url
*/
bool DownloadManager::sendRequest()
{
    if ( _curAttempt>=_attempts ) { return false; }
    _curAttempt++;

    QNetworkReply *reply = _naManager->get(_request);

    connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error), this, &DownloadManager::onNetworkError);
    //connect(reply, &QNetworkReply::sslErrors, this, &DownloadManager::onNetworkErrorSsl);
    connect(reply, &QNetworkReply::downloadProgress, this, &DownloadManager::onProgressChanged);

    return true;
}

bool DownloadManager::tryRequestAgain()
{
    if ( sendRequest() ) { //-- Попробуем ещё разик
        qWarning()<<"Previus attempt download"<<_request.url().toString()<<"has error. Let's try again, number"<<_curAttempt<<"of"<<_attempts;
        return true;
    }

    return false;
}

/**
* @brief Получили ответ
* @param reply
*/
void DownloadManager::onNetworkAnswer(QNetworkReply *reply)
{
    QTemporaryFile * tmpFile = new QTemporaryFile(this);

    if( !tmpFile->open() ){
        emit error("File not opened!");
        return;
    }

    tmpFile->write(reply->readAll());
    tmpFile->flush();

    emit answerReady(tmpFile);
}

/**
* @brief Ошибка сети
* @param err
*/
void DownloadManager::onNetworkError(QNetworkReply::NetworkError err)
{
    if ( tryRequestAgain() ) { return; }
    emit error(QString("Network error: %1").arg(err));
}

/**
* @brief Ошибка сети по ssl
* @param errors
*/
void DownloadManager::onNetworkErrorSsl(const QList<QSslError> &errors)
{
    if ( tryRequestAgain() ) { return; }
    emit error(QString("Network ssl error"));
}

void DownloadManager::onProgressChanged(int bytesReceived, int bytesTotal)
{
    double pr = static_cast<double>(bytesReceived)*100.0/bytesTotal;
    int prR = qRound(pr);
    if ( prR<pr ) prR++;
    emit progress(prR, bytesReceived);
}

