#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QDateTime>
#include <QFile>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QDir>

/**
* @brief The Logger class
* Add DEFINES += QT_MESSAGELOGCONTEXT to your *.pro file
* and qInstallMessageHandler(Logger::qDebugWrapperMessageHandler); at main.cpp to enable write log
*/

class Logger : public QObject
{
    Q_OBJECT
public:
    explicit Logger(QObject *parent = nullptr);
    static Logger & instance();
    bool msg(QString dateTime, QString type, QString category, QString functionName, QString filePath, QString msg);

    static void qDebugWrapperMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString &msg);

    bool sendToServer(QUrl url, const QMap<QString, QVariant> &variables);

    void setQDebugWrapper();

    bool showLog;

    void setLogDir(QString logDir);

signals:
    void newMsg(QString msg);

public slots:

private slots:
    void onNetworkAnswer(QNetworkReply *reply);

private:
    QFile * _logFile;
    QNetworkAccessManager * _manager;
    QDir _logDir;

    static const QtMessageHandler QT_DEFAULT_MESSAGE_HANDLER;
};

#endif // LOGGER_H
