#include <QApplication>
#include <QCommandLineParser>
#include <QDir>
#include <QObject>

#include "appcore.h"
#include "defines.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName("Updatus");
    QApplication::setApplicationVersion(VERSION);

    QCommandLineParser parser;
    parser.setApplicationDescription("UpdateManager");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption confFileOpt(
        QStringList() << "c" << "conffile",
        QObject::tr("main", "Set configuration file to <configfile.cnf>."),
        QObject::tr("main", "configfile"),
        QString("./Updatus.cnf")
    );
    parser.addOption(confFileOpt);

    QCommandLineOption showLogOpt(
        QStringList()<<"l"<<"log",
        QObject::tr("main", "Show log in console")
    );
    parser.addOption(showLogOpt);

    QCommandLineOption guiOpt(
        QStringList()<<"g"<<"nogui",
        QObject::tr("main", "Not show GUI")
    );
    parser.addOption(guiOpt);

    QCommandLineOption autoQuitOpt(
        QStringList()<<"q"<<"noquit",
        QObject::tr("main", "No quit after complete")
    );
    parser.addOption(autoQuitOpt);

    QCommandLineOption showInstallOpt(
        QStringList()<<"s"<<"showinstall",
        QObject::tr("main", "Only show was packets will be installed.")
    );
    parser.addOption(showInstallOpt);

    parser.process(app);


    Logger::instance();
    Logger::instance().setQDebugWrapper();

    if ( parser.isSet(showLogOpt) ) {
        Logger::instance().showLog = true;
    }

    AppCore * core = new AppCore(nullptr);

    if ( !parser.isSet(guiOpt) ) {
        core->withGui();
    }

    if ( !parser.isSet(autoQuitOpt) ) {
        core->autoQuit(true);
    }

    if ( parser.isSet(showInstallOpt) ) {
        core->onlyShowInstall(true);
    }

    QString confFilePath = ( parser.isSet(confFileOpt) )? parser.value(confFileOpt) : QApplication::applicationDirPath()+QDir::separator()+parser.value(confFileOpt);

    bool upgrdSt = core->upgrade(confFilePath);

    if ( !upgrdSt && parser.isSet(guiOpt) ) { //-- Если не удалось и без GUI - сразу выходим
        return -1;
    }

    return app.exec();
}
