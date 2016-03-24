#include "user.h"

User::User(QTcpSocket * serv)
{
    this->sock=serv;
    this->name="Unamed";
    this->isSending=true;
}
