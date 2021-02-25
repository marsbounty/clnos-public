#ifndef COMPLIMENTUI_H
#define COMPLIMENTUI_H

#include <QDialog>

namespace Ui {
class ComplimentUI;
}

class PlayerController;
class ComplimentUI : public QDialog
{
    Q_OBJECT

public:
    explicit ComplimentUI(PlayerController *pCon, QWidget *parent = nullptr);
    ~ComplimentUI();

private slots:
    void on_compliment_clicked();

private:
    Ui::ComplimentUI *ui;
    PlayerController *pCon;
    QList<QString> names;
    QList<int> ids;

    void fillPlayerLists();
};

#endif // COMPLIMENTUI_H
