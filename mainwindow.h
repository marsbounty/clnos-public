#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "flowlayout.h"
#include <QPointer>

class QLabel;
class PlayerController;
class PlayerUI;
class PacketLogger;
class BotController;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void addTab(QString name,PlayerUI *plyui);
    void updateBotCount(int curr, int max);
    void updateGold(int total,int earned);

private slots:
    void on_actionNew_triggered();

    void on_actionLoad_from_Profile_triggered();

    void on_actionPacket_Logger_triggered();

    void on_actionAkt_4_Status_triggered();

    void on_actionSave_current_Session_triggered();

    void on_actionLoad_last_Session_triggered();

    void on_actionSettings_triggered();

    void on_actionQuit_triggered();

    void on_closepsa_clicked();

    void on_actionLoad_Session_triggered();

    void on_actionArena_of_Talents_triggered();

    void on_actionAuto_Buff_Master_triggered();

    void on_actionUse_all_available_Buffs_triggered();

    void on_actionCompliment_Player_triggered();

    void on_actionControls_triggered();

protected:
void  closeEvent(QCloseEvent*ev);

private:
    Ui::MainWindow *ui;
    PlayerController *pCon;
    FlowLayout *dashLayout;
    QPointer<PacketLogger> plog;
    QLabel* botCount;
    QLabel* totalGold;
    QLabel* totalEarned;
    QPointer<BotController> pConUI;
};

#endif // MAINWINDOW_H
