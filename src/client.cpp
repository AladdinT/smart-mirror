#include "headers/client.h"

Client::Client(quint16 port)
{
    socket = new QTcpSocket(this);

    connect(this, &Client::newMessage, this, &Client::displayMessage);
    connect(socket, &QTcpSocket::readyRead, this, &Client::readSocket);
    connect(socket, &QTcpSocket::disconnected, this, &Client::discardSocket);
//    connect(socket, &QAbstractSocket::errorOccurred, this, &Client::displayError);

    socket->connectToHost(QHostAddress::LocalHost,port);

    if(socket->waitForConnected())
    {
        displayMessage("Connected to Server");
        on_pushButton_sendMessage_clicked();
    }
    else
    {
        QMessageBox::critical(this,"QTCPClient", QString("The following error occurred: %1.").arg(socket->errorString()));
    }
}

Client::~Client()
{
    if(socket->isOpen())
        socket->close();
}

void Client::readSocket()
{
    QByteArray buffer;

    QDataStream socketStream(socket);
//    socketStream.setVersion(QDataStream::Qt_5_15);

    socketStream.startTransaction();
    socketStream >> buffer;

    if(!socketStream.commitTransaction())
    {
        QString message = QString("%1 :: Waiting for more data to come..").arg(socket->socketDescriptor());
        emit newMessage(message);
        return;
    }


    QString message = QString("%1 :: %2").arg(socket->socketDescriptor()).arg(QString::fromStdString(buffer.toStdString()));
    emit newMessage(message);

}

void Client::discardSocket()
{
    socket->deleteLater();
    socket=nullptr;

    displayMessage("Disconnected!");
}

void Client::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
        case QAbstractSocket::RemoteHostClosedError:
        break;
        case QAbstractSocket::HostNotFoundError:
            QMessageBox::information(this, "QTCPClient", "The host was not found. Please check the host name and port settings.");
        break;
        case QAbstractSocket::ConnectionRefusedError:
            QMessageBox::information(this, "QTCPClient", "The connection was refused by the peer. Make sure QTCPServer is running, and check that the host name and port settings are correct.");
        break;
        default:
            QMessageBox::information(this, "QTCPClient", QString("The following error occurred: %1.").arg(socket->errorString()));
        break;
    }
}

void Client::on_pushButton_sendMessage_clicked()
{
    if(socket)
    {
        if(socket->isOpen())
        {
            QString str = "Hello from client"; //TODO

            QDataStream socketStream(socket);
//            socketStream.setVersion(QDataStream::Qt_5_15);


            QByteArray byteArray = str.toUtf8();
            socketStream << byteArray;

        }
        else
            QMessageBox::critical(this,"QTCPClient","Socket doesn't seem to be opened");
    }
    else
        QMessageBox::critical(this,"QTCPClient","Not connected");
}

void Client::displayMessage(const QString& str)
{
    qDebug() << (str);
}
