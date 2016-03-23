#include "client.h"


Client::Client()
{
 /*   QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(connecte()));
    timer->start(1000);*/

   w1 = new LogWindow();
   w2 = new MainWindow();

    _stack = new QStackedWidget();
    connect(w1->getButton(),SIGNAL(clicked()),this,SLOT(connecte()));
    _stack->addWidget(w1);

      connect(w2->getButton(),SIGNAL(clicked()),this,SLOT(sendCommand()));
    _stack->addWidget(w2);

    _stack->show();

    tailleMessage = 0;
}

void Client::connecte()
{
    QString ip=w1->getIP();
    int port=w1->getPort().toInt();

    socket = new QTcpSocket(this);
    socket->connectToHost(ip, 50885);
    connect(socket, SIGNAL(readyRead()), this, SLOT(donneesRecues()));
    //connect(socket, SIGNAL(connected()), this, SLOT(connecte()));

    _stack->setCurrentIndex((_stack->currentIndex()+1)%_stack->count());
    qDebug() <<"Client connected to " << ip << ":" << port;
}

void Client::sendCommand()
{
    EnvoyerMessage(w2->getCommand());
}


void Client::EnvoyerMessage(const QString &message)
{
    QByteArray paquet;
    QDataStream out(&paquet, QIODevice::WriteOnly);

    // On pr�pare le paquet � envoyer
    QString messageAEnvoyer = message;

    out << (quint16) 0;
    out << messageAEnvoyer;
    out.device()->seek(0);
    out << (quint16) (paquet.size() - sizeof(quint16));

    socket->write(paquet); // On envoie le paquet
    qDebug() <<"Sending message: "<< message;
}

// On a reçu un paquet (ou un sous-paquet)
void Client::donneesRecues()
{
    /* Même principe que lorsque le serveur reçoit un paquet :
    On essaie de récupérer la taille du message
    Une fois qu'on l'a, on attend d'avoir reçu le message entier (en se basant sur la taille annoncée tailleMessage)
    */
    QDataStream in(socket);

    if (tailleMessage == 0)
    {
        if (socket->bytesAvailable() < (int)sizeof(quint16))
             return;

        in >> tailleMessage;
    }

    if (socket->bytesAvailable() < tailleMessage)
        return;


    // Si on arrive jusqu'à cette ligne, on peut récupérer le message entier
    QString messageRecu;
    in >> messageRecu;

    // On affiche le message sur la zone de Chat
   // listeMessages->append(messageRecu);

    // On remet la taille du message à 0 pour pouvoir recevoir de futurs messages
    tailleMessage = 0;
    qDebug() <<"Received message: "<< messageRecu;
}


