#include "client.h"
#include "mainwindow.h"

#include <QApplication>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    qDebug() << "Client Started";
    Client client;
    client.start("127.0.0.1", 50885);
    return app.exec();
}
