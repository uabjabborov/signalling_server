#include <QCoreApplication>
#include <QDebug>
#include <QThread>
#include <QSettings>
#include <QString>
#include <iostream>
#include <QJsonObject>
#include <QDataStream>
#include <QJsonDocument>
#include <QJsonArray>
#include "server.h"
#include "message.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QSettings settings(QString("config.ini"), QSettings::IniFormat);
    QString address = settings.value("Host/ip", QString("localhost")).toString();
    quint32 portNumber = settings.value("Host/port", 8080).toInt();
    quint8 numberOfThreads = settings.value("App/nthread", QThread::idealThreadCount()).toInt();
    quint32 requestTimeout = settings.value("App/timeOut", 30).toInt();

    Server server(0, address, portNumber, requestTimeout, numberOfThreads);
    server.start();

    return a.exec();
}
