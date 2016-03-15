#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QUdpSocket>

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = 0);
    void HelloUDP();
signals:

public slots:
    void readyRead();

private:
    QUdpSocket *socket;

};

#endif // SERVER_H
