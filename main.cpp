#include <QApplication>
#include <QCommandLineParser>

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
        QStringList()<<"g"<<"gui",
        QObject::tr("main", "Show GUI")
    );
    parser.addOption(guiOpt);

    QCommandLineOption autoQuitOpt(
        QStringList()<<"q"<<"quit",
        QObject::tr("main", "Quit after complete")
    );
    parser.addOption(autoQuitOpt);


    parser.process(app);

    Logger::instance().setQDebugWrapper();

    if ( parser.isSet(showLogOpt) ) {
        Logger::instance().showLog = true;
    }

    AppCore * core = new AppCore(nullptr);

    if ( parser.isSet(guiOpt) ) {
        core->withGui();
    }

    if ( parser.isSet(autoQuitOpt) ) {
        core->autoQuit(true);
    }

    core->upgrade(parser.value(confFileOpt));

    return app.exec();
}
