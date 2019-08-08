#include "logger.h"
#include <QTextStream>
#include <QNetworkRequest>
#include <QUrlQuery>
#include <QHttpPart>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFileInfo>
#include <QApplication>
#include <iostream>

Logger::Logger(QObject *parent) : QObject(parent), showLog(false), _manager(nullptr)
{
    _logFile = new QFile(this);
}

Logger &Logger::instance()
{
    static Logger * _instance = nullptr;
    if ( _instance==nullptr ) {
        _instance = new Logger(nullptr);
    }
    return *_instance;
}

void Logger::msg(QString dateTime, QString type, QString category, QString functionName, QString filePath, QString msg)
{
    if ( !_logFile->isOpen() ) {
        _logFile->setFileName(QString("%1/log-%2.log").arg(".").arg(QDateTime::currentDateTime().toString("dd.MM.yy_h:mm:ss:zzz")));
        _logFile->open(QIODevice::WriteOnly | QIODevice::Append);
    }

    QTextStream ts(_logFile);
    ts<<dateTime<<" ["<<type<<"-"<<""<<category<<"] "<<"<<<< "<<msg<<" >>>>> "<<"{"<<functionName<<"} "<<"==="<<filePath<<endl;

    if ( showLog ) std::cout<<msg.toUtf8().data()<<std::endl;

    emit newMsg(msg);
}

/**
* @brief Перенаправляем стандартный вывод в лог
* @param type
* @param context
* @param msg
*/
void Logger::qDebugWrapperMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString sType="";
    switch (type) {
        case QtInfoMsg: sType = "Info"; break;
        case QtDebugMsg: sType = "Debug"; break;
        case QtWarningMsg: sType = "Warning"; break;
        case QtCriticalMsg: sType = "CRITICAL"; break;
        case QtFatalMsg: sType = "FATAL"; break;
    }

    Logger::instance().msg(
        QDateTime::currentDateTime().toString("yyyyMMdd h:mm:ss.zzz"),
        sType,
        context.category,
        QString("%1:%2").arg(context.function).arg(context.line),
        context.file,
        msg
    );
}

/**
* @brief Отправляем на сервер
* @param url
* @param variables - addition variables, who with set as POST parameters
* @return
*/
bool Logger::sendToServer(QUrl url, const QMap<QString, QVariant> &variables)
{
    if ( !url.isValid() ) return false;
    if ( !_manager ) {
        _manager = new QNetworkAccessManager(this);
        connect(_manager, &QNetworkAccessManager::finished, this, &Logger::onNetworkAnswer);
    }
    QHttpMultiPart * mp = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart pHandler;
    pHandler.setHeader(QNetworkRequest::ContentDispositionHeader, QString("form-data; name=\"HandlerType\""));
    pHandler.setBody("UpdatusLog");
    mp->append(pHandler);

    for(QMap<QString, QVariant>::const_iterator i=variables.constBegin(); i!=variables.constEnd(); ++i) {
        QHttpPart pReqParam;
        pReqParam.setHeader(QNetworkRequest::ContentDispositionHeader, QString("form-data; name=\"%1\"").arg(i.key()));
        pReqParam.setBody(i.value().toString().toUtf8());
        mp->append(pReqParam);
    }

    QString logDumpPath = _logFile->fileName();
    _logFile->close();
    QFileInfo logDumpFileInfo(logDumpPath);

    if ( logDumpPath.isEmpty() ) { return false; }

    QFile * logFile = new QFile(logDumpPath, mp);
    if ( !logFile->exists() || !logFile->open(QIODevice::ReadOnly) ) { return false; }

    QHttpPart pLogDump;
    pLogDump.setHeader(QNetworkRequest::ContentTypeHeader, QString("text/plain"));
    pLogDump.setHeader(QNetworkRequest::ContentDispositionHeader, QString("form-data; name=\"logDump\"; filename=\"%1\"").arg(logDumpFileInfo.fileName()));
    pLogDump.setBodyDevice(logFile);
    mp->append(pLogDump);

    QNetworkRequest request;
    request.setUrl(url);

    _manager->post(request, mp);

    return true;
}

/**
* @brief Устанавливаем перехватчик всех вызовов qDebug(), qInfo(), qWarning()
*/
void Logger::setQDebugWrapper()
{
    qInstallMessageHandler(Logger::qDebugWrapperMessageHandler);
}

void Logger::onNetworkAnswer(QNetworkReply *reply)
{

}
