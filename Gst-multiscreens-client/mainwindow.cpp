#include "mainwindow.h"
#include "client.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;

}

void MainWindow::on_pushButton_clicked()
{

   connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(Client::startTransfer()));
   ui->lineEdit->setText("yolo");
}
