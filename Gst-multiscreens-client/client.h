#ifndef CLIENT_H
#define CLIENT_H

#include <QtNetwork>
#include <QObject>
#include <QString>
#include <QTcpSocket>

class Client: public QObject
{
Q_OBJECT
public:
  Client(QObject* parent = 0);
  void EnvoyerMessage(const QString &message);
  void start(QString address, quint16 port);
public slots:
  void donneesRecues();
  void connecte();
private:
  QTcpSocket *socket; // Représente le serveur
  quint16 tailleMessage;
};
#endif // CLIENT_H
