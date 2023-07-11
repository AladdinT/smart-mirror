#include "headers/calendar.h"

QString paletteColorsSS (void);



calendar::calendar(QListWidget * listWidget_today, QListWidget * listWidget_tomorrow , QObject *parent)
    : QObject(parent), listWidget_today_(listWidget_today) , listWidget_tomorrow_(listWidget_tomorrow)
{
    /*List Today & Tomorrow dates*/
    setupDate();
    setupCalendarStyleSheet();

    /*Update Calendar*/
    refreshCalendarLists();

    // Setup the timer to execute the function every hour
    QTimer * timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &calendar::refreshCalendarLists);
    timer->start(3600000); // 1 hour interval

}


QString calendar::runGCalApi(void)
{
    QProcess process;
    process.setWorkingDirectory( QString(PROJECT_DIR) + "/Plugins/GCalendar/");
    process.start("sudo python3 GCapiDemo.py");
    process.waitForFinished();
    QString output = process.readAllStandardOutput();
    return output;
}

 QListWidgetItem * calendar::pushStyledText(QString text , QString styleSheet , QListWidget * listWidget)
{
    QLabel *myLabel = new QLabel;
    QListWidgetItem *myItem = new QListWidgetItem;

    myLabel->setText( text );
    myLabel->setStyleSheet( styleSheet );

    listWidget->addItem(myItem);
    listWidget->setItemWidget(myItem, myLabel);

    return myItem;
}

QString paletteColorsSS (void)
{
    static int iterator = 0;
    QString myColors [5] = {
        "#2B3467",
        "#A459D1",
        "#BAD7E9",
        "#EB455F"
    };
    QString coloredSS = "color: rgb(255,255,255); \
                        font: 30pt \"Bahnschrift Condensed\"; \
                        border-radius: 30px; \
        border: solid;\
        border-color: %1; \
        border-width: 4px; \
        padding: 10px;   \
        width: 200px; \
        height: 150px;\
        ";
        QString border_color = "qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 rgba(95, 200, 222, 255), stop:1 rgba(255, 255, 255, 255))";

    coloredSS = coloredSS.arg(myColors[iterator] /* border_color*/);
    iterator == 4-1 ? iterator=0 : iterator++ ;
    return coloredSS;
}

void calendar::refreshCalendarLists(void)
{
    /*Reset Calendar*/
    listWidget_today_->clear();
    listWidget_tomorrow_->clear();

    /*Calendar Setup*/
    setupDate();
    setupCalendarStyleSheet();

    /*Parse Json Array*/
    QString eventsStr = runGCalApi();
    QJsonDocument GoogleCalendarResponse = QJsonDocument::fromJson(eventsStr.toUtf8());
    QJsonArray eventsArr = GoogleCalendarResponse.array();

    QDate date = QDate::currentDate();

    QString buffer;
    QString endBuffer;
    QDate startDate ;
    QTime startTime ;
    QTime endTime ;
    QString eventTitle;
    for(int i =0 ; i<eventsArr.size() ; i++)
    {
        /*Data Fetch*/
        /*title fetch*/
        eventTitle.append(eventsArr[i].toObject()["summary"].toString());

        /*start/end date time fetch*/
        buffer.append(eventsArr[i].toObject()["start"].toObject()["dateTime"].toString());
        if(buffer.isEmpty())
        {
            buffer.append(eventsArr[i].toObject()["start"].toObject()["date"].toString());
        }
        else
        {
            endBuffer.append(eventsArr[i].toObject()["end"].toObject()["dateTime"].toString());
            startTime = QTime::fromString( buffer.left(11+8).right(8) , "hh:mm:ss");
            endTime = QTime::fromString(endBuffer.left(11+8).right(8) ,"hh:mm:ss");
        }
        startDate = QDate::fromString(buffer.left(10), "yyyy-MM-dd");

        /*Data display*/
        if(startDate.dayOfWeek() == date.dayOfWeek() )
        {
            /*If event's start date is today*/
            if(startTime.isValid())
            {
                eventTitle = eventTitle.append( "<br><a style=\"font-size:12pt;text-align:right;\">" + startTime.toString("hh:mma") + " - " + endTime.toString("hh:mma") + "</a>" );
            }
            pushStyledText(eventTitle, paletteColorsSS() , listWidget_today_);
        }
        else if (startDate.daysTo(date) == -1 )
        {
            /*If event's start date is tomorrow*/
            if(startTime.isValid())
            {
                eventTitle = eventTitle.append( "<br><a style=\"font-size:12pt\">" + startTime.toString("hh:mma") + " - " + endTime.toString("hh:mma") + "</a>" );
            }
            pushStyledText(eventTitle, paletteColorsSS() , listWidget_tomorrow_);
        }

        /*reset variables*/
        buffer.clear();
        endBuffer.clear();
        eventTitle.clear();
        startTime = QTime::fromString("INVALID","hh:mm:ss");
        endTime = QTime::fromString("INVALID","hh:mm:ss");
    }
}

void calendar::setupDate(void)
{
    QString HeaderStyleSheet = "color: rgb(255,255,255);\
                         background-color: #333333;\
                         font: 30pt \"Bahnschrift Condensed\";   \
                         border-radius: 30px;\
                         border-style: solid; \
                         border-color: #000000;\
                         border-width: 4px;\
                         padding: 10px;\
                         width: 200px;\
                         height: 150px;\
                         ";
    /*List Today & Tomorrow dates*/
    QDate date = QDate::currentDate();
    QString TodayDate = "Today<br><a style=\"font-size:12pt\">" + date.toString("ddd d MMM") + "</a>" ;
    pushStyledText(TodayDate, HeaderStyleSheet , listWidget_today_);

    QString TomorrowDate = "Tomorrow<br><a style=\"font-size:12pt\">" + date.addDays(1).toString("ddd d MMM") + "</a>" ;
    pushStyledText(TomorrowDate, HeaderStyleSheet , listWidget_tomorrow_);
}

void calendar::setupCalendarStyleSheet (void)
{

    /*Table Style Sheet*/
    listWidget_today_->setStyleSheet("color: rgb(255, 255, 255);\
                                    line-height: 150px; \
                                    font: 70pt \"Bahnschrift Light Condensed\";  \
                                    \
                                    height: 250px;\
                                    ");
    listWidget_today_->setFrameStyle(QFrame::NoFrame);
    listWidget_today_->setSpacing(3);

    listWidget_tomorrow_->setStyleSheet("color: rgb(255, 255, 255);\
                                         line-height: 150px; \
                                         font: 70pt \"Bahnschrift Light Condensed\";  \
                                         \
                                         height: 250px;\
                                         ");
    listWidget_tomorrow_->setFrameStyle(QFrame::NoFrame);
    listWidget_tomorrow_->setSpacing(3);

}
