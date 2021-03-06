#-------------------------------------------------
#
# Project created by QtCreator 2016-03-15T13:44:33
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Gst-multiscreens-serveur
TEMPLATE = app


SOURCES += main.cpp \
    server.cpp \
    user.cpp

HEADERS  += \
    server.h \
    user.h

FORMS    +=

unix: LIBS += `pkg-config --cflags --libs gstreamer-1.0 gstreamer-video-1.0 gstreamer-base-1.0`


unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += gstreamer-1.0 gstreamer-video-1.0 gstreamer-base-1.0

RESOURCES += resources.qrc
