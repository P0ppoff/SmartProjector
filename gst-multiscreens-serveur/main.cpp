#include "server.h"

#include <QApplication>
#include <QWidget>



int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    Server server;
    return app.exec();
}

