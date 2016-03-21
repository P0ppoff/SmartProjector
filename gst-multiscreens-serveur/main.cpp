#include "server.h"
#include <gst/gst.h>
#include <gst/video/videooverlay.h>

#include <QApplication>

#include <QWidget>



int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    Server server;

    gst_init (&argc, &argv);






    return app.exec();
}

