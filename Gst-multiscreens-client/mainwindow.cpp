#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QCheckBox* MainWindow::getSendBox()
{
    return ui->checkBox;
}


QPushButton* MainWindow::getPushChat()
{
    return ui->pushChat;
}

QString MainWindow::getLineChat()
{
    return ui->lineChat->text();
}

QTextEdit* MainWindow::getListMessageChat()
{
    return ui->textChat;
}


