#ifndef CLIENT_H
#define CLIENT_H
#include <QTcpSocket>
#include <QObject>
#include <QDebug>
#include <QThread>
#include <QJsonObject>
#include <QMutex>
#include "server.h"
#include "message.h"

class Client : public QTcpSocket
{
    Q_OBJECT
public:
    Client(qintptr socketDescriptor = 0, QObject* parent = 0);
private:
    Server* server; //reference to server
    quint16 m_nNextBlockSize; //size of incoming packets for client
    QString userName; //client name

private:
    void handleMessage(QJsonObject message);
    void destroySelf();

signals:
    void sendMessage(const QJsonObject& msg);

public slots:
    void onSendMessage(const QJsonObject& msg);
    void slotReadyRead();
    void slotDisconnected();
    void slotTimeElapsed();
};

#endif // CLIENT_H
