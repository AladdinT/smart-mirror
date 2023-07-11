#include "headers/server.h"

Server::Server( quint16 port )
{
    m_server = new QTcpServer();

    if(m_server->listen(QHostAddress::Any, port))
    {
       connect(this, &Server::newMessage, this, &Server::displayMessage);
       connect(m_server, &QTcpServer::newConnection, this, &Server::onNewConnection);
       qDebug() << ("Server is listening...");
    }
    else
    {
        QMessageBox::critical(this,"QTCPServer",QString("Unable to start the server: %1.").arg(m_server->errorString()));
    }
}

Server::~Server()
{
    foreach (QTcpSocket* socket, connection_set)
    {
        socket->close();
        socket->deleteLater();
    }

    m_server->close();
    m_server->deleteLater();
}

void Server::onNewConnection()
{
    while (m_server->hasPendingConnections())
        appendToSocketList(m_server->nextPendingConnection());

}

void Server::appendToSocketList(QTcpSocket* socket)
{
    connection_set.insert(socket);
    connect(socket, &QTcpSocket::readyRead, this, &Server::readSocket);
    connect(socket, &QTcpSocket::disconnected, this, &Server::discardSocket);
//    connect(socket, &QAbstractSocket::errorOccurred, this, &Server::displayError);

    displayMessage(QString("INFO :: Client with sockd:%1 has just entered the room").arg(socket->socketDescriptor()));
}

void Server::readSocket() //START FROM HERE
{
    QTcpSocket* socket = reinterpret_cast<QTcpSocket*>(sender());

    QDataStream socketStream(socket);
//    socketStream.setVersion(QDataStream::Qt_5_15);
    socketStream.startTransaction();

    QTextStream myText(socket);
    recvMessage = myText.readAll();

    if(!socketStream.commitTransaction())
    {
        QString message = QString("%1 :: Waiting for more data to come..").arg(socket->socketDescriptor());
        emit newMessage(message);
        return;
    }
    QString message = QString("%1 :: %2").arg(socket->socketDescriptor()).arg( recvMessage );
    emit newMessage(message);

}

void Server::discardSocket()
{
    QTcpSocket* socket = reinterpret_cast<QTcpSocket*>(sender());
    QSet<QTcpSocket*>::iterator it = connection_set.find(socket);
    if (it != connection_set.end()){
        displayMessage(QString("INFO :: A client %1 has just left the room").arg(socket->socketDescriptor()));
        connection_set.remove(*it);
    }
    socket->deleteLater();
}

void Server::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
        case QAbstractSocket::RemoteHostClosedError:
        break;
        case QAbstractSocket::HostNotFoundError:
            QMessageBox::information(this, "QTCPServer", "The host was not found. Please check the host name and port settings.");
        break;
        case QAbstractSocket::ConnectionRefusedError:
            QMessageBox::information(this, "QTCPServer", "The connection was refused by the peer. Make sure QTCPServer is running, and check that the host name and port settings are correct.");
        break;
        default:
            QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
            QMessageBox::information(this, "QTCPServer", QString("The following error occurred: %1.").arg(socket->errorString()));
        break;
    }
}

void Server::broadcastMessage()
{
    QString receiver = "Broadcast"; //TODO: make it configurable

    if(receiver=="Broadcast")
    {
        foreach (QTcpSocket* socket,connection_set)
        {
            sendMessage(socket);
        }
    }
    else
    {
        foreach (QTcpSocket* socket,connection_set)
        {
            if(socket->socketDescriptor() == receiver.toLongLong())
            {
                sendMessage(socket);
                break;
            }
        }
    }
}


void Server::sendMessage(QTcpSocket* socket)
{
    if(socket)
    {
        if(socket->isOpen())
        {
            QString str = fwdMessage; //TODO

            QDataStream socketStream(socket);
//            socketStream.setVersion(QDataStream::Qt_5_15);


//            QByteArray byteArray = str.toUtf8();

//            socketStream.setVersion(QDataStream::Qt_5_15);
//            socketStream << byteArray;
            socketStream << str;
//            serverSocket->waitForBytesWritten(-1);
        }
        else
            QMessageBox::critical(this,"QTCPServer","Socket doesn't seem to be opened");
    }
    else
        QMessageBox::critical(this,"QTCPServer","Not connected");
}


void Server::displayMessage(const QString& str)
{
    qDebug() << str ;
}

