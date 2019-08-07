#include <QCoreApplication>
#include "appcore.h"
#include "defines.h"
#include <QCommandLineParser>


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationName("Updatus");
    QCoreApplication::setApplicationVersion(VERSION);

    QCommandLineParser parser;
    parser.setApplicationDescription("UpdateManager");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption confFileOpt(
        QStringList() << "c" << "conffile",
        QObject::tr("main", "Set configuration file to <configfile.cnf>."),
        QObject::tr("main", "configfile"),
        QString("./updateManager.cnf")
    );
    parser.addOption(confFileOpt);

    QCommandLineOption showLogOpt(
        QStringList()<<"l"<<"log",
        QObject::tr("main", "Show log")
    );
    parser.addOption(showLogOpt);


    parser.process(a);

    if ( !parser.isSet(showLogOpt) ) {
        Logger::instance().setQDebugWrapper();
    }

    AppCore * core = new AppCore(nullptr);

    core->upgrade(parser.value(confFileOpt));

    return a.exec();
}
