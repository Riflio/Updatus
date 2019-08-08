#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class AppCore;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent, AppCore * core);
    ~MainWindow();

private slots:
    void on_logBtnShow_clicked();
    void onNewMsg(QString msg);
    void onProgressChanged(int pr);
    void onNewStatus(QString st, int mode);

private:
    Ui::MainWindow *ui;
    AppCore * _core;

};

#endif // MAINWINDOW_H
