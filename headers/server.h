// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef SERVER_H
#define SERVER_H

#include <QMainWindow>
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QMetaType>
#include <QSet>
#include <QStandardPaths>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDialog>

QT_BEGIN_NAMESPACE
class QLabel;
class QPushButton;
class QLocalServer;
QT_END_NAMESPACE

class Server : public QDialog
{
    Q_OBJECT

public:
    Server(quint16 port = 8080);

    ~Server();
    QString fwdMessage = "Hello, I am server";
    QString recvMessage ;

public slots:
    void broadcastMessage();

signals:
    void newMessage(QString);

private slots:
    void onNewConnection();
    void appendToSocketList(QTcpSocket* socket);

    void readSocket();
    void discardSocket();
    void displayError(QAbstractSocket::SocketError socketError);

    void displayMessage(const QString& str);
    void sendMessage(QTcpSocket* socket);


private:
    QTcpServer* m_server;
    QSet<QTcpSocket*> connection_set;
};

#endif


