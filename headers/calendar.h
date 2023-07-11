#ifndef CALENDAR_H
#define CALENDAR_H

#include <QObject>
#include <QWidget>
#include <QListWidget>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QListWidget>
#include <QListWidgetItem>
#include <QDate>
#include <QtNetwork/QtNetwork>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QEventLoop>
#include <QFile>
#include <QLabel>
#include <cstdlib>
#include <time.h>
#include "conf.h"

class calendar : public QObject
{
    Q_OBJECT
public:
    explicit calendar (QListWidget * listWidget_today,
                      QListWidget * listWidget_tomorrow, QObject *parent = nullptr);
    int silncer = 0;
    void refreshCalendarLists(void);

private :
    QListWidget * listWidget_today_;
    QListWidget * listWidget_tomorrow_;

    QString runGCalApi(void);
    QListWidgetItem * pushStyledText(QString text , QString StyleSheet , QListWidget * listWidget);
    void setupDate(void);
    void setupCalendarStyleSheet(void);

};

#endif // CALENDAR_H
