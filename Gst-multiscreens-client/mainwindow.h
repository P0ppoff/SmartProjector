#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QDebug>
#include <QPushButton>
#include <QString>
#include <QCheckBox>
#include <QTextEdit>
#include <QTreeWidgetItem>

namespace Ui {
class MainWindow;
}

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QCheckBox* getSendBox();
    QPushButton* getPushChat();
    QPushButton* getPushRefresh();
    QString getLineChat();
    QTextEdit* getListMessageChat();
    void setTableScreen(QString entries,QStringList alreadyChecked);
    QStringList getWindowsChecked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H

