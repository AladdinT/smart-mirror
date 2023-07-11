#ifndef ACTIVITYRECOMMEND_H
#define ACTIVITYRECOMMEND_H
#include <QApplication>
#include <QLabel>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>

class Activity  : public QObject
{
    Q_OBJECT

public:
    Activity(QLabel* label  , QObject* parent = nullptr) : QObject(parent) , label_(label)
    {
        // Create a QLabel to display the activity
        label_->setAlignment(Qt::AlignCenter);
        label_->setText("No activity");

        // Setup the timer to execute the function every hour
        timer_ = new QTimer(this);
        connect(timer_, &QTimer::timeout, this, &Activity::requestActivity);
        timer_->start(3600000); // 1 hr

    }

public slots:
    void requestActivity()
    {
        // Send a request to the Bored API
        QJsonDocument jsonDoc =  getResponse("https://www.boredapi.com/api/activity");
        QJsonObject jsonObject = jsonDoc.object();
        QString activity = jsonObject.value("activity").toString();

        // Update the QLabel with the activity
        label_->setText(activity);
    }

private:
    QLabel* label_;
    QTimer* timer_;
    QJsonDocument getResponse(QString URL)
    {
        // create custom temporary event loop on stack
        QEventLoop eventLoop;
        QJsonDocument jsonDoc;
        // "quit()" the event-loop, when the network request "finished()"
        QNetworkAccessManager mgr;
        QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

        // the HTTP request
        QNetworkRequest req( ( QUrl( URL ) ) );
        QNetworkReply *reply = mgr.get(req);
        eventLoop.exec(); // blocks stack until "finished()" has been called

        if (reply->error() == QNetworkReply::NoError)
        {
            //success
            jsonDoc = QJsonDocument::fromJson( reply->readAll() );
            qDebug() << "API" << URL.left(30) << "call :: Succeeded" ;
            delete reply;
        }
        else
        {
            //failure
            qDebug() << "API" << URL.left(30) << "call :: Failed" ;
            delete reply;
        }

        mgr.deleteLater();

        return jsonDoc;
    }

};

#endif // ACTIVITYRECOMMEND_H
