#ifndef AIMODULE_H
#define AIMODULE_H

#include <QObject>
#include "server.h"
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "conf.h"

class AiModule : public QObject
{
    Q_OBJECT
public:
    explicit AiModule(QTabWidget* myTabs,Server *pyServer, QObject *parent = nullptr);
    QString currentFace = "None" ;
    QString currentGesture = "None" ;
    Server *pyServer_;
    QTabWidget *myTabs_;
signals:

public slots:
    void parseData(Server *myServer);
    void facesHanlder();
};

#endif // AIMODULE_H
