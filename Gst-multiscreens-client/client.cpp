#include "client.h"
#include "math.h"
extern "C"
{
#include "wmctrl.h"
}

#define __RPI_SERVER__

Client::Client()
{
    //On déclare les différentes fenêtres à afficher au cours du programme
    w1 = new LogWindow();
    w2 = new MainWindow();

    _stack = new QStackedWidget();
    _stack->setWindowTitle("SmartProjector");

    //LogWindow
    connect(w1->getButton(),SIGNAL(clicked()),this,SLOT(connecte()));
    _stack->addWidget(w1);
    _stack[0].resize(600,500);//On initialise la taille de la première fenêtre, qui donne la taille des autres

    //MainWindow
    connect(w2->getSendBox(),SIGNAL(toggled(bool)),this,SLOT(sendCastingValue(bool)));
    connect(w2->getPushChat(),SIGNAL(clicked()),this,SLOT(sendChat()));
#ifdef __linux__
    connect(w2->getPushRefresh(),SIGNAL(clicked()),this,SLOT(clickSendWindows()));
#else
    w2->getPushRefresh()->hide();
#endif
    _stack->addWidget(w2);
    _stack->show();
    pipeline=NULL;
    isSending=true;
    tailleMessage = 0;
    screen = QApplication::desktop()->screenGeometry();

}

//SLOT : quand le boutton "send" est enclenché
void Client::sendChat()
{
    sendMessage("chat@" + userName + " : "+ w2->getLineChat());
}

//SLOT : quand le boutton "connexion" est enclenché
void Client::connecte()
{
    socket = new QTcpSocket(this);
    socket->abort();
    socket->connectToHost(w1->getIP(), w1->getPort().toInt());
    connect(socket, SIGNAL(readyRead()), this, SLOT(receiveMessage()));
    connect(socket, SIGNAL(connected()), this, SLOT(connexionSuccess()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(connexionLost()));
    qDebug() <<"Trying to connect ...";
}

//SLOT : quand la connexion est perdue
void Client::connexionLost()
{
    socket->abort();
    if(pipeline!=NULL)
    {
        gst_element_set_state (pipeline, GST_STATE_NULL);
        _stack->setCurrentIndex(_stack->currentIndex()-1);
    }
    qDebug() <<"Connexion with server lost...";
}

//SLOT : quand le boutton "Refresh/Send screen" est enclenché
void Client::clickSendWindows()
{
    QStringList toSend =  w2->getWindowsChecked();
    char *temp=getWinList();
    QString res =QString::fromLatin1(temp,strlen(temp));
    w2->updateListeWindows(res,toSend);
    free(temp);
    sendWindowsForLinux(toSend);
}

//SLOT : lorsque la connexion est établie, change l'affichage et communique avec le serveur
void Client::connexionSuccess()
{
    qDebug() <<"Client connected";
    userName=w1->getName();
    sendMessage("name@" + userName +"@"+w1->getPassword());

#ifdef __linux__
    //Récupère la liste des fenêtres existantes
    char *temp=getWinList();
    QString res =QString::fromLatin1(temp,strlen(temp));
    w2->updateListeWindows(res,QStringList());
    free(temp);
#endif
}

//FONCTIONS : envoie le flux video via Gstreamer
void Client::sendWindowsForLinux(QStringList toSend)
{
    int nbWindows = toSend.count();
    if(nbWindows>0)
    {
        QString toLaunch="";
        if(pipeline!=NULL)
        {
            gst_element_set_state (pipeline, GST_STATE_NULL);
            gst_object_unref (pipeline);
        }

        toLaunch="videomixer name=mix";

        int taille_grid; // nombre de ligne/colonne de la grille
        taille_grid = sqrt(nbWindows); // donne le nombre entier dont la racine carré inférieur est le plus proche
        if (pow(taille_grid, 2) != nbWindows) taille_grid++;

        int y=screen.height()/(taille_grid);
        int x=screen.width()/(taille_grid);

        int nb_client = 0;
        for (int i = 0; i < nbWindows; i++)
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

#ifndef __RPI_SERVER__
        toLaunch+=" ! vp8enc deadline=1 threads=4 target-bitrate=256000 ! queue ! rtpvp8pay ! udpsink host="+ w1->getIP() +" port="+QString::number(port);
#else
        toLaunch+=" ! queue ! x264enc bitrate=2048000 threads=4 pass=pass1 tune=zerolatency speed-preset=veryfast intra-refresh=true "
                  "! queue ! rtph264pay pt=96 ! udpsink host="+ w1->getIP() +" port="+QString::number(port);
#endif

        for (int i = 0; i < nbWindows; i++)
        {
#ifndef __RPI_SERVER__
            toLaunch+=" ximagesrc xid="+toSend.at(i) +" use-damage=false ! queue ! videoconvert ! queue ! videoscale ! queue "
                                                      "! video/x-raw , width="+ QString::number(x) +", height="+ QString::number(y) +" ! mix.sink_" + QString::number(i) ;
#else
            toLaunch+=" ximagesrc xid="+toSend.at(i) +" use-damage=false ! queue ! videoconvert ! queue ! videoscale ! queue "
                                                      "! video/x-raw, framerate=30/1, format=I420, force-aspect-ratio=true ! mix.";
#endif
        }

        qDebug() << toLaunch;

        err = NULL;
        pipeline = gst_parse_launch(toLaunch.toUtf8(), &err);
        gst_element_set_state (pipeline, GST_STATE_PLAYING);
    }
    else
        sendScreen();
}


//FONCTIONS : envoie le flux video via Gstreamer
void Client::sendScreen()
{
    if(pipeline!=NULL)
    {
        gst_element_set_state (pipeline, GST_STATE_NULL);
        gst_object_unref (pipeline);
    }
#ifndef __RPI_SERVER__
    QString toLaunch="ximagesrc use-damage=false ! queue ! videoconvert ! queue ! videoscale ! queue ! "
                     "video/x-raw, width="+ QString::number(screen.width())+", height="+ QString::number(screen.height())+", framerate=30/1 "
                                                                                                                          "! vp8enc deadline=1 threads=4 target-bitrate=256000 ! queue ! rtpvp8pay ! udpsink host="+ w1->getIP() +" port="+QString::number(port);
#else
    QString toLaunch="ximagesrc use-damage=false ! queue ! videoconvert ! videoscale "
                     "! video/x-raw, framerate=30/1, format=I420, force-aspect-ratio=true "
                     "! queue ! x264enc pass=pass1 tune=zerolatency speed-preset=veryfast intra-refresh=true "
                     "! queue ! rtph264pay pt=96 ! udpsink host="+ w1->getIP() +" port="+QString::number(port);
#endif

    qDebug() << toLaunch;

    err = NULL;
    pipeline = gst_parse_launch(toLaunch.toUtf8(), &err);
    gst_element_set_state (pipeline, GST_STATE_PLAYING);
}

//SLOT : quand la valeur de la checkbox est modifée
void Client::sendCastingValue(bool b)
{
    QString toSend = "isSending@" + QString::number(b);

    if(b)
    {
        qDebug() << "Client restarted to send his screen";
    }
    else
    {
        qDebug() << "Client stopped to send his screen";
        gst_element_set_state (pipeline, GST_STATE_PAUSED);
        gst_object_unref (pipeline);
        pipeline=NULL;
    }
    isSending=b;
    sendMessage(toSend);

}

//FONCTION : Envoie le message passé en paramètre au serveur
void Client::sendMessage(const QString &message)
{
    QByteArray paquet;
    QDataStream out(&paquet, QIODevice::WriteOnly);

    QString messageAEnvoyer = message;

    out << (quint16) 0;
    out << messageAEnvoyer;
    out.device()->seek(0);
    out << (quint16) (paquet.size() - sizeof(quint16));

    socket->write(paquet); // On envoie le paquet
    qDebug() <<"Sending message: "<< message;
}

//SLOT : Réception de données + traitement
void Client::receiveMessage()
{
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
        processRequest(messageRecu);

        // reset for the next message
        tailleMessage = 0;
    }
}


//Potentiellement utile pour plus tard, ne pas supprimer
void Client::processRequest(const QString &message)
{
    QStringList req= message.split(QRegExp("@"));

    if(req[0].compare("port")==0)//Quand on lui donne son port, le client est prêt à envoyer
    {
        port=req[1].toInt();
        //Une fois que le serveur nous a accepté on peut changer le menu
        _stack->setCurrentIndex((_stack->currentIndex()+1)%_stack->count());
        tailleMessage = 0;
    }
    else if(req[0].compare("restartSending")==0 && port!=-1) // Quand il reçoit l'ordre d'envoyer, il envoie
    {
        nbClientsSending=req[1].toInt();
        nbClientsSending==1?w2->getPushRefresh()->show():w2->getPushRefresh()->hide();
        sendScreen();
    }
    else if(req[0].compare("chat")==0) // On ajoute le message reçu à la liste
    {
        w2->getListMessageChat()->append(req[1]);
    }
    else if(req[0].compare("isTeacher")==0)
    {
        isTeacher=(req[1].toInt() ==1);
        if(!isTeacher){w2->removeTeacherTab();}
    }
}
