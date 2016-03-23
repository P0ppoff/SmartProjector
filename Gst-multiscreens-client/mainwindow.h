#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QDebug>
#include <QPushButton>
#include <QString>

namespace Ui {
class MainWindow;
}

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QPushButton* getButton();
    QString getCommand();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
