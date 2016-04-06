#include "server.h"
#include <iostream>
#include <QDataStream>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QApplication>

#include <gst/gst.h>
#include <gst/video/videooverlay.h>

//#define __RPI_SERVER__

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

    //on récupère l'ip du serveur pour l'afficher
    foreach (const QHostAddress &address, QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost))
            localIp=address.toString();
    }
    //on initialise les variables propres au serveur et on lance l'affichage
    tailleMessage = 0;
    pipeline = NULL;
    allocatedPort = 5000;
    setPipeline();
}

//SLOT : quand une nouvelle connexion est établie, ajoute un Client à la liste et le connecte
void Server::nouvelleConnexion()
{
    User nouveauClient(serveur->nextPendingConnection()) ;
    //Un client se connecte, on l'ajoute
    qDebug() << "Client connected : " << nouveauClient.sock->peerAddress();
    connect(nouveauClient.sock, SIGNAL(readyRead()), this, SLOT(receiveMessage()));
    connect(nouveauClient.sock, SIGNAL(disconnected()), this, SLOT(clientDisconnected()));
    nouveauClient.port=-1;
    clients << nouveauClient;
}



//SLOT : quand un message arrive, le récupère et le traite
void Server::receiveMessage()
{

    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (socket == 0)
        return;

    QDataStream in(socket);

    while(1) {

        if (tailleMessage == 0) {
            if (socket->bytesAvailable() < (int)sizeof(tailleMessage))
                return;

            in >> tailleMessage;
        }

        if (socket->bytesAvailable() < tailleMessage)
            return;

        //récupère le message
        QString messageRecu;
        in >> messageRecu;


        qDebug() <<"Received message: " << messageRecu;
        processRequest(messageRecu,socket);

        // reset for the next message
        tailleMessage = 0;
    }
}

//
int Server::verifyDataBase(QString login,QString pwd)
{
    QFile file(":/database");
    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(0, "error", file.errorString());
    }

    QTextStream in(&file);

    while(!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split("@");
        if(fields[0]==login && fields[1]==pwd)
        {
            file.close();
            return fields[2].toInt();
        }
    }

    file.close();

    return -1;
}

bool Server::alreadyConnected(QString name){
    if(name.compare("mathias")==0) // cet utilisateur se connecte autant qu'il veut, que je puisse tester
    {
        return false;
    }
    for (int i = 0; i < clients.size(); i++)
    {
        if(clients[i].name==name)
        {
            return true;
        }
    }
    return false;
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
        int databaseResult = verifyDataBase(req[1],req[2]);
        if(databaseResult==-1 || alreadyConnected(req[1]))
        {
            //si le client n'est pas dans la base, on le kick
            qDebug() << "SERVER kicked " + req[1];
            clients[idClient].sock->abort();
        }
        else
        {
            clients[idClient].name=req[1];
            sendToClient("port@"+QString::number(allocatedPort),clients[idClient].sock);
            clients[idClient].port=allocatedPort;
            clients[idClient].isTeacher=databaseResult;
            clients[idClient].isSending=true;
            allocatedPort++;
            sendToClient("isTeacher@"+QString::number(databaseResult),clients[idClient].sock);
            setPipeline();
        }
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
        toLaunch="videomixer name=mix";

        int taille_grid; // nombre de ligne/colonne de la grille
        taille_grid = sqrt(nbSender); // donne le nombre entier dont la racine carré inférieur est le plus proche
        if (pow(taille_grid, 2) != nbSender) taille_grid++;

        int y=screen.height()/(taille_grid);
        int x=screen.width()/(taille_grid);


        int nb_client = 0;
        for (int i = 0; i < clients.size(); i++)
        {
            if(clients[i].isSending)
            {
                toLaunch+=" sink_" + QString::number(i);
                toLaunch+="::xpos=" + QString::number((screen.width() / taille_grid) * (nb_client % taille_grid));
                toLaunch+=" sink_" + QString::number(i);
                if(nb_client == 0){
                    toLaunch+="::ypos=0";
                }else{
                    toLaunch+="::ypos=" + QString::number((screen.height() / taille_grid) * ((int)(nb_client / taille_grid)));
                }
                nb_client++;
            }
        }

        
#ifndef __RPI_SERVER__
        toLaunch+=" ! autovideosink sync=false";
#else
        toLaunch+=" ! videoconvert ! ximagesink sync=false";
#endif
        for (int i = 0; i < clients.size(); i++)
        {
            if(clients[i].isSending)
            {
#ifndef __RPI_SERVER__
                toLaunch+=" udpsrc port="+ QString::number(clients[i].port)+" caps = \"application/x-rtp, media=(string)video, "
                                                                            "encoding-name=(string)VP8, payload=(int)96, framerate=(fraction)30/1\" ! rtpvp8depay ! vp8dec ! videoscale "
                                                                            "! video/x-raw , width="+ QString::number(x) +", height="+ QString::number(y) +" ! mix.";
#else
                toLaunch+=" udpsrc port="+ QString::number(clients[i].port)+" caps=\"application/x-rtp, media=video, clock-rate=90000"
                                                                            ", encoding-name=H264, framerate=30/1\" ! rtph264depay ! queue ! h264parse ! queue ! omxh264dec ! queue ! videoscale ! video/x-raw "
                                                                            ", width="+ QString::number(x) +", height="+ QString::number(y) +" ! mix.";
#endif
            }
        }
    }
    else
    {
#ifndef __RPI_SERVER__
        toLaunch="videotestsrc pattern=3 ! textoverlay font-desc=\"Sans 24\" "
                 "text=\"Connect to "+  localIp + ":"+ QString::number(serveur->serverPort()) + "\" shaded-background=true "
                                                                                                "! videoconvert ! video/x-raw , width="+ QString::number(screen.width()) +", height="+ QString::number(screen.height()) + " ! autovideosink";
#else
        toLaunch="videotestsrc pattern=3 ! textoverlay font-desc=\"Sans 24\" "
                 "text=\"Connect to "+  localIp + ":"+ QString::number(serveur->serverPort()) + "\" shaded-background=true "
                                                                                                "! videoconvert ! video/x-raw , width="+ QString::number(screen.width()) +", height="+ QString::number(screen.height()) + " ! ximagesink sync=false";
#endif

    }
    qDebug() << toLaunch;

    err = NULL;
    pipeline = gst_parse_launch(toLaunch.toUtf8(), &err);
    gst_element_set_state (pipeline, GST_STATE_PLAYING);
    if(nbSender>0)
    {sendToAllConnected("restartSending@"+ QString::number(nbSender) );} // tous les client doivent recommencer leur envoi
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
    qDebug() << "SENDING " << message;
}

//FONCTION : envoie le message à tous les clients
void Server::sendToAll(const QString &message)
{
    QByteArray paquet=createPacket(message);

    for (int i = 0; i < clients.size(); i++)
    {
        clients[i].sock->write(paquet);
    }
    qDebug() << "SENDING TO ALL " << message;
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
    qDebug() << "SENDING TO ALL CONNECTED" << message;
}


//SLOT : Lorsque la socket client est perdue supprime les données relatives au client change l'affichage
void Server::clientDisconnected()
{
    // On détermine quel client se déconnecte
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (socket == 0) // Si par hasard on n'a pas trouvé le client à l'origine du signal, on arrête la méthode
        return;

    qDebug() << clients[getIndexFromSocket(socket)].sock->peerAddress() << "is now disconnected";
    int num=getIndexFromSocket(socket);
    int removedClientPort=clients[num].port;
    clients.removeAt(num);

    socket->deleteLater();
    if(removedClientPort!=-1)
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


