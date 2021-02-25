#ifndef CHATUI_H
#define CHATUI_H

#include <QDialog>

namespace Ui {
class chatUI;
}
class chatmng;

class chatUI : public QDialog
{
    Q_OBJECT

public:
    explicit chatUI(chatmng *mng, QWidget *parent = 0);
    ~chatUI();

private:
    Ui::chatUI *ui;
    chatmng* mng;
    void init();

private slots:
    void addChat(QString msg,int type);
    void on_chatType_currentIndexChanged(int index);
    void on_send_clicked();
};

#endif // CHATUI_H
