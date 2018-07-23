#ifndef SERVER_H

#define SERVER_H
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include <QReadWriteLock>
#include <QDebug>
#include <QJsonArray>
#include <QVector>
#include <QSharedPointer>
#include <QList>

class Server : public QTcpServer
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = 0, QString hostAddress = "localhost", quint32 portNumber = 8080, quint32 requestTimeout = 30, quint16 numberOfThreads = QThread::idealThreadCount());

    friend class Client; //declaring Client as friend to give it access to private members of server;
public:
    void start (); // start server
    void printOnlineUsers();
    QList<QString> getOnlineUsers(); // get array of online users


private:
    //host configuration parameters
    QHostAddress hostAddress;
    quint32 portNumber;
    qint32 requestTimeout;
    quint16 numberOfThreads;

    //threads serving incoming clients
    QVector<QThread*> threads;

    //hashmap of online clients
    QHash<QString, QSharedPointer<QTcpSocket>> online;
    QReadWriteLock onlineMutex;

private:
    void initThreads(); // initialize threads
    void setOnline (const QString &user, QSharedPointer<QTcpSocket> socket); //set client online once client is authentificated
    void setOffline (const QString &user); //set client offline
    bool isOnline (const QString &user); //check whether client is online
    QSharedPointer <QTcpSocket> getOnlineUser(const QString &user); //get reference of specific client

signals:

public slots:
protected:
    void incomingConnection(qintptr socketDescriptor);
};

#endif // SERVER_H
