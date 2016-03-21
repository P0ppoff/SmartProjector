#ifndef SERVER_H
#define SERVER_H

#include <QtNetwork>
#include <QObject>
#include <QWidget>


class Server: public QWidget
{
    Q_OBJECT

    public:
      Server();
      void envoyerATous(const QString &message);

    public slots:
       void nouvelleConnexion();
       void donneesRecues();
       void deconnexionClient();
       void connecte();

    private:
      QTcpServer *serveur;
      QList<QTcpSocket *> clients;
      quint16 tailleMessage;

};


#endif // SERVER_H
