#include "server.h"
#include "client.h"
#include <iostream>
#include <QHashIterator>
#include <QSettings>
#include <QDebug>
#include <QReadLocker>
#include <QWriteLocker>
#include <QTimer>

Server::Server(QObject *parent, QString hostAddress, quint32 portNumber, quint32 requestTimeout, quint16 numberOfThreads):hostAddress(hostAddress), portNumber(portNumber), requestTimeout(requestTimeout), numberOfThreads(numberOfThreads), QTcpServer(parent)
{
}


void Server::initThreads()
{
    for(int i = 0; i<numberOfThreads; i++)
    {
        QThread *thread = new QThread(this);
        thread->start();
        threads.append(thread);
        qDebug() << "connection handler [" << (i+1) << "] is ready";
    }
}

void Server::start()
{
    if(!this->listen(hostAddress, portNumber))
    {
        qDebug() << "hosting failed!";
        return;
    }

    qDebug() << "server is running on" << this->serverAddress().toString() << this->portNumber;

    initThreads();
}

void Server ::incomingConnection(qintptr socketDescriptor)
{
    Client* newClient = new Client(socketDescriptor, this);
    QThread* th = threads.at(online.size()%numberOfThreads);
    connect(newClient, &Client::readyRead, newClient, &Client::slotReadyRead, Qt::QueuedConnection);
    connect(newClient, &Client::disconnected, newClient, &Client::slotDisconnected, Qt::QueuedConnection);
    connect(newClient, &Client::sendMessage, newClient, &Client::onSendMessage, Qt::QueuedConnection);
    QTimer::singleShot(requestTimeout * 1000, newClient, &Client::slotTimeElapsed);
    newClient->moveToThread(th);
    qDebug() << "new incoming connection [" << socketDescriptor << "]";
}

void Server::setOnline(const QString &user, QSharedPointer<QTcpSocket> socket)
{
    QWriteLocker locker(&onlineMutex);
    if(!online.contains(user))
        online.insert(user, socket);
}

void Server::setOffline(const QString &user)
{
    QWriteLocker locker(&onlineMutex);
    if(online.contains(user))
        online.remove(user);
}

bool Server::isOnline(const QString &user)
{
    QReadLocker locker(&onlineMutex);
    return online.contains(user);
}

QSharedPointer<QTcpSocket> Server::getOnlineUser(const QString &user)
{
    QReadLocker locker(&onlineMutex);
    if(online.contains(user))
        return  online[user];
    return Q_NULLPTR;
}

QList<QString> Server::getOnlineUsers()
{
    QReadLocker locker(&onlineMutex);
    QHashIterator<QString, QSharedPointer<QTcpSocket>> iter(online);
    QList<QString> list;
    while(iter.hasNext())
        {
            iter.next();
            list.push_front(iter.key());
        }
    return list;
}

void Server::printOnlineUsers()
{
    QReadLocker locker(&onlineMutex);

        QHashIterator<QString, QSharedPointer<QTcpSocket>> iter(online);
        qDebug() << "online users:";
        while(iter.hasNext())
            {
                iter.next();
                qDebug() << iter.key() << "  "<<iter.value().data()->socketDescriptor();
            }
}
