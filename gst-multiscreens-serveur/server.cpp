#include "server.h"
#include <iostream>
#include <QDataStream>
#include <QDesktopWidget>
#include <QApplication>

#include <gst/gst.h>
#include <gst/video/videooverlay.h>


Server::Server()
{
    // Gestion du lancement du serveur
    serveur = new QTcpServer(this);
    if (!serveur->listen(QHostAddress::Any, 50885)) // Démarrage du serveur sur toutes les IP disponibles et sur le port 50585
    {
        qDebug() << "Server failed to start";
        exit(0);
    }
    else
    {
       qDebug() << "Server Started";
       screen = QApplication::desktop()->screenGeometry();
       connect(serveur, SIGNAL(newConnection()), this, SLOT(nouvelleConnexion()));
    }
    tailleMessage = 0;
    pipeline = NULL;
    allocatedPort = 5000;
    setPipeline();
}

//SLOT : quand une nouvelle connexion est établie, ajoute un Client à la liste et le connecte
void Server::nouvelleConnexion()
{  
    User nouveauClient(serveur->nextPendingConnection()) ;


    qDebug() << "Client connected : " << nouveauClient.sock->peerAddress();
    connect(nouveauClient.sock, SIGNAL(readyRead()), this, SLOT(donneesRecues()));
    connect(nouveauClient.sock, SIGNAL(disconnected()), this, SLOT(deconnexionClient()));
    sendToClient("port@"+QString::number(allocatedPort),nouveauClient.sock);
    nouveauClient.port=allocatedPort;
    allocatedPort++;

    clients << nouveauClient;
}

//SLOT : quand un message arrive, le récupère et le traite
void Server::donneesRecues()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (socket == 0)
        return;

    QDataStream in(socket);
    if (tailleMessage == 0)
    {
        if (socket->bytesAvailable() < (int)sizeof(quint16))
             return;
        in >> tailleMessage;
    }

    if (socket->bytesAvailable() < tailleMessage)
        return;
    QString message;
    in >> message;


     qDebug() << "Receiving message : " << message;
     tailleMessage = 0;
     processRequest(message,socket);
}

//FONCTION : réagit selon le message passé en paramètre
void Server::processRequest(const QString &message,QTcpSocket*socket)
{
    QStringList req = message.split(QRegExp("@"));
    int idClient = getIndexFromSocket(socket);

    if(req[0].compare("isSending")==0)
    {
        bool statePlaying=req[1].toInt();
        qDebug() << "SERVER pipeline will change because user streaming stopped";

        clients[idClient].isSending=statePlaying;
        setPipeline();
    }
    else if (req[0].compare("name")==0)
    {
        clients[idClient].name=req[1];
        setPipeline();
    }
    else if (req[0].compare("chat")==0)
    {
        sendToAll(message);
    }
}

//FONCTION : gestion de Gstreamer
void Server::setPipeline()
{
    int nbSender=0;
    QString toLaunch;

    if(pipeline!=NULL)
    {
       gst_element_set_state (pipeline, GST_STATE_NULL);
       gst_object_unref (pipeline);
    }


    for (int i = 0; i < clients.size(); i++)
    {
        if(clients[i].isSending)
        {
            nbSender++;
        }
    }

    if(nbSender>0)
    {
        int y=screen.height()/((int)nbSender);
        int x=screen.width()/((int)nbSender);

        toLaunch="videomixer name=mix";

        for (int i = 0; i < clients.size(); i++)
        {
            if(clients[i].isSending)
            {
                toLaunch+=" sink_" + QString::number(i);
                toLaunch+="::xpos=" + QString::number((i/(int)sqrt(nbSender))*x);
                toLaunch+=" sink_" + QString::number(i);
                toLaunch+="::ypos=" + QString::number((i%(int)sqrt(nbSender))*y);
            }
        }
        toLaunch+=" ! autovideosink sync=false";

        for (int i = 0; i < clients.size(); i++)
        {
            if(clients[i].isSending)
            {
                toLaunch+=" udpsrc port="+ QString::number(clients[i].port)+" caps = \"application/x-rtp, media=(string)video, "
                          "encoding-name=(string)VP8, payload=(int)96\" ! rtpvp8depay ! vp8dec ! videoscale "
                          "! video/x-raw , width="+ QString::number(x) +", height="+ QString::number(y) +" ! mix.sink_" + QString::number(i) ;
            }
        }
    }
    else
    {

        toLaunch="videotestsrc pattern=3 ! textoverlay font-desc=\"Sans 24\" "
                 "text=\"Connect to "+ serveur->serverAddress().toString() + ":"+ QString::number(serveur->serverPort()) + "\" shaded-background=true !  autovideosink";

                // "! video/x-raw , width="+ QString::number(screen.width()) +", height="+ QString::number(screen.height()) + "! autovideosink";
    }
    qDebug() << toLaunch;

    err = NULL;
    pipeline = gst_parse_launch(toLaunch.toUtf8(), &err);
    gst_element_set_state (pipeline, GST_STATE_PLAYING);
    sendToAllConnected("restartSending@"); // tous les client doivent recommencer leur envoi
}


//FONCTION : fabrique la paquet à envoyer
QByteArray Server::createPacket(const QString &message)
{
    // Préparation du paquet
    QByteArray paquet;
    QDataStream out(&paquet, QIODevice::WriteOnly);

    out << (quint16) 0; // On écrit 0 au début du paquet pour réserver la place pour écrire la taille
    out << message; // On ajoute le message à la suite
    out.device()->seek(0); // On se replace au début du paquet
    out << (quint16) (paquet.size() - sizeof(quint16)); // On écrase le 0 qu'on avait réservé par la longueur du message
    return paquet;
}

//FONCTION : Envoie à la socket le message
void Server::sendToClient(const QString &message,QTcpSocket*socket)
{
    socket->write(createPacket(message));
}

//FONCTION : envoie le message à tous les clients
void Server::sendToAll(const QString &message)
{
    QByteArray paquet=createPacket(message);

    for (int i = 0; i < clients.size(); i++)
    {
            clients[i].sock->write(paquet);
    }
}

//FONCTION : envoie le message à tous les clients
void Server::sendToAllConnected(const QString &message)
{
    QByteArray paquet=createPacket(message);

    for (int i = 0; i < clients.size(); i++)
    {
        if(clients[i].isSending)
        {
            clients[i].sock->write(paquet);
        }
    }
}


//SLOT : Lorsque la socket client est perdue supprime les données relatives au client change l'affichage
void Server::deconnexionClient()
{
    // On détermine quel client se déconnecte
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (socket == 0) // Si par hasard on n'a pas trouvé le client à l'origine du signal, on arrête la méthode
        return;

    qDebug() << clients[getIndexFromSocket(socket)].sock->peerAddress() << "is now disconnected";
    clients.removeAt(getIndexFromSocket(socket));

    socket->deleteLater();
    setPipeline();
}


//FONCTION : retourne l'indice du Client dont la socket est passé en paramètre
int Server::getIndexFromSocket(QTcpSocket*socket)
{
    for (int i = 0; i < clients.size(); i++)
    {
        if(clients[i].sock==socket)
        {
            return i;
        }
    }
    return -1;
}


