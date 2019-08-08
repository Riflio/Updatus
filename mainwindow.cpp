#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "appcore.h"
#include "logger.h"

MainWindow::MainWindow(QWidget *parent, AppCore * core) :
    QMainWindow(parent), ui(new Ui::MainWindow), _core(core)
{
    ui->setupUi(this);
    ui->logListView->hide();
    window()->adjustSize();

    connect(&Logger::instance(), &Logger::newMsg, this, &MainWindow::onNewMsg);
    connect(_core, &AppCore::progress, this, &MainWindow::onProgressChanged);
    connect(_core, &AppCore::statusChanged, this, &MainWindow::onNewStatus);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_logBtnShow_clicked()
{
    if ( !ui->logListView->isHidden() ) {
        ui->logListView->hide();
    } else {
        ui->logListView->show();
    }
}

void MainWindow::onNewMsg(QString msg)
{
    ui->logListView->addItem(msg);
}

void MainWindow::onProgressChanged(int pr)
{
    ui->progress->setValue(pr);
}

void MainWindow::onNewStatus(QString st, int mode)
{
    Q_UNUSED(mode);
    ui->lblCurrentAction->setText(st);
}
