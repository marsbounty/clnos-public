#include "settingsui.h"
#include "ui_settingsui.h"
#include "settings.h"
#include "player.h"
#include "QStandardItemModel"
#include "nosstrings.h"
#include "inventory.h"
#include "misc.h"
#include "bazarmanager.h"
#include <QCompleter>

SettingsUI::SettingsUI(Player *ply, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsUI),
    ply(ply)
{
    ui->setupUi(this);
    open();
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle("Settings - "+ply->getName());
    ui->saveprofile_name->setValidator(new QRegExpValidator(QRegExp("([0-9]|[a-z]|[A-Z]){0,15}")));
    ui->settingsTabs->setCurrentIndex(0);
    tradeItems.clear();
    //connect(ply->getInv(),SIGNAL(updateInv()),this,SLOT(updateInv()));
    setWindowIcon(QIcon(QPixmap(":/icons/ressources/settingsicon.png")));
    QCompleter *completer = new QCompleter(NosStrings::getItemNames(),this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    ui->loot_name->setCompleter(completer);
    ui->autoPotLine->setCompleter(completer);
    loadSettings();

}

SettingsUI::~SettingsUI()
{
    ply->closeSettings();
    delete ui;
}

void SettingsUI::on_saveprofile_clicked()
{
    QString name = ui->saveprofile_name->text();
    if(name != ""){
        ply->getBazar()->saveItemLists();
        ply->getSettings()->saveSettings(ui->saveprofile_name->text());
    }
}

void SettingsUI::on_checkBox_clicked()
{
    ply->getSettings()->changeSetting("misc/joinIC",ui->checkBox->isChecked());
}

void SettingsUI::loadSettings()
{
    ui->checkBox->setChecked(ply->getSettings()->getSetting("misc/joinIC").toBool());
    ui->req_trades->setCurrentIndex(ply->getSettings()->getSetting("misc/acceptTrades").toInt());
    ui->accept_group->setChecked(ply->getSettings()->getSetting("misc/joinGroups").toBool());

    ui->saveprofile_name->setText(ply->getSettings()->getName());
    ui->master_name->setText(ply->getSettings()->getSetting("misc/master").toString());
    ui->wearSP->setChecked(ply->getSettings()->getSetting("misc/wearSP").toBool());

    ui->masterid->setText(QString::number(ply->getSettings()->getSetting("misc/masterid").toInt()));
    ui->followMaster->setChecked(ply->getSettings()->getSetting("misc/followMaster").toBool());

    ui->goldBuffer->setValue(ply->getSettings()->getSetting("misc/goldbuffer").toInt());
    ui->autoCollect->setChecked(ply->getSettings()->getSetting("misc/autoCollect").toBool());

    ui->autoJoinSheep->setChecked(ply->getSettings()->getSetting("misc/joinSheeps").toBool());
    tradeItems = ply->getSettings()->getSetting("loot/items").value<QList<QList<int> >>();

    ui->restartSheep->setChecked(ply->getSettings()->getSetting("misc/createSheepRaids").toBool());

    ui->raidInvites->setChecked(ply->getSettings()->getSetting("misc/acceptRaidRequest").toBool());

    ui->autoPotEnable->setChecked(ply->getSettings()->getSetting("misc/autoHeal").toBool());
    ui->autoPotPercent->setValue(ply->getSettings()->getSetting("misc/autoHealPercent").toInt());

    ui->mimicSkills->setChecked(ply->getSettings()->getSetting("misc/mimicSkills").toBool());
    ui->mimicExperimental->setChecked(ply->getSettings()->getSetting("misc/mimicSkillsExperimental").toBool());
    ui->mimicExperimental->setEnabled(ply->getSettings()->getSetting("misc/mimicSkills").toBool());

    int lootradio = ply->getSettings()->getSetting("loot/gold").toInt();
    if(lootradio == -1){
        ui->lootAMAP->setChecked(true);
        ui->lootFixedGold->setEnabled(false);
    }else{
        ui->lootFixed->setChecked(true);
        ui->lootFixedGold->setValue(lootradio);
    }


    healItems = ply->getSettings()->getSetting("misc/healItems").value<QList<int>>();
    loadTradeItems();
    showHealItems();
}

void SettingsUI::loadTradeItems()
{

    ui->loot_itemlist->clear();
    foreach (QList<int> item, tradeItems) {
        bool all = item.at(1)==-1;
        ui->loot_itemlist->addItem(NosStrings::getItemName(item.at(0))+" | x"+(all?QString("ALL"):QString::number(item.at(1))));
    }
}

int SettingsUI::getPosOfId(int id)
{
    int count = 0;
    foreach (QList<int> item, tradeItems) {
        if(item.at(0)==id){
            return count;
        }
        count++;
    }
    return -1;
}

void SettingsUI::showHealItems()
{
    ui->autoPotList->clear();
    foreach (int i, healItems) {
        NosStrings::getItemName(i);
        ui->autoPotList->addItem(NosStrings::getItemName(i));
    }
}

void SettingsUI::on_accept_group_toggled(bool checked)
{
    ply->getSettings()->changeSetting("misc/joinGroups",checked);
}

void SettingsUI::on_master_name_editingFinished()
{
    ply->getSettings()->changeSetting("misc/master",ui->master_name->text());
    ply->getMisc()->startMasterSearch();
    ui->masterid->setText(QString::number(ply->getSettings()->getSetting("misc/masterid").toInt()));
}

void SettingsUI::on_req_trades_currentIndexChanged(int index)
{
    ply->getSettings()->changeSetting("misc/acceptTrades",index);
}

void SettingsUI::on_wearSP_toggled(bool checked)
{
    ply->getSettings()->changeSetting("misc/wearSP",checked);
}

void SettingsUI::on_followMaster_toggled(bool checked)
{
    ply->getSettings()->changeSetting("misc/followMaster",checked);
    ply->getMisc()->toggleFollower();
}

void SettingsUI::on_goldBuffer_valueChanged(int arg1)
{
    ply->getSettings()->changeSetting("misc/goldbuffer",arg1);
}


void SettingsUI::on_autoCollect_toggled(bool checked)
{
    ply->getSettings()->changeSetting("misc/autoCollect",checked);
}

void SettingsUI::on_autoJoinSheep_toggled(bool checked)
{
    ply->getSettings()->changeSetting("misc/joinSheeps",checked);
}

void SettingsUI::on_loot_addItem_clicked()
{
    int itemid = NosStrings::getItemId(ui->loot_name->text());
    int ammount = ui->tradeAmmount->value();
    if(ui->lootAll->isChecked()){
        ammount = -1;
    }
    if(itemid != 0){
        if(getPosOfId(itemid)==-1){
            tradeItems.append({itemid,ammount});
            loadTradeItems();
        }
    }
    ply->getSettings()->changeSetting("loot/items",QVariant::fromValue(tradeItems));
}

void SettingsUI::on_loot_removeItem_clicked()
{
    QListWidgetItem* curr = ui->loot_itemlist->currentItem();
    if(curr == nullptr){
        return;
    }
    int itemid = NosStrings::getItemId(curr->text().split(" |").at(0));
    int id = getPosOfId(itemid);
    if(id!=-1){
        tradeItems.removeAt(id);
    }
    ply->getSettings()->changeSetting("loot/items",QVariant::fromValue(tradeItems));
    loadTradeItems();
}

void SettingsUI::on_restartSheep_toggled(bool checked)
{
    ply->getSettings()->changeSetting("misc/createSheepRaids",checked);
}

void SettingsUI::on_lootAll_toggled(bool checked)
{
    ui->tradeAmmount->setEnabled(!checked);
}

void SettingsUI::on_lootFixed_toggled(bool checked)
{
    ui->lootFixedGold->setEnabled(checked);
    if(checked){
        ply->getSettings()->changeSetting("loot/gold",ui->lootFixedGold->value());
    }
}

void SettingsUI::on_lootAMAP_toggled(bool checked)
{
    if(checked){
        ply->getSettings()->changeSetting("loot/gold",-1);
    }
}

void SettingsUI::on_lootFixedGold_valueChanged(int arg1)
{
    ply->getSettings()->changeSetting("loot/gold",arg1);
}

void SettingsUI::on_autoPotEnable_toggled(bool checked)
{
    ply->getSettings()->changeSetting("misc/autoHeal",checked);
}

void SettingsUI::on_autoPotPercent_valueChanged(int arg1)
{
    ply->getSettings()->changeSetting("misc/autoHealPercent",arg1);
}

void SettingsUI::on_autoPotAdd_clicked()
{
    int itemid = NosStrings::getItemId(ui->autoPotLine->text());
    if(itemid != 0){
        if(!healItems.contains(itemid)){
            healItems.append(itemid);
            showHealItems();
        }
    }

    ply->getSettings()->changeSetting("misc/healItems",QVariant::fromValue(healItems));
}

void SettingsUI::on_autoPotClear_clicked()
{
    healItems.clear();
    showHealItems();
    ply->getSettings()->changeSetting("misc/healItems",QVariant::fromValue(healItems));
}

void SettingsUI::on_raidInvites_toggled(bool checked)
{
    ply->getSettings()->changeSetting("misc/acceptRaidRequest",checked);
}

void SettingsUI::on_mimicSkills_toggled(bool checked)
{
    ply->getSettings()->changeSetting("misc/mimicSkills",checked);
    ui->mimicExperimental->setEnabled(checked);
}

void SettingsUI::on_mimicExperimental_toggled(bool checked)
{
    ply->getSettings()->changeSetting("misc/mimicSkillsExperimental",checked);
}

