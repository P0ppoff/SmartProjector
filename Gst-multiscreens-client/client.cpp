#include "client.h"
#include <QHostAddress>
#include "mainwindow.h"

Client::Client(QObject* parent): QObject(parent)
{
}

Client::~Client()
{
  client.close();
}


void Client::start(QString address, quint16 port)
{
  QHostAddress addr(address);
  client.connectToHost(addr, port);
}

void Client::startTransfer()
{
  client.write("Hello, world", 13);
qDebug() << "GROS LOL";
}
