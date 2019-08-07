#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QDateTime>
#include <QFile>
#include <QUrl>

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
    void msg(QString dateTime, QString type, QString category, QString functionName, QString filePath, QString msg);

    static void qDebugWrapperMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString &msg);

    bool sendToServer(QUrl url, const QMap<QString, QVariant> &variables);

    void setQDebugWrapper();

signals:

public slots:

private:
    QFile * _logFile;

};

#endif // LOGGER_H
