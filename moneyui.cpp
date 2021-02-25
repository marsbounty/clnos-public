#include <QDesktopServices>
#include <QMap>
#include <QCryptographicHash>
#include "moneyui.h"
#include "ui_moneyui.h"
#include "player.h"
#include "itemlist.h"
#include "bitem.h"
#include "nosstrings.h"
#include "QCompleter"
#include "bazarmanager.h"
#include "networkmanager.h"
#include "inventory.h"
#include "misc.h"
#include "settings.h"
#include "logger.h"
#include "battlemanager.h"
#include "nosobjectmanager.h"
#include "objects/skill.h"
#include <QDebug>


MoneyUI::MoneyUI( Player *ply,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MoneyUI),
    ply(ply)

{
    ui->setupUi(this);
    show();
    setAttribute(Qt::WA_DeleteOnClose);
    buylist = ply->getBazar()->getBuylist();
    sellist = ply->getBazar()->getSelllist();
    ui->buy_buylist->setModel(buylist);
    QCompleter *completer = new QCompleter(NosStrings::getItemNames(),this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    ui->buy_itemname->setCompleter(completer);
    ui->sell_selllist->setModel(sellist);
    ui->sell_itemname->setCompleter(completer);
    ui->bazarSearch->setCompleter(completer);
    setWindowTitle("MoneyUI - "+ply->getName());
    ui->medal->setText(ply->getHasMedal()?"Medal Available":"No Medal Found");
    connect(ply->getInv(),SIGNAL(updateInv()),this,SLOT(inv_update()));
    ui->buyDelay->setValue(ply->getSettings()->getSetting("basar/buyDelay").toInt());
    ui->sellDelay->setValue(ply->getSettings()->getSetting("basar/sellDelay").toInt());
    ui->collectIfCheaper->setChecked(ply->getSettings()->getSetting("basar/collectIfCheaper").toBool());
    ui->collectMargin->setValue(ply->getSettings()->getSetting("basar/collectIfCheaperMargin").toInt());
    ui->ammountMargin->setValue(ply->getSettings()->getSetting("basar/collectAmmountMargin").toInt());
    ui->collectDelay->setValue(ply->getSettings()->getSetting("basar/collectDelay").toInt()/60000);
    ui->scripting->setEnabled(false);
    ui->tabs->setCurrentIndex(0);
    updateStatus();
    inv_update();
    connect(ply->getBazar(),SIGNAL(currentSellList(QList<QList<int> >)),this,SLOT(getCurrentSellStatus(QList<QList<int> >)));
    connect(ply->getBazar(),SIGNAL(updateBazarList(QMap<int,bItem*>)),this,SLOT(recieveBazarList(QMap<int,bItem*>)));
    connect(ply->getMisc(),SIGNAL(raidListUpdate(QList<raid>)),this,SLOT(drawRaidList(QList<raid>)));
    init();
    setWindowIcon(QIcon(QPixmap(":/icons/ressources/Gold.png")));
    generateSkillButtons();
}

MoneyUI::~MoneyUI()
{
    delete ui;
}

void MoneyUI::inv_update()
{
    ui->inv_cella->setText(QString::number(ply->getInv()->getItemCount(1014)));
    ui->inv_gillion->setText(QString::number(ply->getInv()->getItemCount(1013)));
    int flaskcount = 0;
    flaskcount += ply->getInv()->getItemCount(1036);
    flaskcount += ply->getInv()->getItemCount(1037);
    flaskcount += ply->getInv()->getItemCount(1038);
    ui->inv_flasks->setText(QString::number(flaskcount));
    ui->freeInvSlots->setText(QString::number(ply->getInv()->getFreeInvSlots()));
    ui->inv_grass->setText(QString::number(ply->getInv()->getItemCount(1153)));
    ui->inv_ore->setText(QString::number(ply->getInv()->getItemCount(1179)));
    ui->inv_wheat->setText(QString::number(ply->getInv()->getItemCount(1178)));
    ui->inv_bbfrucht->setText(QString::number(ply->getInv()->getItemCount(1162)));

    ui->mainInvList->clear();
    ui->otherInvList->clear();
    ui->eqInvList->insertItems(0,ply->getInv()->getInvList(0));
    ui->mainInvList->insertItems(0,ply->getInv()->getInvList(1));
    ui->otherInvList->insertItems(0,ply->getInv()->getInvList(2));
}

void MoneyUI::on_buy_unlimited_toggled(bool checked)
{
    ui->buy_ammount->setEnabled(!checked);
}

void MoneyUI::on_buy_additem_clicked()
{
    int itemid = NosStrings::getItemId(ui->buy_itemname->text());
    int maxamm = ui->buy_unlimited->isChecked()?-1:ui->buy_ammount->value();
    int price = ui->buy_price->value();
    if(itemid != 0){
        bItem* newi = new bItem();
        newi->setItemid(itemid);
        newi->setMaxammount(maxamm);
        newi->setPrice(price);
        buylist->addUpdate(newi);
    }
}

void MoneyUI::on_buy_remove_clicked()
{
    QModelIndex index = ui->buy_buylist->selectionModel()->currentIndex();
    if(index.isValid()){
        buylist->removeItem(index.row());
    }
}

void MoneyUI::on_buy_removeall_clicked()
{
    buylist->clear();
}

void MoneyUI::on_sell_addupdate_clicked()
{
    int itemid = NosStrings::getItemId(ui->sell_itemname->text());
    int beatby = ui->sell_beatcomp->isChecked()?ui->sell_beatcompby->value():-1;
    int ammount = ui->sell_stacksize->value();
    bool asMuchAsP = ui->asMuchAsP->isChecked();
    int price = ui->sell_price->value();
    if(itemid != 0){
        bItem* newi = new bItem();
        newi->setItemid(itemid);
        newi->setMaxammount(beatby);
        newi->setAmmount(asMuchAsP?-1:ammount);
        newi->setPrice(price);
        newi->setType(ui->sell_beatmyself->isChecked()?1:0);
        sellist->addUpdate(newi);
    }
}

void MoneyUI::on_sell_beatcomp_toggled(bool checked)
{
    ui->sell_beatcompby->setEnabled(checked);
}

void MoneyUI::on_sell_removeall_clicked()
{
    sellist->clear();
}

void MoneyUI::on_sell_remove_clicked()
{
    QModelIndex index = ui->sell_selllist->selectionModel()->currentIndex();
    if(index.isValid()){
        sellist->removeItem(index.row());
    }
}

void MoneyUI::on_buy_starttoggle_clicked()
{
    ply->getBazar()->toggleAutobuy();
    updateStatus();
}

void MoneyUI::on_sell_starttoggle_clicked()
{
    ply->getBazar()->toggleAutosell();
    updateStatus();
}

void MoneyUI::updateStatus()
{
    ui->sell_status->setText(ply->getBazar()->getAutoSellStatus()?(ply->getBazar()->getPaused()?"Paused":"Running"):"Stopped");
    ui->buy_status->setText(ply->getBazar()->getAutoBuyStatus()?(ply->getBazar()->getPaused()?"Paused":"Running"):"Stopped");
    if(ply->getBazar()->getAutoBuyStatus()){
        ui->buyDelay->setEnabled(false);
    }else{
        ui->buyDelay->setEnabled(true);
    }
    if(ply->getBazar()->getAutoSellStatus()){
        ui->sellDelay->setEnabled(false);
        ui->autoCollectSold->setEnabled(false);
    }else{
        ui->sellDelay->setEnabled(true);
        ui->autoCollectSold->setEnabled(true);
    }
    ui->crafting_status->setText(ply->getMisc()->getCraftStatus()==0?"Stopped":"Started");
    ui->sell_starttoggle->setText(ply->getBazar()->getAutoSellStatus()?"Stop":"Start");
    ui->buy_starttoggle->setText(ply->getBazar()->getAutoBuyStatus()?"Stop":"Start");
    ui->crafting_start->setText(ply->getMisc()->getCraftStatus()==0?"Start":"Stop");
}

void MoneyUI::init()
{
    loadCollectables();

    if(ply->getMisc()->getIsCollecting()){
        ui->toggleCollect->setText("Stop Collecting");
    }else{
        ui->toggleCollect->setText("Start Collecting");
    }
    if(ply->getInv()->getItemCount(2072)==0){
        ui->joinOwnMiniland->setEnabled(false);
    }



    ui->autoCollectSold->setChecked(ply->getSettings()->getSetting("basar/autoCollectSold").toBool());


    ui->bazarResults->setColumnCount(7);
    ui->bazarResults->setHorizontalHeaderLabels({"Item","Price","Ammount","Owner","Timeleft","",""});
    ui->bazarResults->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->bazarResults->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Stretch);
    ui->basarStatus->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->basarStatus->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Stretch);
    ui->raidlistList->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->raidlistList->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Stretch);
    ui->sell_selllist->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->sell_selllist->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Stretch);
    ui->buy_buylist->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->buy_buylist->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Stretch);
}

void MoneyUI::loadCollectables()
{
    ui->list_collectable->clear();
    ui->list_collectable->addItems(ply->getMisc()->getCollectables());
}

void MoneyUI::on_crafting_start_clicked()
{
    ply->getMisc()->toggleCrafting();
    updateStatus();
}

void MoneyUI::on_collect_sold_clicked()
{
   ply->getBazar()->checkSold();
}

void MoneyUI::on_buyDelay_editingFinished()
{
    ply->getSettings()->changeSetting("basar/buyDelay",ui->buyDelay->value());
}

void MoneyUI::on_sellDelay_editingFinished()
{
    ply->getSettings()->changeSetting("basar/sellDelay",ui->sellDelay->value());
}

void MoneyUI::on_scriptSearch_clicked()
{
    //QFileDialog::getOpenFileUrl();
}

void MoneyUI::on_scriptRun_clicked()
{
    //new lua(ui->scriptPath->text(),this);
}

void MoneyUI::on_joinMiniland_clicked()
{
    int id = ply->getSettings()->getSetting("misc/masterid").toInt();
    if(id!=0){
        ply->getMisc()->customDelay(5,"mjoin 1 "+QString::number(id),"-2");
    }
}

void MoneyUI::on_joinArena_clicked()
{
    ply->getMisc()->joinArena();
}

void MoneyUI::on_collectable_reload_clicked()
{
    loadCollectables();
}

void MoneyUI::on_toggleCollect_clicked()
{
    if(ply->getMisc()->getIsCollecting()){
        ui->toggleCollect->setText("Start Collecting");
        ply->getMisc()->stopCollecting();
    }else{
        ui->toggleCollect->setText("Stop Collecting");
        ply->getMisc()->startCollecting();
    }
}

void MoneyUI::on_inviteMiniland_clicked()
{
    QString id = ply->getSettings()->getSetting("misc/master").toString();
    if(id!=""){
        ply->getNet()->send(Settings::getGlobalSettings()->getSetting("packets/minilandInv").toString()+" "+id);
    }
}

void MoneyUI::on_joinOwnMiniland_clicked()
{
    if(ply->getInv()->getItemCount(2072)!=0){
        ply->getInv()->useItem(2072);
    }
}

void MoneyUI::on_autoCollectSold_toggled(bool checked)
{
    ply->getSettings()->changeSetting("basar/autoCollectSold",checked);
}

void MoneyUI::getCurrentSellStatus(QList<QList<int> > items)
{

    ui->basarStatus->clear();
    ui->basarStatus->setRowCount(items.count());
    int i = 0;
    QList<int> var;
    foreach (var, items) {
        //ui->basarStatus->setItem(i,0,new QTableWidgetItem(QString::number(var.first)));
        ui->basarStatus->setItem(i,0,new QTableWidgetItem(NosStrings::getItemName(var.at(1))));
        ui->basarStatus->setItem(i,1,new QTableWidgetItem(QString::number(var.at(2))+"/"+QString::number(var.at(3))+" Sold"));
        ui->basarStatus->setItem(i,2,new QTableWidgetItem(QString::number(var.at(4))+" Gold"));
        ui->basarStatus->setItem(i,3,new QTableWidgetItem(QString::number(var.at(5))+" min left"));
        QPushButton *bttn = new QPushButton("Cancel");
        bttn->setObjectName(QString::number(var.at(0)));
        connect(bttn,SIGNAL(clicked()),this,SLOT(cancelSelling()));
        ui->basarStatus->setCellWidget(i,4,bttn);
        i++;
    }
}

void MoneyUI::cancelSelling()
{
    QObject *senderObj = sender();
    QString item = senderObj->objectName();
    ply->getBazar()->collectItem(item.toInt());
    ply->getBazar()->checkSold();
}

void MoneyUI::joinRaid()
{
    QObject *senderObj = sender();
    QString item = senderObj->objectName();
    ply->getNet()->send("rl 3 "+item);
}


void MoneyUI::on_bazarSearchButton_clicked()
{
    int id = NosStrings::getItemId(ui->bazarSearch->text());
    if(NosStrings::getItemId(ui->bazarSearch->text())!=0){
        ply->getBazar()->searchItem(id,3);
    }
}

void MoneyUI::recieveBazarList(QMap<int, bItem *> list)
{
    ui->bazarResults->clear();
    ui->bazarResults->setHorizontalHeaderLabels({"Item","Price","Ammount","Owner","Timeleft","",""});
    ui->bazarResults->setRowCount(list.count());
    int c = 0;
    QMapIterator<int, bItem*> i(list);
    while (i.hasNext()) {
        i.next();
        //ui->bazarResults->setItem(c,0, new QTableWidgetItem(QString::number(i.key())));
        ui->bazarResults->setItem(c,0, new QTableWidgetItem(NosStrings::getItemName(i.value()->getItemid())));
        QTableWidgetItem *newW = new QTableWidgetItem();
        newW->setData(Qt::DisplayRole,i.value()->getPrice());
        ui->bazarResults->setItem(c,1, newW);
        ui->bazarResults->setItem(c,2, new QTableWidgetItem(QString::number(i.value()->getAmmount())));
        ui->bazarResults->setItem(c,3, new QTableWidgetItem(i.value()->getOwner()));
        QPushButton *temp = new QPushButton("Buy All");
        temp->setObjectName(QString::number(i.key())+"|"+QString::number(i.value()->getAmmount()));
        temp->setEnabled(ui->enableBuyButtons->isChecked());
        QPushButton *temp2 = new QPushButton("Buy One");
        temp2->setObjectName(QString::number(i.key())+"|"+QString::number(1));
        temp2->setEnabled(ui->enableBuyButtons->isChecked());
        connect(temp,SIGNAL(clicked()),this,SLOT(buyBazarItem()));
        connect(temp2,SIGNAL(clicked()),this,SLOT(buyBazarItem()));
        int timeleft = i.value()->getTimeleft();
        QString timeString = timeleft>60?(QString::number(timeleft/60)+" h"):(QString::number(timeleft)+" min");
        ui->bazarResults->setItem(c,4, new QTableWidgetItem(timeString));
        ui->bazarResults->setCellWidget(c,5, temp);
        ui->bazarResults->setCellWidget(c,6, temp2);
        c++;
    }
    ui->bazarResults->sortByColumn(1,Qt::AscendingOrder);

}

void MoneyUI::buyBazarItem()
{
    QObject *senderObj = sender();
    QStringList temp = senderObj->objectName().split("|");
    int tid = temp.at(0).toInt();
    int ammount = temp.at(1).toInt();
    ply->getBazar()->buyItem(tid,ammount);
}


void MoneyUI::on_asMuchAsP_toggled(bool checked)
{
    ui->sell_stacksize->setEnabled(!checked);
}

void MoneyUI::on_create_sheep_clicked()
{
    ply->getMisc()->createSheepRaid();
}

void MoneyUI::on_collectIfCheaper_toggled(bool checked)
{
    ply->getSettings()->changeSetting("basar/collectIfCheaper",checked);
}

void MoneyUI::on_collectMargin_valueChanged(int arg1)
{
    ply->getSettings()->changeSetting("basar/collectIfCheaperMargin",arg1);
}

void MoneyUI::on_collectDelay_valueChanged(int arg1)
{
    ply->getSettings()->changeSetting("basar/collectDelay",arg1*60000);
}

void MoneyUI::on_ammountMargin_valueChanged(int arg1)
{
    ply->getSettings()->changeSetting("basar/collectAmmountMargin",arg1);
}

void MoneyUI::on_mount_clicked()
{
    QList<int> ids = {1906,1907,9054,9055,9058,5173,9073,5117,5196,5914,9079,9080,9081,9082,9078,9083,9084,9085,9086,9087,9088,9090,9091,9092,9093,9094};
    foreach (int id, ids) {
        if(ply->getInv()->getItemCount(id)!=0){
            ply->getInv()->useItem(id);
            ply->getLog()->addLog("MISC","Mounting "+QString::number(id));
            return;
        }
    }
    ply->getLog()->addLog("MISC","No Mount found");
}

void MoneyUI::on_raidlistUpdate_clicked()
{
    ply->getNet()->send("rl");
}

void MoneyUI::drawRaidList(QList<raid> list)
{
    ui->raidlistList->setRowCount(list.count());
    ui->raidlistList->clear();
    QMap<int,QString> raidNames = {{0,"Cuby"},{16,"Pirate"}};
    int i = 0;
    foreach (raid rd, list) {
        QString name = QString::number(rd.id);
        if(raidNames.contains(rd.id)){
            name= raidNames.value(rd.id);
        }
        ui->raidlistList->setItem(i,0,new QTableWidgetItem(name));
        ui->raidlistList->setItem(i,1,new QTableWidgetItem(rd.owner));
        ui->raidlistList->setItem(i,2,new QTableWidgetItem(QString::number(rd.playerCount)+" Players"));
        QPushButton *bttn = new QPushButton("Join");
        bttn->setObjectName(rd.owner);
        connect(bttn,SIGNAL(clicked()),this,SLOT(joinRaid()));
        ui->raidlistList->setCellWidget(i,3,bttn);
        i++;
    }
}

void MoneyUI::on_joinAOT_clicked()
{
    ply->getMisc()->joinAOT();
}

void MoneyUI::on_attackClosest_clicked()
{
    ply->getBmanager()->attackClosestEnemy();
}

void MoneyUI::on_useAllBuffs_clicked()
{
    ply->getBmanager()->useAllBuffs();
}

void MoneyUI::on_attackClosestPlayer_clicked()
{
    ply->getBmanager()->attackClosestPlayer();
}

void MoneyUI::onSkillPressed()
{
    QObject *senderObj = sender();
    int id = senderObj->objectName().toInt();

    ply->getLog()->addLog("UI",QString("Pressed Skill with id %1").arg(id));
    ply->getBmanager()->useSkillSimple(id);
}

void MoneyUI::generateSkillButtons()
{
    QList<int> used;
    const QMap<int,skill*> skills = nosObjectManager::getSkills();
    foreach(int id,ply->getAvailSkills()){
        if(skills.value(id)->getType()!=0&&!used.contains(id)&&skills.value(id)->getType()==1){
            QWidget* button = new QPushButton(skills.value(id)->getName(),this);
            button->setObjectName(QString::number(id));
            connect(button,SIGNAL(clicked()),this,SLOT(onSkillPressed()));
            ui->skillLayout->addWidget(button);
            used.append(id);
        }
    }


}

void MoneyUI::on_equipdraco_clicked()
{
    QPair<int,int> pos = ply->getInv()->getItemPos(4503);
    if(pos.first!=-1){
        ply->getNet()->send(QString("wear %1 %2").arg(pos.second).arg(pos.first));
    }
}

void MoneyUI::on_equipglace_clicked()
{
    QPair<int,int> pos = ply->getInv()->getItemPos(4504);
    if(pos.first!=-1){
        ply->getNet()->send(QString("wear %1 %2").arg(pos.second).arg(pos.first));
    }
}

void MoneyUI::on_wearsp_clicked()
{
    ply->getNet()->send("sl 0");
}

void MoneyUI::on_btn_openShop_clicked()
{
    int sid = ply->getSettings()->getSetting("login/server").toInt()+1;
    int pid = ply->getId();
    QString userId = ply->getNet()->getUsername();
    QString m_szName = ply->getName();
    QByteArray sastemp = QString("%1%2%3%4shtmxpdlfeoqkr").arg(sid).arg(pid).arg(userId).arg(m_szName).toUtf8();
    QString sas = QString(QCryptographicHash::hash(sastemp,QCryptographicHash::Md5).toHex().toUpper());
    QStringList lang = {"en","de","fr","it","pl","es","cz","ru","tr"};
    QString c = lang.at(ply->getNet()->getLangid());
    QString url = QString("https://intl-shop-nostale.gameforge.com/?sid=%1&pid=%2&user_id=%3&m_szName=%4&sas=%5&c=%6&shopType=ingame").arg(sid).arg(pid).arg(userId).arg(QUrl::toPercentEncoding(m_szName).constData()).arg(sas).arg(c);
    QDesktopServices::openUrl(QUrl(url));

}
