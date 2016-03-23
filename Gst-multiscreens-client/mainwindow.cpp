#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

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

QPushButton* MainWindow::getButton()
{
    return ui->pushButton;
}

QString MainWindow::getCommand()
{
    return  ui->textEdit->toPlainText();
}


