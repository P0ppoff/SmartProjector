#ifndef SERVER_H
#define SERVER_H

#include <QtNetwork>
#include <QObject>
#include <QWidget>

#include <gst/gst.h>
#include <gst/video/videooverlay.h>


#include "user.h"

class Server: public QWidget
{
    Q_OBJECT

    GError* err;
    GstElement *pipeline;

    public:
      Server();
      void envoyerATous(const QString &message);
      void processRequest(const QString &message,QTcpSocket*socket);
      void sendToClient(const QString &message,QTcpSocket*socket);
      void setPipeline();

    public slots:
       void nouvelleConnexion();
       void donneesRecues();
       void deconnexionClient();


    private:
      QRect screen;
      int getIndexFromSocket(QTcpSocket*socket);
      QTcpServer *serveur;
      QList<User> clients;
      //QList<QTcpSocket *> clients;
      int allocatedPort;
      quint16 tailleMessage;

};


#endif // SERVER_H
