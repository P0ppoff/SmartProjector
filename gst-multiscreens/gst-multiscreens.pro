#-------------------------------------------------
#
# Project created by QtCreator 2016-03-15T13:44:33
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Gst-multiscreens
TEMPLATE = app


SOURCES += main.cpp \
    server.cpp \
    client.cpp

HEADERS  += \
    server.h \
    client.h

FORMS    += mainwindow.ui
