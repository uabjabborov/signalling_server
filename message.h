#ifndef MESSAGE_H
#define MESSAGE_H

#include <QObject>
#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QList>

class Message : public QObject
{
    Q_OBJECT
    Q_ENUMS(Commands)
    Q_ENUMS(Types)

public:
    enum Commands {GET = 10, SEND = 11, AUTH = 12, RESPONSE = 13, POST = 14, UNKNOWN_COMMAND = 0};
    Commands cmd;

    enum Types {SUCCESS = 20, FAILURE = 21, MESSAGE = 22, DELIVERED = 23, UNKNOWN_TYPE = 0};
    Types type;

public:
    Message(QJsonObject json = QJsonObject(), QObject* parent = 0);
    Message(Commands cmd = Message::Commands::UNKNOWN_COMMAND, Types type = Message::Types::UNKNOWN_TYPE, QString sender="", QString receiver = "", QList<QString> message = QList<QString>(), QObject* parent = 0);
    //Message(const Message& msg);

public:
    QString getSender() const;
    QString getReceiver() const;
    QList<QString> getMessage() const;

    Commands getCommmand() const;
    Types getType() const;

public:
    void setMessageID(qint64 id);
    qint64 getMessageID() const;
    QJsonObject getJsonMessage() const;

private:
    QList<QString> message;
    QString sender;
    QString receiver;
    QJsonObject parsedMessage;
    qint64 messageID;
private:
    void parseToJson();
    void parseFromJson(QJsonObject json);
};



#endif // MESSAGE_H
