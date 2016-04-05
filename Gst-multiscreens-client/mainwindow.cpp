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

QPushButton* MainWindow::getPushRefresh()
{
    return ui->pushRefresh;
}

QString MainWindow::getLineChat()
{
    return ui->lineChat->text();
}

QTextEdit* MainWindow::getListMessageChat()
{
    return ui->textChat;
}

void MainWindow::removeTeacherTab()
{
    ui->tabWidget->removeTab(2);
}


//FONCTION : actualise la liste des fenêtres actives sur le pc
void MainWindow::updateListeWindows(QString entries,QStringList alreadyChecked)
{
    ui->table->clear();
    QStringList lines= entries.split(QRegExp("\n"));
    foreach( QString line, lines ) {
        if (!line.trimmed().isEmpty()) // S'il n'y a pas de blanc sur la ligne
        {
            QString xid=line.split("  ").at(0);//réupère Xid
            QString name=line.mid(xid.length()+2);//récupère le nom de la fenêtre
            QTreeWidgetItem * newItem = new QTreeWidgetItem();

            newItem->setCheckState(0,alreadyChecked.contains(xid)?
                                       Qt::Checked:
                                       Qt::Unchecked);
            newItem->setText(0,xid);
            newItem->setText(1,name);
            newItem->setFlags(newItem->flags() | Qt::ItemIsUserCheckable);
            ui->table->addTopLevelItem(newItem);
        }
    }

    ui->table->resizeColumnToContents(0);
}


QStringList MainWindow::getWindowsChecked()
{
    QStringList toReturn;
    for(int i=0;i < ui->table->topLevelItemCount();i++) {
        if(ui->table->topLevelItem(i)->checkState(0)){
            toReturn << ui->table->topLevelItem(i)->text(0);
        }
    }
    return toReturn;
}


