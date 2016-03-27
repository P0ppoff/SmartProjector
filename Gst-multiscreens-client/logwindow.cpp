#include "logwindow.h"
#include "ui_logwindow.h"



LogWindow::LogWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LogWindow)
{
    ui->setupUi(this);
}

LogWindow::~LogWindow()
{
    delete ui;
}

QPushButton* LogWindow::getButton()
{
    return ui->pushButton;
}

QString LogWindow::getIP()
{
    return ui->IP_text->text();
}

QString LogWindow::getPort()
{
    return ui->Port_text->text();
}

QString LogWindow::getName()
{
    return ui->Name_text->text();
}

