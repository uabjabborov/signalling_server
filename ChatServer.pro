#-------------------------------------------------
#
# Project created by QtCreator 2018-04-10T12:14:30
#
#-------------------------------------------------

QT       += core
QT       += sql
QT       += network

QT       -= gui

TARGET = ChatServer
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    server.cpp \
    client.cpp \
    message.cpp

HEADERS += \
    server.h \
    client.h \
    message.h
