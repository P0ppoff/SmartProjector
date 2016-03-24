#ifndef LOGWINDOW_H
#define LOGWINDOW_H

#include <QWidget>
#include <QDebug>
#include <QPushButton>
#include <QString>

namespace Ui {
class LogWindow;
}

class LogWindow : public QWidget
{
    Q_OBJECT

public:
    explicit LogWindow(QWidget *parent = 0);
    ~LogWindow();
    QPushButton* getButton();
    QString getIP();
    QString getName();
    QString getPort();

private:
    Ui::LogWindow *ui;
};

#endif // LOGWINDOW_H
