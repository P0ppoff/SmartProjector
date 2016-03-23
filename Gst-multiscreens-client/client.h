#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QString>
#include <QTcpSocket>
#include <QStackedWidget>
#include "mainwindow.h"
#include "logwindow.h"


class Client: public QWidget
{
    Q_OBJECT
    QStackedWidget* _stack;
    LogWindow* w1 ;
    MainWindow* w2 ;

    public:
      Client();

      void start(QString address, quint16 port);
      void EnvoyerMessage(const QString &message);

    public slots:
      void donneesRecues();
      void connecte();
      void sendCommand();



private slots:

private:
      QTcpSocket *socket; // Représente le serveur
      quint16 tailleMessage;
};
#endif // CLIENT_H
