#ifndef PACKETLOGGER_H
#define PACKETLOGGER_H

#include <QDialog>
#include <QStringListModel>
#include <QPointer>

class PlayerController;
class Player;

namespace Ui {
class PacketLogger;
}

class PacketLogger : public QDialog
{
    Q_OBJECT

public:
    explicit PacketLogger(PlayerController *pCon,QWidget *parent = 0);
    ~PacketLogger();

private slots:
    void on_clear_packetView_clicked();
    void on_add_filter_recieve_clicked();
    void on_remove_filter_recieve_clicked();
    void on_add_filter_send_clicked();
    void on_remove_filter_send_clicked();
    void updatePList();

    void recieveRecievePacket(QStringList packet);
    void recieveSendPacket(QStringList packet);
    void on_sendPacket_clicked();

    void on_botname_currentTextChanged(const QString &arg1);

private:
    Ui::PacketLogger *ui;
    QStringListModel *recvModel;
    QStringListModel *sendModel;
    PlayerController *pCon;
    QPointer<Player> ply;

    void selectChar(QString name);
    QString getTimestamp();
};

#endif // PACKETLOGGER_H
