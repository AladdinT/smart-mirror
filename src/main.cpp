#include "headers/mainwindow.h"
#include "headers/server.h"
#include "headers/client.h"
#include <QApplication>
#include <QDebug>
#include <QThread>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
//    w.show();
    w.showFullScreen();
    return a.exec();
}
