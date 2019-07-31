#include <QCoreApplication>
#include "appcore.h"
#include <QDebug>


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    AppCore * core = new AppCore(0);

    return a.exec();
}
