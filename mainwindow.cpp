#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "logger.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->logListView->hide();
    window()->adjustSize();

    connect(&Logger::instance(), &Logger::newMsg, this, &MainWindow::onNewMsg);

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
