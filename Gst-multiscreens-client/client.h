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

    public:
      Client();
      void processRequest(const QString &message);
      void start(QString address, quint16 port);
      void EnvoyerMessage(const QString &message);

    public slots:
      void donneesRecues();
      void lol();
      void connecte();
      void connexionSuccess();
      void sendScreen();
      void sendCastingValue(bool b);



private slots:

private:
      QTcpSocket *socket; // Représente le serveur
      quint16 tailleMessage;
};
#endif // CLIENT_H
