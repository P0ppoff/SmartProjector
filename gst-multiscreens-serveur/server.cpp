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
}

//SLOT : quand une nouvelle connexion est établie, ajoute un Client à la liste et le connecte
void Server::nouvelleConnexion()
{  
    User nouveauClient(serveur->nextPendingConnection()) ;
    clients << nouveauClient;

    qDebug() << "Client connected : " << nouveauClient.sock->peerAddress();
    connect(nouveauClient.sock, SIGNAL(readyRead()), this, SLOT(donneesRecues()));
    connect(nouveauClient.sock, SIGNAL(disconnected()), this, SLOT(deconnexionClient()));
    sendToClient("Serveur : Connexion effectuée",nouveauClient.sock);
    //setPipeline();
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

     processRequest(message,socket);
     tailleMessage = 0;

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
        //sendToClient("isSendingOK@" + req[1] ,socket);
    }
    else if (req[0].compare("name")==0)
    {
        clients[idClient].name=req[1];
        setPipeline();
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
        //screen.height();
        //screen.width();

        toLaunch="videomixer name=mix ";
        for (int i = 0; i < clients.size(); i++)
        {
            if(clients[i].isSending)
            {
                toLaunch+="sink_" + QString::number(i);
                toLaunch+="::xpos=" + QString::number(i*300);
                toLaunch+=" sink_" + QString::number(i) ;
                toLaunch+="::ypos=" + QString::number((nbSender-i)*300);
            }
        }
        toLaunch+=" ! autovideosink sync=false ";

        for (int i = 0; i < clients.size(); i++)
        {
            if(clients[i].isSending)
            {
                toLaunch+="udpsrc port=5100 caps = \"application/x-rtp, media=(string)video, clock-rate=(int)90000, "
                          "encoding-name=(string)VP8, payload=(int)96, ssrc=(uint)2432391931"
                          ", timestamp-offset=(uint)1048101710, seqnum-offset=(uint)9758\" ! rtpjitterbuffer latency=0 ! rtpvp8depay "
                          "! vp8dec  ! videoscale "
                          "! video/x-raw ,width=600,height=300 ! mix.sink_" + QString::number(i) ;
            }
        }
        qDebug() << toLaunch;
        err = NULL;
        pipeline = gst_parse_launch(toLaunch.toUtf8(), &err);
        gst_element_set_state (pipeline, GST_STATE_PLAYING);
    }


}

//FONCTION : Envoie à la socket le message
void Server::sendToClient(const QString &message,QTcpSocket*socket)
{
    // Préparation du paquet
    QByteArray paquet;
    QDataStream out(&paquet, QIODevice::WriteOnly);

    out << (quint16) 0; // On écrit 0 au début du paquet pour réserver la place pour écrire la taille
    out << message; // On ajoute le message à la suite
    out.device()->seek(0); // On se replace au début du paquet
    out << (quint16) (paquet.size() - sizeof(quint16)); // On écrase le 0 qu'on avait réservé par la longueur du message

    socket->write(paquet);
}




//FONCTION : potentiellement utile pour plus tard
void Server::envoyerATous(const QString &message)
{
    QByteArray paquet;
    QDataStream out(&paquet, QIODevice::WriteOnly);

    out << (quint16) 0; // On écrit 0 au début du paquet pour réserver la place pour écrire la taille
    out << message; // On ajoute le message à la suite
    out.device()->seek(0); // On se replace au début du paquet
    out << (quint16) (paquet.size() - sizeof(quint16)); // On écrase le 0 qu'on avait réservé par la longueur du message

    for (int i = 0; i < clients.size(); i++)
    {
        clients[i].sock->write(paquet);
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


