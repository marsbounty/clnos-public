#ifndef MONEYUI_H
#define MONEYUI_H

#include <QMap>
#include <QDialog>



namespace Ui {
class MoneyUI;
}

struct raid{
    int id;
    QString owner;
    int playerCount;
};

class Player;
class bItem;
class itemlist;

class MoneyUI : public QDialog
{
    Q_OBJECT

public:
    explicit MoneyUI(Player* ply, QWidget *parent = 0);
    ~MoneyUI();

private slots:
    void inv_update();

    void on_buy_unlimited_toggled(bool checked);
    void on_buy_additem_clicked();
    void on_buy_remove_clicked();
    void on_buy_removeall_clicked();
    void on_sell_addupdate_clicked();
    void on_sell_beatcomp_toggled(bool checked);
    void on_sell_removeall_clicked();
    void on_sell_remove_clicked();
    void on_buy_starttoggle_clicked();
    void on_sell_starttoggle_clicked();

    void on_crafting_start_clicked();

    void on_collect_sold_clicked();

    void on_buyDelay_editingFinished();

    void on_sellDelay_editingFinished();

    void on_scriptSearch_clicked();

    void on_scriptRun_clicked();

    void on_joinMiniland_clicked();

    void on_joinArena_clicked();


    void on_collectable_reload_clicked();

    void on_toggleCollect_clicked();

    void on_inviteMiniland_clicked();

    void on_joinOwnMiniland_clicked();

    void on_autoCollectSold_toggled(bool checked);

    void getCurrentSellStatus(QList<QList<int>> items);
    void cancelSelling();
    void joinRaid();

    void on_bazarSearchButton_clicked();
    void recieveBazarList(QMap<int,bItem*> list);
    void buyBazarItem();

    void on_asMuchAsP_toggled(bool checked);

    void on_create_sheep_clicked();

    void on_collectIfCheaper_toggled(bool checked);

    void on_collectMargin_valueChanged(int arg1);

    void on_collectDelay_valueChanged(int arg1);

    void on_ammountMargin_valueChanged(int arg1);


    void on_mount_clicked();

    void on_raidlistUpdate_clicked();
    void drawRaidList(QList<raid> list);

    void on_joinAOT_clicked();

    void on_attackClosest_clicked();

    void on_useAllBuffs_clicked();

    void on_attackClosestPlayer_clicked();
    void onSkillPressed();

    void on_equipdraco_clicked();

    void on_equipglace_clicked();

    void on_wearsp_clicked();

    void on_btn_openShop_clicked();

private:
    Ui::MoneyUI *ui;
    Player* ply;
    itemlist* buylist;
    itemlist* sellist;

    void generateSkillButtons();
    void updateStatus();
    void init();
    void loadCollectables();
};

#endif // MONEYUI_H
