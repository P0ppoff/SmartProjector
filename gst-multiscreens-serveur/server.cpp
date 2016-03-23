#include "server.h"
#include <iostream>
#include <QDataStream>

#include <gst/gst.h>
#include <gst/video/videooverlay.h>


Server::Server()
{
    // Création et disposition des widgets de la fenêtre

   // connect(boutonQuitter, SIGNAL(clicked()), qApp, SLOT(quit()));

    // Gestion du serveur
    serveur = new QTcpServer(this);
    if (!serveur->listen(QHostAddress::Any, 50885)) // Démarrage du serveur sur toutes les IP disponibles et sur le port 50585
    {
        // Si le serveur n'a pas été démarré correctement
        qDebug() << "Server failed to start";
    }
    else
    {
       // Si le serveur a été démarré correctement
       qDebug() << "Server Started";
       connect(serveur, SIGNAL(newConnection()), this, SLOT(nouvelleConnexion()));
    }
    tailleMessage = 0;
}

void Server::nouvelleConnexion()
{
    envoyerATous(tr("<em>Un nouveau client vient de se connecter</em>"));

    QTcpSocket *nouveauClient = serveur->nextPendingConnection();
    clients << nouveauClient;

    qDebug() << "Client connected : " << nouveauClient->peerAddress();

    connect(nouveauClient, SIGNAL(readyRead()), this, SLOT(donneesRecues()));
    connect(nouveauClient, SIGNAL(disconnected()), this, SLOT(deconnexionClient()));
}



void Server::donneesRecues()
{
    // 1 : on reçoit un paquet (ou un sous-paquet) d'un des clients
    // On détermine quel client envoie le message (recherche du QTcpSocket du client)
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (socket == 0) // Si par hasard on n'a pas trouvé le client à l'origine du signal, on arrête la méthode
        return;
    // Si tout va bien, on continue : on récupère le message
    QDataStream in(socket);
    if (tailleMessage == 0) // Si on ne connaît pas encore la taille du message, on essaie de la récupérer
    {
        if (socket->bytesAvailable() < (int)sizeof(quint16)) // On n'a pas reçu la taille du message en entier
             return;
        in >> tailleMessage; // Si on a reçu la taille du message en entier, on la récupère
    }

    // Si on connaît la taille du message, on vérifie si on a reçu le message en entier
    if (socket->bytesAvailable() < tailleMessage) // Si on n'a pas encore tout reçu, on arrête la méthode
        return;
    // Si ces lignes s'exécutent, c'est qu'on a reçu tout le message : on peut le récupérer !
    QString message;
    in >> message;
    // 2 : on renvoie le message à tous les clients


     qDebug() << "Receiving message : " << message;

    envoyerATous(message);
    // 3 : remise de la taille du message à 0 pour permettre la réception des futurs messages
    tailleMessage = 0;




    GError* err = NULL;
    //GstElement *pipeline = gst_pipeline_new ("xvoverlay");
    GstElement *pipeline = gst_parse_launch(message.toUtf8(), &err);
    gst_element_set_state (pipeline, GST_STATE_PLAYING);


}

void Server::envoyerATous(const QString &message)
{
    // Préparation du paquet
    QByteArray paquet;
    QDataStream out(&paquet, QIODevice::WriteOnly);

    out << (quint16) 0; // On écrit 0 au début du paquet pour réserver la place pour écrire la taille
    out << message; // On ajoute le message à la suite
    out.device()->seek(0); // On se replace au début du paquet
    out << (quint16) (paquet.size() - sizeof(quint16)); // On écrase le 0 qu'on avait réservé par la longueur du message

    // Envoi du paquet préparé à tous les clients connectés au serveur
    for (int i = 0; i < clients.size(); i++)
    {
        QByteArray paquet;
        clients[i]->write(paquet);
    }
}


void Server::deconnexionClient()
{
    envoyerATous(tr("<em>Un client vient de se déconnecter</em>"));

    // On détermine quel client se déconnecte
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (socket == 0) // Si par hasard on n'a pas trouvé le client à l'origine du signal, on arrête la méthode
        return;

    clients.removeOne(socket);

    socket->deleteLater();
}

