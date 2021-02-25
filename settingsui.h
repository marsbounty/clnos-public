#ifndef SETTINGSUI_H
#define SETTINGSUI_H

#include <QDialog>

namespace Ui {
class SettingsUI;
}

class Player;
class QStandardItemModel;

class SettingsUI : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsUI(Player *ply,QWidget *parent = 0);
    ~SettingsUI();

private slots:
    void on_saveprofile_clicked();
//    void on_craft_cella_start_clicked();
    void on_checkBox_clicked();
    void on_accept_group_toggled(bool checked);
    void on_master_name_editingFinished();
    void on_req_trades_currentIndexChanged(int index);

    void on_wearSP_toggled(bool checked);

    void on_followMaster_toggled(bool checked);

    void on_goldBuffer_valueChanged(int arg1);


    void on_autoCollect_toggled(bool checked);

    void on_autoJoinSheep_toggled(bool checked);

    void on_loot_addItem_clicked();

    void on_loot_removeItem_clicked();

    void on_restartSheep_toggled(bool checked);

    void on_lootAll_toggled(bool checked);

    void on_lootFixed_toggled(bool checked);

    void on_lootAMAP_toggled(bool checked);

    void on_lootFixedGold_valueChanged(int arg1);

    void on_autoPotEnable_toggled(bool checked);

    void on_autoPotPercent_valueChanged(int arg1);

    void on_autoPotAdd_clicked();

    void on_autoPotClear_clicked();

    void on_raidInvites_toggled(bool checked);

    void on_mimicSkills_toggled(bool checked);

    void on_mimicExperimental_toggled(bool checked);
private:
    Ui::SettingsUI *ui;
    Player *ply;

    void loadSettings();
    void loadTradeItems();

    int getPosOfId(int id);
    void showHealItems();

    QList<QList<int> > tradeItems;
    QList<int> healItems;
};

#endif // SETTINGSUI_H
