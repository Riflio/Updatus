#include <QApplication>
#include <QCommandLineParser>
#include <QDir>

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
        QObject::tr("main", "Show log")
    );
    parser.addOption(showLogOpt);

    QCommandLineOption guiOpt(
        QStringList()<<"g"<<"nogui",
        QObject::tr("main", "Not show GUI")
    );
    parser.addOption(guiOpt);

    QCommandLineOption autoQuitOpt(
        QStringList()<<"q"<<"quit",
        QObject::tr("main", "Quit after complete")
    );
    parser.addOption(autoQuitOpt);

    parser.process(app);

    if ( parser.isSet(showLogOpt) && !parser.value(showLogOpt).isEmpty() ) {
        Logger::instance().setLogDir(parser.value(showLogOpt));
    } else {
        Logger::instance().setLogDir(QApplication::applicationDirPath());
    }


    Logger::instance().setQDebugWrapper();

    if ( parser.isSet(showLogOpt) ) {
        Logger::instance().showLog = true;
    }

    AppCore * core = new AppCore(nullptr);

    if ( !parser.isSet(guiOpt) ) {
        core->withGui();
    }

    if ( parser.isSet(autoQuitOpt) ) {
        core->autoQuit(true);
    }


    QString confFilePath = ( parser.isSet(confFileOpt) )? parser.value(confFileOpt) : QApplication::applicationDirPath()+QDir::separator()+parser.value(confFileOpt);

    if ( !core->upgrade(confFilePath) ) {
        return -1;
    }

    return app.exec();
}
