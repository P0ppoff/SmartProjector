#ifndef USER_H
#define USER_H

#include <QtNetwork>
#include <QString>
#include <QWidget>



class User
{

public:
    QTcpSocket * sock;
    int port;
    bool isSending;
    QString name;
    User(QTcpSocket * serv);
};

#endif // USER_H
