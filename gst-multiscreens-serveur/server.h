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
    QByteArray createPacket(const QString &message);
    void sendToAll(const QString &message);
    void sendToAllConnected(const QString &message);
    void sendToClient(const QString &message,QTcpSocket*socket);
    void processRequest(const QString &message,QTcpSocket*socket);
    void setPipeline();

public slots:
    void nouvelleConnexion();
    void receiveMessage();
    void clientDisconnected();


private:
    QRect screen;
    int getIndexFromSocket(QTcpSocket*socket);
    QTcpServer *serveur;
    QList<User> clients;
    //QList<QTcpSocket *> clients;
    int allocatedPort;
    quint16 tailleMessage;
    QString localIp;
    int verifyDataBase(QString login,QString pwd);
    bool alreadyConnected(QString name);

};


#endif // SERVER_H
