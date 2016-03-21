#include "server.h"

#include <gst/gst.h>
#include <gst/video/videooverlay.h>
#include <iostream>

using namespace std;

Server::Server(QObject* parent): QObject(parent)
{
  connect(&server, SIGNAL(newConnection()),this, SLOT(acceptConnection()));

  server.listen(QHostAddress::Any, 8888);
}

Server::~Server()
{
  server.close();
}

void Server::acceptConnection()
{
  client = server.nextPendingConnection();

  connect(client, SIGNAL(readyRead()),this, SLOT(startRead()));
}

void Server::startRead()
{
  char buffer[1024] = {0};
  client->read(buffer, client->bytesAvailable());
  qDebug() << buffer;

  GError* err = NULL;
  //GstElement *pipeline = gst_pipeline_new ("xvoverlay");
  GstElement *pipeline = gst_parse_launch(buffer, &err);
  gst_element_set_state (pipeline, GST_STATE_PLAYING);

  client->close();
}
