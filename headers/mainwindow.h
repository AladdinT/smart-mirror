#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QDebug>
#include <QPushButton>
#include <QLabel>
#include "server.h"
#include "weather.h"
#include "activityRecommend.h"
#include "aimodule.h"
#include "conf.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void displayImage(QString imagePath, QLabel * imgLabel, int height = 100 , int width = 100);
    Ui::MainWindow *ui;
//    Server pyServer;

public slots:


public slots:
   void on_pushButton_led_on_clicked();
   void on_pushButton_led_off_clicked();
   void gestureHanlder(AiModule* obj);

private slots:
   void on_pushButton_fan_off_clicked();

   void on_pushButton_fan_on_clicked();

private:
    QTimer * handleGestureTimer;
    QTimer * getFaceTimer;
    QTimer * homeTimer;

    void setButtonGIF(QString gifPath, QPushButton * pushButton);
    void updateGpioStatus();
    void updateLedButtons();


};
#endif // MAINWINDOW_H
