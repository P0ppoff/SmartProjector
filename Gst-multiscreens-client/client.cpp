#include "client.h"

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
    _stack[0].resize(300,400);//On initialise la taille de la première fenêtre, qui donne la taille des autres

    //MainWindow
    connect(w2->getSendBox(),SIGNAL(toggled(bool)),this,SLOT(sendCastingValue(bool)));
    _stack->addWidget(w2);
    _stack->show();

    tailleMessage = 0;
}

//SLOT : quand le boutton "connexion" est enclenché
void Client::connecte()
{
    socket = new QTcpSocket(this);
    socket->abort();
    socket->connectToHost(w1->getIP(), w1->getPort().toInt());
    connect(socket, SIGNAL(readyRead()), this, SLOT(donneesRecues()));
    connect(socket, SIGNAL(connected()), this, SLOT(connexionSuccess()));
    //connect(w1, SIGNAL(), this, SLOT(lol()));
    qDebug() <<"Trying to connect ...";
}

void Client::lol()
{
    qDebug() <<"YOLO";
}

//SLOT : lorsque la connexion est établie, change l'affichage et communique avec le serveur
void Client::connexionSuccess()
{
    _stack->setCurrentIndex((_stack->currentIndex()+1)%_stack->count());
    qDebug() <<"Client connected";
    EnvoyerMessage("name@" + w1->getName());
}

//FONCTIONS : envoie le flux video via Gstreamer
void Client::sendScreen()
{
    QString toLaunch="ximagesrc ! videoconvert ! videoscale "
                     "! video/x-raw, width=900, height=900, framerate=30/1 "
                     "! textoverlay font-desc=\"Sans 24\" text=" + w1->getName() +" shaded-background=true "
                     "! vp8enc deadline=1 ! rtpvp8pay ! udpsink host=127.0.0.1 port="+QString::number(port);
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
        sendScreen();
        qDebug() << "Client stopped to send his screen";
    }
    else
    {
        gst_element_set_state (pipeline, GST_STATE_NULL);
        qDebug() << "Client restarted to send his screen";
    }
    EnvoyerMessage(toSend);
}

//FONCTION : Envoie le message passé en paramètre au serveur
void Client::EnvoyerMessage(const QString &message)
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
void Client::donneesRecues()
{
    QDataStream in(socket);

    if (tailleMessage == 0) // récupère la taille du message
    {
        if (socket->bytesAvailable() < (int)sizeof(quint16))
             return;
        in >> tailleMessage;
    }
    if (socket->bytesAvailable() < tailleMessage)
        return;

    //récupère le message
    QString messageRecu;
    in >> messageRecu;

    tailleMessage = 0;

    qDebug() <<"Received message: "<< messageRecu;
    processRequest(messageRecu);
}


//Potentiellement utile pour plus tard, ne pas supprimer
void Client::processRequest(const QString &message)
{
    QStringList req= message.split(QRegExp("@"));

   if(req[0].compare("port")==0)
    {
       port=req[1].toInt();
       sendScreen();
    }
}
