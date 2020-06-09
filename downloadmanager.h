#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QObject>

#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QTemporaryFile>

/**
* @brief Наша задача сделать запрос по указанному ЮРЛу и сохранить принятое во временный файл
*/
class DownloadManager : public QObject
{
    Q_OBJECT
public:
    explicit DownloadManager(QObject *parent = nullptr);
    int request(QUrl url, int attempts);
    bool tryRequestAgain();

signals:
    void answerReady(QTemporaryFile * tempFile);
    void error(QString msg);
    void progress(int pr, int bytes);

private slots:
    void onNetworkAnswer(QNetworkReply *reply);
    void onNetworkError(QNetworkReply::NetworkError err);
    void onNetworkErrorSsl(const QList<QSslError> &errors);
    void onProgressChanged(int bytesReceived, int bytesTotal);

private:
    QNetworkAccessManager * _naManager;
    int _attempts;
    int _curAttempt;

    bool sendRequest();    
    QNetworkRequest _request;
};

#endif // DOWNLOADMANAGER_H
