#ifndef PLAYER_H
#define PLAYER_H

#include <QMainWindow>

class Player : public QMainWindow
{
    Q_OBJECT

public:
    Player(QWidget *parent = 0);
    ~Player();
};

#endif // PLAYER_H
