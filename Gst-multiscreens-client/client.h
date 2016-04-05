#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QString>
#include <QTcpSocket>
#include <QStackedWidget>
#include <QRect>
#include <QApplication>
#include <QDesktopWidget>

#include <gst/gst.h>
#include <gst/video/videooverlay.h>

#include "mainwindow.h"
#include "logwindow.h"


class Client: public QWidget
{
    Q_OBJECT
    QStackedWidget* _stack;
    LogWindow* w1 ;
    MainWindow* w2 ;
    GError* err;
    GstElement *pipeline;
    GstBus *bus;
    GstMessage *msg;
    QString userName;

    public:
      Client();
      void processRequest(const QString &message);
      void start(QString address, quint16 port);
      void sendMessage(const QString &message);

   private slots:
      void receiveMessage();
      void connexionLost();
      void connecte();
      void connexionSuccess();
      void sendScreen();
      void sendCastingValue(bool b);
      void sendChat();
      void clickSendWindows();
      void sendWindowsForLinux(QStringList toSend);

    private:
      QTcpSocket *socket; // Représente le serveur
      quint16 tailleMessage;
      int port;
      bool isSending;
      bool isTeacher;
      int nbClientsSending;
      QRect screen;
};
#endif // CLIENT_H
