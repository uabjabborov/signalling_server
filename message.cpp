#include "message.h"

Message::Message(QJsonObject json, QObject *parent):QObject(parent)
{
    parseFromJson(json);
    parsedMessage = json;
}

Message::Message(Commands cmd, Types type, QString sender, QString receiver, QList<QString> message, QObject *parent): cmd(cmd), type(type), sender(sender), receiver(receiver), message(message), messageID(0), QObject(parent)
{
    parseToJson();
}

void Message::parseToJson()
{
    parsedMessage["command"] = this->cmd;
    parsedMessage["type"] = this->type;
    parsedMessage["sender"] = this->sender;
    parsedMessage["receiver"] = this->receiver;
    parsedMessage["messageID"] = this->messageID;
    QJsonArray array;
    for(QString str : message)
    {
       array.append(str);
    }

    parsedMessage["message"] = array;
}

void Message::parseFromJson(QJsonObject json)
{
    this->cmd = static_cast <Message::Commands> (json["command"].toInt());
    this->type = static_cast <Message::Types> (json["type"].toInt());
    this->sender = json["sender"].toString();
    this->receiver = json["receiver"].toString();
    this->messageID = json["messageID"].toDouble();
    QList<QString> text;
    QJsonArray array = json["message"].toArray();
    for(int i = 0; i < array.size(); i++)
    {
       text.push_back(array.at(i).toString());
    }
    this->message = text;
}



QJsonObject Message::getJsonMessage() const
{
    return parsedMessage;
}

QString Message::getSender() const
{
    return this->sender;
}

QString Message::getReceiver() const
{
    return this->receiver;
}

QList<QString> Message::getMessage() const
{
    return this->message;
}

Message::Commands Message::getCommmand() const
{
    return this->cmd;
}

Message::Types Message::getType() const
{
    return this->type;
}

void Message::setMessageID(qint64 id)
{
    this->messageID = id;
    parsedMessage["messageID"] = this->messageID;
}

qint64 Message::getMessageID() const
{
    return this->messageID;
}
