#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QString>
#include <QTcpSocket>
#include <QStackedWidget>

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
    QString userName;

    public:
      Client();
      void processRequest(const QString &message);
      void start(QString address, quint16 port);
      void EnvoyerMessage(const QString &message);

    public slots:
      void donneesRecues();
      void connexionLost();
      void connecte();
      void connexionSuccess();
      void sendScreen();
      void sendCastingValue(bool b);
      void sendChat();



private slots:

private:
      QTcpSocket *socket; // Représente le serveur
      quint16 tailleMessage;
      int port;
};
#endif // CLIENT_H
