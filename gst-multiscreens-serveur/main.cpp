#include "server.h"
#include <gst/gst.h>
#include <gst/video/videooverlay.h>

#include <QApplication>
#include <QWidget>



int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    gst_init (&argc, &argv);
    Server server;
    return app.exec();
}

