#include <QCoreApplication>
#include "appcore.h"
#include <QDebug>


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    AppCore * core = new AppCore(nullptr);

    core->upgrade("./updateManager.cnf");

    return a.exec();
}
