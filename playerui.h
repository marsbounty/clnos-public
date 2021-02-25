#ifndef PLAYERUI_H
#define PLAYERUI_H

#include <QWidget>
#include <QMap>

class Player;


namespace Ui {
class PlayerUI;
}

struct server{
    QString name;
    QString ip;
    QMap<int,int> channels;
};

class PlayerUI : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerUI(Player *ply,QWidget *parent = 0, QString username = "", QString password = "", int server = 1);
    ~PlayerUI();
    void showWindow(int id);
    void showServerSelection(QMap<int, server> &srv);
    void addChar(QString name,int id);
    void displayLoginError(QString errorstr);
    void setUsername(QString name);
    void setLvl(int lvl, int jlvl);
    void setCharImage(int i);
    void setGold(int v1,int v2);
    void showGFLoginError(QString err);

signals:
    void login(QString name,QString password,QString ip,qint16 port);

private slots:
    void icStatusUpdate(int i);
    void buyStatusUpdate(int i);
    void sellStatusUpdate(int i);
    void craftStatusUpdate(int i);

    void mapChanged(int id);
    void on_login_login_clicked();
    void on_cmb_server_currentIndexChanged(int index);
    void on_btn_select_server_clicked();
    void on_btn_select_char_clicked();
    void on_login_fail_ok_clicked();
    void on_cinfo_logout_clicked();
    void on_cinfi_minimap_clicked();
    void on_cinfo_settings_clicked();
    void on_login_cancel_clicked();
    void on_login_cancel_2_clicked();
    void on_cinfo_money_clicked();

    void on_close_clicked();

    void on_loginFailRetry_clicked();

    void on_loginFailClose_clicked();
    void runReconnect();
    void stopReconnect();

    void on_toolButton_4_clicked();

    void on_gfloginstart_clicked();

    void on_gfloginStop_clicked();


    void on_cmb_login_version_currentIndexChanged(int index);

private:
    QMap<QString,int> serverids;
    Ui::PlayerUI *ui;
    Player *ply;
    QMap<int,server> servers;
    QMap<QString,int> chars;
    bool autologin;
    int reconnectTime;
};

#endif // PLAYERUI_H
