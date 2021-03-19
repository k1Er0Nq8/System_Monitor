#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QDir>
#include <QTimer>
#include <QMessageBox>
#include "qcustomplot.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setupRealtimeDataDemo(QCustomPlot *customPlot);
private:
    Ui::MainWindow *ui;
    QTimer *timer;
    QTimer dataTimer;
    void show_Table_Info(int index);
    int custom_count;
    void NewTread();
    void openfile(QString filename);
private slots:
    void changeInfo(int);
    void timer_update();
    void info_update();
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_6_clicked();
};

#endif // MAINWINDOW_H
