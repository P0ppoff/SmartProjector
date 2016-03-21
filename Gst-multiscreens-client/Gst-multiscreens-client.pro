#-------------------------------------------------
#
# Project created by QtCreator 2016-03-15T15:08:52
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Gst-multiscreens-client
TEMPLATE = app


SOURCES += main.cpp\
    client.cpp

HEADERS  += client.h

FORMS    += \
    logwindow.ui


unix: LIBS += `pkg-config --cflags --libs gstreamer-1.0 gstreamer-video-1.0 gstreamer-base-1.0`


unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += gstreamer-1.0 gstreamer-video-1.0 gstreamer-base-1.0

DISTFILES +=
