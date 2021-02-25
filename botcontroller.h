#ifndef BOTCONTROLLER_H
#define BOTCONTROLLER_H

#include <QDialog>

namespace Ui {
class BotController;
}
class PlayerController;

class BotController : public QDialog
{
    Q_OBJECT

public:
    explicit BotController(PlayerController *pCon, QWidget *parent = nullptr);
    ~BotController();

private slots:
    void on_healBtn_clicked();

    void on_aotBtn_clicked();

    void on_attackBtn_clicked();

    void on_chatBtn_clicked();

    void on_complimentBtn_clicked();

    void on_packet_send_clicked();

private:
    Ui::BotController *ui;
    PlayerController *pCon;
    QList<QString> names;
    QList<int> ids;

    void fillPlayerLists();
};

#endif // BOTCONTROLLER_H
