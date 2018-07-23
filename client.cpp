#include "client.h"
#include <QJsonDocument>
#include <QDataStream>
#include <QMutexLocker>
#include <QTimer>
#include <QMetaObject>
#include <QSharedPointer>
#include <QDateTime>
#include <QMetaEnum>

Client::Client(qintptr socketDescriptor, QObject *parent)
{
    if(!this->setSocketDescriptor(socketDescriptor))
    {
        qDebug() << "unable to create new socket " + socketDescriptor;
        return;
    }
    this->userName.clear();
    this->server = static_cast<Server*>(parent);
    this->m_nNextBlockSize = 0;
}

void Client::slotReadyRead()
{
    QDataStream in(this);
    while(this->bytesAvailable() > 0)
    {
        if(!m_nNextBlockSize){
            if(this->bytesAvailable()<sizeof(quint16))
                break;
            in >> m_nNextBlockSize;
        }
        if(this->bytesAvailable() < m_nNextBlockSize)
            break;
        QByteArray bytes;
        in >> bytes;
        handleMessage(QJsonDocument::fromBinaryData(bytes).object());
        m_nNextBlockSize = 0;
    }
}

void Client::handleMessage(QJsonObject message)
{
    QSharedPointer<QTcpSocket> socket;
    QSharedPointer<Client> target;

    Message parsedMessage(message);
    Message::Commands cmd = parsedMessage.getCommmand();
    Message::Types type = parsedMessage.getType();

    qDebug() << parsedMessage.getSender() << "->" << parsedMessage.getReceiver() << QDateTime::currentDateTime().toString();
    switch (cmd) {
    case Message::Commands::AUTH:
        if(server->isOnline(parsedMessage.getSender()))
        {
            emit sendMessage(Message(Message::Commands::AUTH, Message::Types::FAILURE).getJsonMessage());
            break;
        }
        this->userName = parsedMessage.getSender();
        this->server->setOnline(this->userName, QSharedPointer<Client>(this));
        qDebug() << "connection [" << this->socketDescriptor() << "] is registered as" << this->userName;
        emit sendMessage(Message(Message::Commands::AUTH, Message::Types::SUCCESS, QString("server"), this->userName).getJsonMessage());
        break;

    case Message::Commands::SEND:

        if(!server->isOnline(parsedMessage.getSender()) || !server->isOnline(parsedMessage.getReceiver()))
        {
            Message failure(Message::Commands::RESPONSE, Message::Types::FAILURE, parsedMessage.getReceiver(), parsedMessage.getSender());
            failure.setMessageID(parsedMessage.getMessageID());
            emit sendMessage(failure.getJsonMessage());
            break;
        }
        socket = server->getOnlineUser(parsedMessage.getReceiver());
        target = socket.staticCast<Client>();

        if(!QMetaObject::invokeMethod(target.data(), "onSendMessage", Qt::QueuedConnection, Q_ARG(const QJsonObject&, message)))
        {
            Message failure(Message::Commands::RESPONSE, Message::Types::FAILURE, parsedMessage.getReceiver(), parsedMessage.getSender());
            failure.setMessageID(parsedMessage.getMessageID());
            emit sendMessage(failure.getJsonMessage());
        }
        break;

    case Message::Commands::RESPONSE:
        if(!server->isOnline(parsedMessage.getReceiver()))
        {
            break;
        }
        switch (type) {
        case Message::Types::DELIVERED:
            socket = server->getOnlineUser(parsedMessage.getReceiver());
            target = socket.staticCast<Client>();
            QMetaObject::invokeMethod(target.data(), "onSendMessage", Qt::QueuedConnection, Q_ARG(const QJsonObject&, message));
            break;
        default:
            break;
        }

        break;

    case Message::Commands::GET:
        if(server->isOnline(parsedMessage.getSender()))
        {
            QList<QString> text = server->getOnlineUsers();
            emit sendMessage(Message(Message::Commands::POST, Message::Types::MESSAGE, QString("server"), this->userName, text).getJsonMessage());
        }
        break;
    default:
        emit sendMessage(Message(Message::Commands::RESPONSE, Message::Types::FAILURE).getJsonMessage());
        emit disconnected();
        break;
    }
}

void Client::onSendMessage(const QJsonObject &msg)
{
    QByteArray arrBlock;
    QDataStream out (&arrBlock, QIODevice::WriteOnly);
    QJsonDocument doc(msg);
    out << quint16(0) << doc.toBinaryData();
    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));
    if(this->isOpen())
        this->write(arrBlock);
}

void Client::slotDisconnected()
{
    if(!this->userName.isNull())
        qDebug() << this->userName << "disconnected";
    else
        qDebug() << "connection [" << this->socketDescriptor() << "] aborted";
        destroySelf();
}


void Client::destroySelf()
{
    if(!this->userName.isNull())
    {
        server->setOffline(this->userName);
    }
    else
        this->deleteLater();
}

void Client::slotTimeElapsed()
{
    if(this->userName.isNull())
        emit disconnected();
}



