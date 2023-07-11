#ifndef WEATHER_H
#define WEATHER_H

#include <QObject>
#include <QLabel>
#include <QTimer>

class Weather : public QObject
{
    Q_OBJECT
public:
    explicit Weather(QLabel* label_icon, QLabel* label_degrees,
                     QLabel* label_description, QObject* parent = nullptr);
    QJsonDocument   getResponse         (QString URL);
    QString         getGeolocation      (void);
    /*Weather Info*/
    double temperature;
    double windSpeed;
    int humidity;
    int windDegree;
    QString city;
    QString description;
    QString icon;
    QString latestUpdate;
    QJsonDocument   getWeatherResponse  (QString location);

    void displayImage(QString imagePath, QLabel * imgLabel, int height = 100 , int width = 100);
public slots:
    void WeatherUpdate();
private:
    QLabel* label_icon_;
    QLabel* label_degrees_;
    QLabel* label_description_;
    QTimer* WeatherTimer;
    void UpdateUI (void);
};

#endif // WEATHER_H
