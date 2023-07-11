#include "headers/weather.h"
#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#define _1_HR   3600000
Weather::Weather(QLabel* label_icon, QLabel* label_degrees, QLabel* label_description,  QObject* parent)
    : QObject(parent), label_icon_(label_icon), label_degrees_(label_degrees), label_description_(label_description)
{
    WeatherTimer = new QTimer(this);
    connect(WeatherTimer, SIGNAL(timeout()) , this , SLOT(WeatherUpdate()) );
    WeatherTimer->start(_1_HR);

    WeatherUpdate();

}


QJsonDocument Weather::getResponse(QString URL)
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

    return jsonDoc;
}

QString Weather::getGeolocation(void)
{
    QJsonDocument ipResponse = getResponse("https://api.ipify.org/?format=json");
    QString myIP = ipResponse.object()["ip"].toString();
    QString locationAPI = "https://ipinfo.io/"+ myIP +"/geo";
    QJsonDocument locResponse = getResponse(locationAPI);
    QString myCity = locResponse.object()["city"].toString();
    QString myCountry = locResponse.object()["country"].toString();
    QString myLocation = myCity + "," + myCountry;

    return myLocation;
}

QJsonDocument Weather::getWeatherResponse(QString location)
{
    QString apiKey = QString(OPENWEATHERMAP_API_KEY);
    QString baseURL = "http://api.openweathermap.org/data/2.5/weather?";
    QString city = location;
    QString completeURL = baseURL+"appid="+apiKey+"&q="+city;
    //http://api.openweathermap.org/data/2.5/weather?appid57cb9875a41e9615380e16c8e384e34d&q=6thofoctober
    QJsonDocument weatherDoc = getResponse(completeURL);

    return weatherDoc;
}

void Weather::WeatherUpdate()
{
    /*This Function should be called every 1 hr to update weather*/
    /*Run weather api*/
    QJsonDocument WeatherDoc = getWeatherResponse(getGeolocation());
    QJsonObject WeatherJSON = WeatherDoc.object();

    /*Extract and store necessary Info*/
    temperature = WeatherJSON["main"].toObject()["temp"].toDouble();
    windSpeed = WeatherJSON["wind"].toObject()["speed"].toDouble();
    humidity = WeatherJSON["main"].toObject()["humidity"].toInt();
    windDegree = WeatherJSON["wind"].toObject()["deg"].toInt();
    temperature = temperature - 273; //Kelvin to Celisuis
    windSpeed = windSpeed * 18 / 5; //m/s to km/hr
    city = WeatherJSON["name"].toString();
    description = WeatherJSON["weather"].toArray().first()["description"].toString();
    icon = WeatherJSON["weather"].toArray().first()["icon"].toString();

    QTime time = QTime::currentTime();
    latestUpdate= time.toString("hh:mm:ss");

    /*Display Updates*/
    UpdateUI();

    /*Debugging if function is called each 1hr*/
    qDebug() << "Weather update called ::" << latestUpdate;

}


void Weather::displayImage (QString imagePath, QLabel * imgLabel, int height , int width)
{
    /*Assignes a picture to a label*/
    QPixmap pix(imagePath);
    pix = pix.scaled(width, height , Qt::KeepAspectRatio, Qt::SmoothTransformation);
    imgLabel->setPixmap(pix);
}

void Weather::UpdateUI ()
{
    /*Update Labels*/
    QString printableTemp = QString::number(temperature,'f',0) + "°C";
    label_degrees_->setText(printableTemp);
    label_description_->setText(description + "\n" + city);
    /*display icon*/
    QString weatherIcon = ":/Plugins/weather/icons/";
    weatherIcon.append(icon);
    displayImage(weatherIcon, label_icon_ ,150,150);
}
