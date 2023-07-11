#ifndef TIMEUPDATER_H
#define TIMEUPDATER_H

#include <QObject>
#include <QLabel>
#include <QTimer>
#include <QDateTime>

class TimeUpdater : public QObject
{
    Q_OBJECT
public:
    explicit TimeUpdater(QLabel* label_time, QLabel* label_date, QLabel* label_am_pm, QObject* parent = nullptr)
        : QObject(parent), label_time_(label_time), label_date_(label_date), label_am_pm_(label_am_pm)
    {
        // Create a timer and connect its timeout() signal to our updateLabels() slot
        timer_ = new QTimer(this);
        connect(timer_, &QTimer::timeout, this, &TimeUpdater::updateLabels);

        // Update the labels immediately
        updateLabels();

        // Start the timer to update every second (1000 milliseconds)
        timer_->start(1000);
    }

public slots:
    void updateLabels()
    {
        /*This Function should be called every 1 sec to update time*/
        /*Get Date & Time*/
        QTime time = QTime::currentTime();
        QDate date = QDate::currentDate();

        /*Convert to readable strings in the given format*/
        QString time_txt = time.toString("hh:mmA");
        time_txt.chop(2);
        QString date_txt = date.toString("dddd\nd  MMM  yyyy");

        /*Displaying to UI using labels*/
        QString disp_time = time_txt + "<a style=\"font-size:18pt; \">"+ time.toString(".ss") +"</a>";
        if(time.toString("A") == "AM")
        {
            label_am_pm_->setText("AM<p style=\" color:#73777b; \">PM</p>");
        }
        else
        {
            label_am_pm_->setText("<p style=\" color:#73777b; \">AM</p>PM");
        }
        label_time_->setText(disp_time);
        label_date_->setText(date_txt);
    }

private:
    QLabel* label_time_;
    QLabel* label_date_;
    QLabel* label_am_pm_;
    QTimer* timer_;
};


#endif // TIMEUPDATER_H
