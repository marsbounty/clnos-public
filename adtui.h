#ifndef ADTUI_H
#define ADTUI_H

#include <QDialog>
#include <player.h>

namespace Ui {
class adtui;
}

class adtui : public QDialog
{
    Q_OBJECT

public:
    explicit adtui(QList<Player *> players,QWidget *parent = nullptr);
    ~adtui();

private:
    Ui::adtui *ui;
    QList<Player *> players;
    void loadPlayerlist();
};

#endif // ADTUI_H
