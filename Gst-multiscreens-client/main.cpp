#include "client.h"

#include <QApplication>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    gst_init (&argc, &argv);
    Client client;
    return app.exec();
}
