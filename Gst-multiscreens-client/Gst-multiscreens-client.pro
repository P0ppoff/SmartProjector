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
    client.cpp \
    mainwindow.cpp \
    logwindow.cpp \
    wmctrl.c

HEADERS  += client.h \
    mainwindow.h \
    logwindow.h \
    wmctrl.h

FORMS    += \
    mainwindow.ui \
    logwindow.ui


unix: LIBS += -L/usr/X11/lib -lX11 -lstdc++ `pkg-config --cflags --libs gstreamer-1.0 gstreamer-video-1.0 gstreamer-base-1.0`


unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += gstreamer-1.0 gstreamer-video-1.0 gstreamer-base-1.0 glib-2.0

DISTFILES +=
