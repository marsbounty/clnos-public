#include "bazarmanager.h"
#include "player.h"
#include "networkmanager.h"
#include "logger.h"
#include "inventory.h"
#include "bitem.h"
#include <QTimer>
#include <QElapsedTimer>
#include "itemlist.h"
#include "settings.h"
#include "nosstrings.h"
#include <QDebug>
#include "misc.h"

BazarManager::BazarManager(Player *ply, QObject *parent) : QObject(parent),ply(ply)
{
    blist.clear();
    slist.clear();
    lastsearch = 0;
    type = 0;
    searching = false;
    autoBuyTimer = new QTimer(this);
    autoSellTimer = new QTimer(this);
    autoCollectTimer = new QTimer(this);
    autoCollectTimer->setSingleShot(true);
    autoBuyTimer->setSingleShot(true);
    autoSellTimer->setSingleShot(true);
    timeout = new QElapsedTimer();
    buylist = new itemlist(0,this);
    selllist = new itemlist(1,this);
    paused = false;
    autoBuyStatus = false;
    autoSellStatus = false;
    currSlot = {0,0};
    connect(autoBuyTimer,SIGNAL(timeout()),this,SLOT(autobuy()));
    connect(autoSellTimer,SIGNAL(timeout()),this,SLOT(autosell()));
    connect(autoCollectTimer,SIGNAL(timeout()),this,SLOT(autoCheckSold()));
    loadItemLists();
}

BazarManager::~BazarManager()
{
    saveItemLists();
}

void BazarManager::searchItem(int id,int typeN)
{
    if(!ply->getIsOldVersion()&&!ply->getMisc()->getBasarOpen()){
        ply->getLog()->addLog("BASAR","Cannot search for Items while basar is not open.");
        return;
    }

    if(ply->getConnected()&&(!timeout->isValid()||timeout->hasExpired(2500))){
        if(typeN!=0){
            type = typeN;
        }
        searching = true;
        if(timeout->isValid()){
            ply->getLog()->addLog("BAZAR","Didn't recieve Answer after "+QString::number(timeout->restart())+" !");
            type =0;
        }else {
            timeout->start();
        }
        ply->getLog()->addLog("BAZAR","Searching item "+QString::number(id));
        ply->getNet()->send("c_blist 0 0 0 0 0 0 0 0 1 "+QString::number(id));
        //ply->getNet()->send("c_slist 0 0");
        lastsearch = id;
    }else{
        ply->getLog()->addLog("Bazar","Search already in progress or not conected!");
    }
}

void BazarManager::recieveBlist(QStringList packet)
{
    qDeleteAll(blist);
    blist.clear();
    ply->getLog()->addLog("BAZAR","Recieved b_list!");
    foreach (QString item, packet) {
        QStringList temp = item.split("|");
        if(temp.size()== 15){
           bItem *newi = new bItem();
           newi->setOwner(temp[2]);
           newi->setItemid(temp[3].toInt());
           newi->setAmmount(temp[4].toInt());
           newi->setPrice(temp[6].toInt());
           newi->setTimeleft(temp[7].toInt());
           newi->setMaxammount(temp[5].toInt());
           int id = temp[0].toInt();
           blist.insert(id,newi);
        }
    }
    ply->getLog()->addLog("BAZAR","Recieved "+QString::number(blist.count())+" Items!");
    //emit bsearchFinished(lastsearch);

    if(type==1){  // Type one = buy 2 = sell
        int tid = getCheapest(searchitem->getItemid());     //get transaction id of cheapest entry
        if(tid!=0){             //if item found
            ply->getLog()->addLog("BAZAR","Found cheapest item");
            int price = blist.value(tid)->getPrice();   //update current price in ui
            searchitem->setCurrentprice(price);
            if(price<=searchitem->getPrice()){      //check if price is right
                ply->getLog()->addLog("BAZAR","Buying cheapest item");
                if(searchitem->getMaxammount()==-1){
                    buyItem(tid,-1);
                }else{
                    buyItem(tid,searchitem->getMaxammount()-searchitem->getAmmount());
                }
            }
            buylist->updateId(searchitem->getItemid());
        }
        type=0;
    }else if(type == 2){//selling
        ply->getLog()->addLog("Bazar","Checking if item should be sold");
        int tid = getCheapest(searchitem->getItemid());
        if(tid!=0){
            bItem* item = blist.value(tid);
            ply->getLog()->addLog("Bazar","Got Item ID "+QString::number(item->getItemid())+" Looking for ItemId "+QString::number(searchitem->getItemid()));
            searchitem->setCurrentprice(item->getPrice());
            searchitem->setTimeleft(item->getAmmount());
            int sellprice = item->getPrice()-searchitem->getMaxammount();//maxammount = sell comp by
            if(searchitem->getMaxammount()==-1){
                sellprice = searchitem->getPrice();
                ply->getLog()->addLog("BAZAR:Sell","Ignoring Competition selling for const Price");
            }
            int slotammount = ply->getInv()->getSlotCount(currSlot.first,currSlot.second);
            int ammount = searchitem->getAmmount();
            ammount = ammount==-1?slotammount:ammount;

            long price = sellprice * ammount;
            long taxmax = price > 100000 ? price / 200 : 500;
            long taxmin = price >= 4000 ? (60 + (price - 4000) / 2000 * 30 > 10000 ? 10000 : 60 + (price - 4000) / 2000 * 30) : 50;
            long tax = !ply->getHasMedal() ? taxmax : taxmin;

            if((sellprice<1000000||ply->getHasMedal())&&ply->getGold()>tax){
                if(sellprice>= searchitem->getPrice()){
                    if(searchitem->getType()==0){// Beat myself
                        if(item->getOwner()!=ply->getName()){
                            sellItem(currSlot,ammount,sellprice,tax,false,ply->getHasMedal());
                        }
                    }else{
                        sellItem(currSlot,ammount,sellprice,tax,false,ply->getHasMedal());
                    }
                }else{
                    ply->getLog()->addLog("BAZAR:Sell","Current Items too cheap to sell yours");
                }
            }else{
                ply->getLog()->addLog("BAZAR:Sell","Cannot sell items for more that 100.000 at the Moment unless you have a medal!");
                ply->getLog()->addLog("Bazar","Tax Money: "+QString::number(tax));
            }
        }else{
            int slotammount = ply->getInv()->getSlotCount(currSlot.first,currSlot.second);
            int ammount = searchitem->getAmmount();
            ammount = ammount==-1?slotammount:ammount;
            int sellprice = searchitem->getPrice();
            long price = sellprice * ammount;
            long taxmax = price > 100000 ? price / 200 : 500;
            long taxmin = price >= 4000 ? (60 + (price - 4000) / 2000 * 30 > 10000 ? 10000 : 60 + (price - 4000) / 2000 * 30) : 50;
            long tax = !ply->getHasMedal() ? taxmax : taxmin;
            if((sellprice<1000000||ply->getHasMedal())&&ply->getGold()>tax){
                sellItem(currSlot,ammount,sellprice,tax,false,ply->getHasMedal());
            }
        }
    }else if(type==3){
        emit updateBazarList(blist);
    }
    searching = false;
    if(timeout->isValid()){
        ply->getLog()->addLog("BAZAR","Recieved response after "+QString::number(timeout->elapsed())+"ms");
    }
    timeout->invalidate();
    type = 0;
}

void BazarManager::recieveSlist(QStringList packet)
{
    QList<QList<int>> out;
    qDeleteAll(slist);
    slist.clear();
    ply->getLog()->addLog("BAZAR","Recieved s_list!");
    foreach (QString item, packet) {
        QStringList temp = item.split("|");
        if(temp.size()== 16){
           bItem *newi = new bItem();
           newi->setItemid(temp[2].toInt());
           newi->setAmmount(temp[3].toInt());
           newi->setMaxammount(temp[4].toInt());
           newi->setPrice(temp[6].toInt());
           newi->setTimeleft(temp[8].toInt());
           int id = temp[0].toInt();
           slist.insert(id,newi);
           if(newi->getAmmount()==newi->getMaxammount()){
               collectItem(id);
           }

           int margin = ply->getSettings()->getSetting("basar/collectIfCheaperMargin").toInt();
           bool acollect = ply->getSettings()->getSetting("basar/collectIfCheaper").toBool();
           int ammountmargin = ply->getSettings()->getSetting("basar/collectAmmountMargin").toInt();
           bItem* slistItem = selllist->getItemById(newi->getItemid());
           if(slistItem!=nullptr){
               if(newi->getPrice()-margin>slistItem->getCurrentprice()&&slistItem->getMaxammount()!=-1&&acollect&&slistItem->getTimeleft()>=ammountmargin){
                   collectItem(id);
               }
           }
           out.append({id,newi->getItemid(),newi->getAmmount(),newi->getMaxammount(),newi->getPrice(),newi->getTimeleft()});
        }
    }
    emit currentSellList(out);
    ply->getLog()->addLog("BAZAR","Recieved "+QString::number(slist.count())+" Items!");
}

void BazarManager::buyItem(int id, int ammount)
{
    ply->getLog()->addLog("BAZAR","tID "+QString::number(id));
    if(blist.contains(id)){
        bItem *item = blist.value(id);
        int goldBuffer = ply->getSettings()->getSetting("misc/goldbuffer").toInt();

        int buyAmmount = (ply->getGold()-goldBuffer)/item->getPrice();
        if(ammount>=0){
            buyAmmount = buyAmmount < ammount ? buyAmmount : ammount;
            //qDebug()<<buyAmmount<<" maxamm: "<<item->getMaxammount();
        }
        if(buyAmmount>0&&item->getMaxammount()!=1){
            if(item->getAmmount()>=buyAmmount){
                ply->getLog()->addLog("BAZAR","Buying "+QString::number(buyAmmount));
                ply->getNet()->send("c_buy "+QString::number(id)+" "+QString::number(item->getItemid())+" "+QString::number(buyAmmount)+" "+QString::number(item->getPrice()));
            }else{
                ply->getNet()->send("c_buy "+QString::number(id)+" "+QString::number(item->getItemid())+" "+QString::number(item->getAmmount())+" "+QString::number(item->getPrice()));
                ply->getLog()->addLog("BAZAR","Ammount to high. Buying "+ QString::number(item->getMaxammount()));
            }
        }else{
            ply->getLog()->addLog("BAZAR","Cannot buy item! You have "+QString::number(ply->getGold()) +" ( "+ QString::number(goldBuffer*(-1))+ " ) Gold");
        }
    }else{
        ply->getLog()->addLog("BAZAR","Transaction ID not found");
    }
}

void BazarManager::collectItem(int id)
{
    if(slist.contains(id)){
        bItem *item = slist.value(id);
        ply->getNet()->send("c_scalc "+QString::number(id)+" "+QString::number(item->getItemid())+" "+QString::number(item->getAmmount())+" "+QString::number(item->getMaxammount())+" "+QString::number(item->getPrice()));
        ply->getLog()->addLog("BAZAR","Collected "+QString::number(item->getAmmount())+" Items!");
    }else{
        ply->getLog()->addLog("BAZAR","Cannot Collect Items!");
    }
}

void BazarManager::buyWhen(bItem *item)
{
    if(!(timeout->isValid()&&!timeout->hasExpired(2500))){
        searchitem=item;
        if((item->getAmmount()<item->getMaxammount())||item->getMaxammount() == -1){
            type = 1;
            searchItem(item->getItemid());
            ply->getLog()->addLog("BAZAR","Trying to autobuy "+QString::number(item->getItemid()));
        }else{
            timeout->invalidate();
        }
    }
}

QString BazarManager::getCheapestSeller(int id)
{
    int tid = getCheapest(id);
    if(tid!=0){
        return blist[tid]->getOwner();
    }
    return "";
}

int BazarManager::getCheapestPrice(int id)
{
    int tid = getCheapest(id);
    if(tid!=0){
        return blist[tid]->getPrice();
    }
    return -1;
}

void BazarManager::sellStack(int id, int price)
{
    QPair<int,int> slot = ply->getInv()->getFullestStack(id);
    if(slot.first!=0&&slot.second!=0){
        sellItem(slot,ply->getInv()->getSlotCount(slot.first,slot.second),price,500,false,false);
        //sellItem(slot,1,price,500,false,false);
    }
}

void BazarManager::sellItem(QPair<int, int> slot, int ammount, int price, int tax, bool isPackage, bool medal)
{
    Q_UNUSED(isPackage);
    if(ammount == 0){
        return;
    }
    if(slot==QPair<int,int>(0,0)){
        return;
    }
    int slotammount = ply->getInv()->getSlotCount(slot.first,slot.second);
    if(slotammount<ammount){
        return;
    }


    ply->getLog()->addLog("BAZAR:Sell","Selling "+QString::number(ammount)+" Items");
    ply->getNet()->send("c_reg 0 "+QString::number(slot.first)+" "+QString::number(slot.second)+" 9 1 "+QString::number(ply->getHasMedal()?4:1)+" 0 "+QString::number(ammount)+" "+QString::number(price)+" "+QString::number(tax)+" "+QString::number(medal?2:0));
}

void BazarManager::toggleAutobuy()
{
    autoBuyStatus = !autoBuyStatus;
    searching = false;
    if(autoBuyStatus){
        if(!paused){
            changeAutoBuy(true);
        }else{
            emit buyStatus(2);
        }
    }else{
        changeAutoBuy(false);
    }
    QString status = autoBuyStatus?"Starting":"Stopping";
    ply->getLog()->addLog("BAZAR",status+" Auto Buy");
}

void BazarManager::pause(bool st)
{
    paused = st;
    if(paused){
        if(autoBuyStatus){
            changeAutoBuy(false);
        }
        if(autoSellStatus){
            changeAutoSell(false);
        }
    }else{
        if(autoBuyStatus){
            changeAutoBuy(true);
        }
        if(autoSellStatus){
            changeAutoSell(true);
        }
        QString status = paused?"Starting":"Stopping";
        ply->getLog()->addLog("BAZAR",status+" Pause");
    }
}

void BazarManager::toggleAutosell()
{
    autoSellStatus = !autoSellStatus;
    if(autoSellStatus){
        if(!paused){
            changeAutoSell(true);
        }else{
            emit sellStatus(2);
        }
    }else{
        changeAutoSell(false);
    }
    QString status = autoSellStatus?"Starting":"Stopping";
    ply->getLog()->addLog("BAZAR",status+" Auto Sell");
}

void BazarManager::changeAutoBuy(bool status)
{
    if(status){
        if(!ply->getIsOldVersion()){
            ply->getMisc()->tryToOpenBasar();
        }
        int delay = Settings::getDelay("buy");//ply->getSettings()->getSetting("basar/buyDelay").toInt()*1000;
        autoBuyTimer->start(delay);
        ply->getLog()->addLog("BAZAR","Auto Buy Delay is "+ QString::number(delay)+ " msec +random");
        emit buyStatus(1);
    }else{
        if(!autoSellTimer->isActive()&&!ply->getIsOldVersion()){
            ply->getMisc()->closeBasar();
        }
        autoBuyTimer->stop();
        emit buyStatus(0);
    }
}

void BazarManager::changeAutoSell(bool status)
{
    if(status){
        if(!ply->getIsOldVersion()){
            ply->getMisc()->tryToOpenBasar();
        }
        int delay = Settings::getDelay("sell");//ply->getSettings()->getSetting("basar/sellDelay").toInt()*1000;
        autoSellTimer->start(delay);
        ply->getLog()->addLog("BAZAR","Auto Sell Delay is "+ QString::number(delay)+ " msec +random");
        if(ply->getSettings()->getSetting("basar/autoCollectSold").toBool()){
            autoCollectTimer->start(Settings::getDelay("checksold"));
            checkSold();
        }
        emit sellStatus(1);
    }else{
        if(!autoBuyTimer->isActive()&&!ply->getIsOldVersion()){
            ply->getMisc()->closeBasar();
        }
        autoSellTimer->stop();
        autoCollectTimer->stop();
        emit sellStatus(0);
    }
}

int BazarManager::getCheapest(int id)
{
    int transid = 0;
    int price = -1;
    QMapIterator<int, bItem*> i(blist);
    while (i.hasNext()) {
        i.next();
        if(i.value()->getItemid() == id){
            if(i.value()->getPrice()<price||price==-1){
                price = i.value()->getPrice();
                transid = i.key();
            }
        }
    }
    return transid;
}

bool BazarManager::getPaused() const
{
    return paused;
}

itemlist *BazarManager::getBuylist()
{
    return buylist;
}

itemlist *BazarManager::getSelllist()
{
    return selllist;
}

void BazarManager::rc_buy(QStringList packet)
{
    if(packet.size()>=8){
        if(packet[0].toInt()==1){
            bItem* item = buylist->getItemById(packet[1].toInt());
            if(item != nullptr){
                item->setAmmount(item->getAmmount()+packet[3].toInt());
                buylist->updateId(item->getItemid());
            }
        }
    }
}

void BazarManager::checkSold()
{
    if(!ply->getIsOldVersion()&&!ply->getMisc()->getBasarOpen()){
        return;
    }

    ply->getNet()->send("c_slist 0 0");
    ply->getLog()->addLog("BAZAR","Checking Sold Items!");
}

void BazarManager::saveItemLists()
{
    ply->getSettings()->changeSetting("basar/buylist",QVariant::fromValue(buylist->toList()));
    ply->getSettings()->changeSetting("basar/selllist",QVariant::fromValue(selllist->toList()));
}

void BazarManager::loadItemLists()
{
    buylist->loadList(ply->getSettings()->getSetting("basar/buylist").value<QList<QList<int>>>());
    selllist->loadList(ply->getSettings()->getSetting("basar/selllist").value<QList<QList<int>>>());
}

void BazarManager::autobuy()
{
    int delay = Settings::getDelay("buy");
    autoBuyTimer->start(delay);
    if(buylist->count()>0){
        buyWhen(buylist->getNext());
    }
}

void BazarManager::autosell()
{
    //bool logmode = Settings::getGlobalSettings()->getSetting("logging/packets").toBool();
    int delay = Settings::getDelay("sell");//ply->getSettings()->getSetting("basar/sellDelay").toInt()*1000;
    autoSellTimer->start(delay);
    bool logmode = true;

    if(selllist->count()>0){
        if(!timeout->isValid()||timeout->hasExpired(2500)){
            bool checkIfEmpty = ply->getSettings()->getSetting("basar/collectIfCheaper").toBool();


            bItem* item = selllist->getNext();  //Get current selected item
            ply->getLog()->addLog("Bazar","Trying to sell "+NosStrings::getItemName(item->getItemid()));
            QPair<int,int> slot = ply->getInv()->getFullestStack(item->getItemid());  //get Position in inventory of current item
            if(slot==QPair<int,int>(0,0)&&!checkIfEmpty){
                if(logmode){
                    ply->getLog()->addLog("BAZAR:Sell","Item not available!");
                }
                return;
            }//check if you have the item
            int slotammount = ply->getInv()->getSlotCount(slot.first,slot.second);
            if(slotammount<item->getAmmount()&&!checkIfEmpty){
                if(logmode){
                    ply->getLog()->addLog("Bazar","Not enough Items in Inventory");
                }
                return;
            }//check if you have enough for selected sell stack
            searching = true;
            searchitem = item;
            type = 2;
            currSlot = slot;
            searchItem(item->getItemid());
            if(logmode){
                ply->getLog()->addLog("BAZAR","Trying to autosell "+QString::number(item->getItemid())+ " "+NosStrings::getItemName(item->getItemid()));
            }
        }else{
            if(logmode){
                ply->getLog()->addLog("BAZAR_TEST","FAIL 0");
            }
        }
    }
}

void BazarManager::autoCheckSold()
{
    checkSold();
    autoCollectTimer->start(Settings::getDelay("checksold"));
}

bool BazarManager::getAutoSellStatus() const
{
    return autoSellStatus;
}


bool BazarManager::getAutoBuyStatus() const
{
    return autoBuyStatus;
}

