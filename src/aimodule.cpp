#include "headers/aimodule.h"

AiModule::AiModule( QTabWidget *myTabs,Server *pyServer, QObject *parent)
    : QObject(parent) , pyServer_(pyServer), myTabs_(myTabs)
{
    connect(pyServer_, &Server::newMessage, [=]{ parseData(pyServer);} );
}
void AiModule::parseData(Server *myServer)
{
    /*update current face & gesture*/
    QJsonDocument recvData;
    recvData = QJsonDocument::fromJson( (myServer->recvMessage).toUtf8() );
    currentFace = recvData.object()["face"].toString();
    currentGesture = recvData.object()["gesture"].toString();
    qDebug() << currentFace << currentGesture;
    /*Handler*/
    facesHanlder();
}


void AiModule::facesHanlder()
{
    static unsigned int counter_no_face = 0;
    static unsigned int counter_unknown = 0;

    if(currentFace != "unknown" && currentFace != "no_face")
    {
        counter_no_face = 0;
        counter_unknown = 0;
        myTabs_->setIconSize(QSize(250,150));
    }
    else if (currentFace == "no_face")
    {
        counter_no_face ++;
    }
    else if(currentFace == "unknown")
    {
        counter_unknown++;
    #if UNKNOWN_CONTROL == NOT_ALLOWED
        currentGesture = " ";
    #endif
    }

    if(counter_no_face >= BLOCK_MARGIN || counter_unknown >= BLOCK_MARGIN)
    {
        //sleep handler
        counter_no_face = 0;
        counter_unknown = 0;

        myTabs_->setCurrentIndex(1);
        myTabs_->setIconSize(QSize(0,0));
    }
}
