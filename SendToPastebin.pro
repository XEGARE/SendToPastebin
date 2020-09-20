#-------------------------------------------------
#
# Project created by QtCreator 2020-09-18T19:38:15
#
#-------------------------------------------------

QT       += core gui svg network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SendToPastebin
TEMPLATE = app

RC_ICONS = pastebin.ico

SOURCES += main.cpp\
        sendtopastebin.cpp

HEADERS  += sendtopastebin.h

RESOURCES += \
    resources.qrc
