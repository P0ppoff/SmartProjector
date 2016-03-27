#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QDebug>
#include <QPushButton>
#include <QString>
#include <QCheckBox>
#include <QTextEdit>

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
    QString getLineChat();
    QTextEdit* getListMessageChat();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H

