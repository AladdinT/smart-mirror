#include "headers/mainwindow.h"
#include "ui_mainwindow.h"
#include "headers/server.h"
#include <QObject>
#include <QDebug>
#include <QDateTime>
#include <QPixmap>
#include <QThread>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>
#include <QProcess>
#include <QtNetwork/QtNetwork>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QEventLoop>
#include <QMovie>
#include <QLabel>
#include <QSize>
#include "headers/calendar.h"
#include "headers/timeupdater.h"
#include "headers/activityRecommend.h"

/**********Global Variables*****************/
#define ICONS_ARR_LENGTH   6
QString icons_arr[6] =
{
    "blackSquare",
    "home",
    "calendar",
    "smartHome",
    "settings",
    "blackSquare"
};


QMap<int, QString> gpioStates;



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /*Recommend an Activity*/
    Activity* myAct = new Activity(ui->label_activity,this);
    myAct->requestActivity();

    /*Calendar Init*/
    calendar * myCalendar = new calendar(ui->listWidget_today, ui->listWidget_tomorrow);
    connect(ui->pushButton_refresh_calendar, &QPushButton::clicked, [=]() {
        myCalendar->refreshCalendarLists();
    });


    /*TimeUpdater Init*/
    TimeUpdater* timeUpdater = new TimeUpdater(ui->label_time, ui->label_date, ui->label_am_pm);
    timeUpdater->updateLabels();

    /*Weather Init*/
    Weather* nowWeather = new Weather(ui->icon_label, ui->big_weather_label, ui->descriptive_label);
    QString printableWeather;
    printableWeather = "Humidity : " + QString::number(nowWeather->humidity) + "%\nWind : " + QString::number(nowWeather->windSpeed , 'f',2) + " km/hr";
    ui->weather_label->setText(printableWeather);

    //Run myAImodule.py in background
    system(qPrintable("python3 " + QString(PROJECT_DIR)+ "/Plugins/myAiModule/myAImodule.py") );


    /*Handle gestures of new message*/
    Server *pyServer = new Server;
    AiModule* myAi = new AiModule(ui->tabWidget, pyServer);
    connect(pyServer, &Server::newMessage, [=]{
        ui->label_face->setText(myAi->currentFace);
        ui->label_gesture->setText(myAi->currentGesture);
        if(myAi->currentFace == "unknown")
        {
#if UNKNOWN_CONTROL == NOT_ALLOWED
            myAi->currentGesture = " ";
#endif
        }
        gestureHanlder(myAi);
    });



    /*Every 3.5 sec Update Smart Home status*/
    homeTimer = new QTimer(this);
    connect(homeTimer, &QTimer::timeout , [=]{ updateGpioStatus(); updateLedButtons(); } );
    homeTimer->start(3500);
    setButtonGIF(":/Media/icons/fan.gif",ui->pushButton_fan_on);

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::setButtonGIF(QString gifPath, QPushButton * pushButton)
{
    /*WARNING: this is not generic as it uses a static QMovie object */
    static QMovie *movie = new QMovie();

    /*the if is a workaround to run this only once */
    if(movie->fileName().isNull())
    {
        /*Update the ui with every .gif frame*/
        MainWindow::connect(movie, &QMovie::frameChanged, [=]{
          pushButton->setIcon(movie->currentPixmap().scaled(225, 225, Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
        });
        qDebug() << "connecting";
    }
    movie->stop();
    movie->setFileName(gifPath);
    movie->start();


}

void MainWindow::displayImage (QString imagePath, QLabel * imgLabel, int height , int width)
{
    /*Assignes a picture to a label*/
    QPixmap pix(imagePath);
    pix = pix.scaled(width, height , Qt::KeepAspectRatio, Qt::SmoothTransformation);
    imgLabel->setPixmap(pix);
}
void sendGPIORequest(const QString& ipAddress, int gpioNumber, const QString& state)
{
    // create custom temporary event loop on stack
    QEventLoop eventLoop;
    // Create a network access manager
    QNetworkAccessManager* manager = new QNetworkAccessManager();

    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
    // Construct the request URL
    QString requestUrl = "http://" + ipAddress + "/" + QString::number(gpioNumber) + "/" + state;
    QNetworkRequest request(requestUrl);

    // Send the HTTP request to control the specified GPIO
    QNetworkReply* reply = manager->get(request);
    eventLoop.exec(); // blocks stack until "finished()" has been called

    if (reply->error() == QNetworkReply::NoError)
    {
        //success
        qDebug() << "API" << requestUrl.left(30) << "call :: Succeeded" ;
        delete reply;
    }
    else
    {
        //failure
        qDebug() << "API" << requestUrl.left(30) << "call :: Failed" ;
        delete reply;
    }
    manager->deleteLater();


}



// Function to send a GET request to the ESP32 server and retrieve the status JSON
void MainWindow::updateGpioStatus()
{
    // create custom temporary event loop on stack
    QEventLoop eventLoop;
    // Create a network access manager
    QNetworkAccessManager* manager = new QNetworkAccessManager();
    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

    // Construct the request URL
    QString requestUrl = "http://"+QString(ESP32_IP)+"/status";
    QNetworkRequest request(requestUrl);

    QNetworkReply* reply = manager->get(request);
    eventLoop.exec(); // blocks stack until "finished()" has been called

    if (reply->error() == QNetworkReply::NoError)
    {
            QByteArray response = reply->readAll();
            QJsonDocument jsonResponse = QJsonDocument::fromJson(response);
            QJsonObject json = jsonResponse.object();
//            qDebug() << json;

            // Update states
            gpioStates[25] = json["GPIO25"].toString();
            gpioStates[2] = json["GPIO2"].toString();

    }
    else
    {
            // Handle error
            qDebug() << "Error: " << reply->errorString();
    }

        reply->deleteLater();
        manager->deleteLater();
}



void MainWindow::on_pushButton_led_on_clicked()
{
    //IP address of ESP32 board
    QString ipAddress = QString(ESP32_IP);

    // Toggle GPIO 2
    sendGPIORequest(ipAddress, 2, "on");

    //Update status
    updateGpioStatus();
    updateLedButtons();
}


void MainWindow::on_pushButton_led_off_clicked()
{
    //IP address of ESP32 board
    QString ipAddress = QString(ESP32_IP);

    // Toggle GPIO 2
    sendGPIORequest(ipAddress, 2, "off");

    //Update status
    updateGpioStatus();
    updateLedButtons();
}

void MainWindow::updateLedButtons()
{
    //Update Buttons
    QPixmap * myIcon = new QPixmap(":/Plugins/smartHome/"+ gpioStates[2] + "_lamp.png");
    QString selected_ss = "background-color: #333333; \
            color: #FFFFFF;\
            border-radius: 10px;\
            padding: 10px;\
            padding-left: 15px; \
            border-style: dashed;\
            icon-size: 128px; \
            border-width: 5px;\
            border-color : #3ACCCC;";
    QString not_selected_ss = "background-color: #333333; \
            color: #FFFFFF;\
            border-radius: 10px;\
            padding: 10px;\
            padding-left: 15px; \
            border-style: dashed;\
            icon-size: 128px; \
            border-width: 0px;\
            border-color : #3ACCCC;";

//    qDebug() << gpioStates[2] ;
    if(gpioStates[2] == "on")
    {
        ui->pushButton_led_on->setStyleSheet(selected_ss);
        ui->pushButton_led_off->setStyleSheet(not_selected_ss);
    }
    else if(gpioStates[2] == "off")
    {
        ui->pushButton_led_off->setStyleSheet(selected_ss);
        ui->pushButton_led_on->setStyleSheet(not_selected_ss);
    }

    if(gpioStates[25] == "on")
    {
        ui->pushButton_fan_on->setStyleSheet(selected_ss);
        ui->pushButton_fan_off->setStyleSheet(not_selected_ss);
    }
    else if(gpioStates[25] == "off")
    {
        ui->pushButton_fan_off->setStyleSheet(selected_ss);
        ui->pushButton_fan_on->setStyleSheet(not_selected_ss);
    }
    delete (myIcon);

}


void MainWindow::gestureHanlder(AiModule* obj)
{

    if(obj->currentGesture == "0")
    {

    }
    else if(obj->currentGesture == "1")
    {
        ui->tabWidget->setCurrentIndex(1);
    }
    else if(obj->currentGesture== "2")
    {
        ui->tabWidget->setCurrentIndex(2);
    }
    else if(obj->currentGesture == "3")
    {
        ui->tabWidget->setCurrentIndex(3);
    }

    if(ui->tabWidget->currentIndex()== 2)
    {
        if(obj->currentGesture == "6")
        {
            ui->pushButton_refresh_calendar->click();
        }
    }
    else if(ui->tabWidget->currentIndex()== 3)
    {
        if(obj->currentGesture == "6")
        {
            ui->pushButton_led_on->click();
        }
        else if(obj->currentGesture == "7")
        {
            ui->pushButton_led_off->click();
        }
        else if(obj->currentGesture == "8")
        {
            ui->pushButton_fan_on->click();
        }
        else if(obj->currentGesture == "9")
        {
            ui->pushButton_fan_off->click();
        }
    }

}




void MainWindow::on_pushButton_fan_off_clicked()
{
    //IP address of ESP32 board
    QString ipAddress = QString(ESP32_IP);

    // Toggle GPIO 2
    sendGPIORequest(ipAddress, 25, "off");

    //Update status
    updateGpioStatus();
    updateLedButtons();
}

void MainWindow::on_pushButton_fan_on_clicked()
{
    //IP address of ESP32 board
    QString ipAddress = QString(ESP32_IP);

    // Toggle GPIO 2
    sendGPIORequest(ipAddress, 25, "on");

    //Update status
    updateGpioStatus();
    updateLedButtons();
}
