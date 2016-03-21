#ifndef CLIENT_H
#define CLIENT_H

#include <QtGui>
#include <QtNetwork>
#include <QObject>
#include <QString>
#include <QTcpSocket>
#include "ui_logwindow.h"

class Client: public QWidget, private Ui::LogWindow
{
    Q_OBJECT

    public:
      Client();
      void start(QString address, quint16 port);
      void EnvoyerMessage(const QString &message);

    public slots:
      void donneesRecues();
      void connecte();


private slots:
      void on_pushButton_clicked();

private:
      QTcpSocket *socket; // Représente le serveur
      quint16 tailleMessage;
};
#endif // CLIENT_H
