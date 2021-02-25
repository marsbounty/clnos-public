#include "misc.h"
#include "player.h"
#include "networkmanager.h"
#include "settings.h"
#include "map.h"
#include "logger.h"
#include <QTimer>
#include <QTime>
#include "bazarmanager.h"
#include "inventory.h"
#include "moneyui.h"
#include "stattracker.h"
#include "nosstrings.h"
#include "notificationsystem.h"
#include "battlemanager.h"

Misc::Misc(Player *ply,QObject *parent) : QObject(parent),ply(ply)
{
    connect(ply->getNet(),SIGNAL(recievedDlg(QStringList)),this,SLOT(recieveDlg(QStringList)));
    connect(ply->getNet(),SIGNAL(recievedQnaml(QStringList)),this,SLOT(recieveQnaml(QStringList)));
    isCollecting = false;
    icHelpTimer = new QTimer(this);
    craftTimer = new QTimer(this);
    delayT = new QTimer(this);
    collectTimer = new QTimer(this);
    collectTimer->setSingleShot(true);
    connect(collectTimer,SIGNAL(timeout()),this,SLOT(tryCollect()));
    connect(icHelpTimer,SIGNAL(timeout()),this,SLOT(icHelper()));
    connect(ply,SIGNAL(connectionStatus(int)),this,SLOT(rcvStatusUpdate(int)));
    connect(craftTimer,SIGNAL(timeout()),this,SLOT(craftingUse()));
    connect(ply->getInv(),SIGNAL(updateInv()),this,SLOT(invUpdate()));
    connect(delayT,SIGNAL(timeout()),this,SLOT(delayHelper()));
    connect(ply->getMap(),SIGNAL(mapChanged(int)),this,SLOT(mapChanged(int)));
    inIc = false;
    lastFollowerPos = QPoint(0,0);
    inSheep = false;
    killSkillUp = false;
    craftStatus = 0;
    delayCommand ="";
    delayVar = "";
    delaySteps = 0;
    delayCurrent = 0;
    AOTstatus = 0;
    followS = false;
    masterSearch = false;
    basarOpen = false;

}

Misc::~Misc()
{
    closeBasar();
    if(mui){
        delete(mui);
    }
}

int Misc::randInt(int low, int high)
{
    // Random number between low and high
    return qrand() % ((high + 1) - low) + low;
}

void Misc::joinIC()
{
    ply->getNet()->send("guri 508");
    ply->getNet()->send("#guri^506");
    //connect(ply->getMap(),SIGNAL(mapChanged(int)),this,SLOT(mapChanged(int)));
    pauseCrafting(true);
}

void Misc::joinSheep()
{
    ply->getNet()->send("#guri^514");
    //connect(ply->getMap(),SIGNAL(mapChanged(int)),this,SLOT(mapChanged(int)));
    pauseCrafting(true);
}

void Misc::recieveQnaml(QStringList packet)
{
    if(packet[1]=="#guri^506"){
        if(ply->getSettings()->getSetting("misc/joinIC").toBool()==true){
            joinIC();
            ply->getLog()->addLog("MISC","Joined IC");
        }else{
            ply->getLog()->addLog("MISC","IC started without you!");
        }
    }else if(packet[1]=="#guri^514"){
        if(ply->getSettings()->getSetting("misc/joinSheeps").toBool()==true){
            joinSheep();
            ply->getLog()->addLog("MISC","Trying to join sheepfarm");
        }else{
            ply->getLog()->addLog("MISC","Sheepfarm Raid started without you!");
        }
    }
}

void Misc::recieveDlg(QStringList packet)
{
    ply->getLog()->addLog("MISC","Requested Dialog "+packet.join(" "));
    if(packet[0].contains("pjoin")){
        if(ply->getSettings()->getSetting("misc/joinGroups").toBool()){
            ply->getNet()->send(packet[0]);
            ply->getLog()->addLog("MISC","Accepting Group Invite");
        }
    }else if(packet[0].contains("req_exc")){
        QString bankZero = "";
        if(!ply->getIsOldVersion()){
            bankZero = " 0";
        }

        int trd = ply->getSettings()->getSetting("misc/acceptTrades").toInt();
        if(trd==1){
            ply->getNet()->send(packet[0]);
            ply->getNet()->send("exc_list 0"+bankZero);
            ply->getLog()->addLog("MISC","Accepting Trade Invite");
        /*}else if(trd==3){
            QStringList temp = packet[0].split("^");
            if(ply->getMap()->getPlayerNameById(temp[2].toInt())==ply->getSettings()->getSetting("misc/master")){
                ply->getNet()->send(packet[0]);
                ply->getNet()->send("exc_list 0"+bankZero);
                ply->getLog()->addLog("MISC","Accepting Trade Invite by Master");
            }
        }else if(trd == 4){
            QStringList temp = packet[0].split("^");
            if(ply->getMap()->getPlayerNameById(temp[2].toInt())==ply->getSettings()->getSetting("misc/master")){
                ply->getNet()->send(packet[0]);
                ply->getNet()->send("exc_list "+QString::number(ply->getGold()>5000?ply->getGold()-5000:0)+bankZero);
                ply->resetGoldTracker();
                ply->getLog()->addLog("MISC","Accepting Trade Invite by Master giving Money");
            }*/
        }else if(trd >= 2){
            QStringList temp = packet[0].split("^");
            if(ply->getMap()->getPlayerNameById(temp[2].toInt())==ply->getSettings()->getSetting("misc/master")){
                ply->getNet()->send(packet[0]);
                int gold = ply->getSettings()->getSetting("loot/gold").toInt();
                int backup = ply->getSettings()->getSetting("misc/goldbuffer").toInt();
                int request = ply->getGold()-backup;
                if(gold!=-1){
                    request = gold<request?gold:request;
                }
                request = request >=0?request:0;
                QList<QList<int>> list = ply->getSettings()->getSetting("loot/items").value<QList<QList<int>>>();
                QString items = "";
                foreach (QList<int> item, list) {
                    QPair<int,int> pos = ply->getInv()->getFullestStack(item.at(0));
                    if(pos!=QPair<int,int>(0,0)){
                        int ammount = item.at(1);
                        int sammount = ply->getInv()->getSlotCount(pos.first,pos.second);
                        if(item.at(1)!=-1){
                            ammount = ammount<sammount?ammount:sammount;
                        }else{
                            ammount = sammount;
                        }
                        items.append(" "+QString::number(pos.first)+" "+QString::number(pos.second)+" "+QString::number(ammount));
                    }

                }
                ply->getNet()->send("exc_list "+QString::number(request)+bankZero+items);
                ply->resetGoldTracker();
                ply->getLog()->addLog("MISC","Accepting Trade Invite by Master giving Money");
            }
        }
    }else if(packet[0].contains("revival")){
        ply->getNet()->send(packet[0]);
    }
}

void Misc::mapChanged(int id)
{
    if(id == 2004){
        inIc = true;
        ply->getBazar()->pause(true);
        emit icStatus(1);
        connect(ply->getMap(),SIGNAL(portalSpawned(int,QString,int,int)),this,SLOT(portalSpawned(int,QString,int,int)));
        if(Settings::getGlobalSettings()->getSetting("ic/move").toBool()){
            //int basedelay = Settings::getGlobalSettings()->getSetting("ic/autoDelay").toInt()*1000;
            int delay = Settings::getDelay("ic");//randInt(basedelay-500,basedelay+500);
            icHelpTimer->start(delay);
            ply->getLog()->addLog("IC","Move delay set to "+ QString::number(delay));
            QTimer::singleShot(2000+Settings::getDelay("other"),this,SLOT(icHelper()));
        }else{
            ply->getLog()->addLog("IC","Moving disabled!");
        }
        stattracker::getTracker()->icJoined();
    }else if(id == 2009){
        ply->getBazar()->pause(true);
        ply->getLog()->addLog("MISC","Joined sheepfarm");
        inSheep = true; 
    }else if(id == 2006){
        ply->getLog()->addLog("MISC","Joined Arena");
        if(AOTstatus>=1){
            //ply->getMap()->moveTo(QPoint(39,42));
            AOTstatus = 2 ;
            QTimer::singleShot(1000,this,SLOT(joinAOThelper()));
        }

    }else{
        if(inIc){
            ply->getLog()->addLog("IC","Ic left!");
            icHelpTimer->stop();
            emit icStatus(0);
            inIc = false;
            disconnect(this,SLOT(portalSpawned(int,QString,int,int)));
        }else if(inSheep){
            inSheep = false;
            killSkillUp = false;
            if(ply->getSettings()->getSetting("misc/createSheepRaids").toBool()){
                QTimer::singleShot(qrand()%180000+30000,this,SLOT(createSheepRaid()));
            }
        }else{
            if(Settings::getGlobalSettings()->getSetting("notifications/enabled").toBool()&&Settings::getGlobalSettings()->getSetting("notifications/mapchange").toBool()){
                notificationSystem::get().showMessage("Map Changed!",ply->getName()+" changed Map to "+NosStrings::getMapName(id));
            }
        }
        ply->getBazar()->pause(false);
        //disconnect(this,SLOT(mapChanged(int)));
        pauseCrafting(false);
    }
    if(isCollecting){
        isCollecting = false;
        if(delayT->isActive()){
            delayT->stop();
        }
    }
}

void Misc::portalSpawned(int id, QString name, int x, int y)
{
    Q_UNUSED(name)
    if(inIc&&id == 0){
        icHelpTimer->stop();
        QPoint dest = QPoint(x,y);
        ply->getMap()->moveTo(dest);
        ply->getLog()->addLog("IC","IC won! Leaving through portal!");
        stattracker::getTracker()->icWon();
    }
}

void Misc::attackClosestSheep()
{
    entity me = ply->getMap()->getPlayerById(ply->getId());
    entity enmy =ply->getMap()->getEnemyById(ply->getMap()->getClosestEnemy(me.x,me.y));

    ply->getLog()->addLog("SHEEP","Trying to attack "+QString::number(enmy.id));
    if(inSheep&&enmy.id!=0&&killSkillUp){
        if(abs(enmy.x-me.x)<2&&abs(enmy.y-me.y)<2&&!ply->getIsDead()){
            ply->getLog()->addLog("SHEEP","Sheep in range");
            ply->getNet()->send("sh 3 "+QString::number(enmy.id));
            ply->getLog()->addLog("SHEEP","My pos " +QString::number(me.x)+"/"+QString::number(me.y));
            ply->getLog()->addLog("SHEEP","Sheep pos " +QString::number(enmy.x)+"/"+QString::number(enmy.y));
            QTimer::singleShot(200,this,SLOT(attackClosestSheep()));
        }else{
            QTimer::singleShot(500,this,SLOT(attackClosestSheep()));
            ply->getMap()->moveTo(QPoint(enmy.x,enmy.y));
        }
    }
}

void Misc::setKillSkillUp(bool value)
{
    killSkillUp = value;
    ply->getLog()->addLog("SHEEP","Skill "+(value?QString("up"):QString("down")));
}

void Misc::skillMirrorHandler(int user,int target, int skill)
{
    if(!ply->getSettings()->getSetting("misc/mimicSkills").toBool())return;

    int ownerid = ply->getSettings()->getSetting("misc/masterid").toInt();
    if(user != ownerid) return;
    entity owner = ply->getMap()->getPlayerById(user);
    entity self = ply->getMap()->getPlayerById(ply->getId());
    entity targetE = ply->getMap()->getEnemyById(target);
    if(owner.sp!=16||self.sp!=16)return;
    switch (skill) {
    case 1051:
        ply->getNet()->send("u_s 2 1 "+QString::number(ply->getId()));
        break;
    case 1049:
        if(QPoint(self.x-targetE.x,self.y-targetE.y).manhattanLength()<6){
            ply->getNet()->send("u_s 0 3 "+QString::number(target));
        }

        break;
    default:
        return;
        break;
    }

}

void Misc::tryToOpenBasar()
{
    if(basarOpen){
        return;
    }
    if(ply->getIsOldVersion()){
        return;
    }
    if(ply->getHasMedal()){
        ply->getNet()->send("c_skill");
    }else{
        entity basarLady = ply->getMap()->getNpcByType(793);
        if(basarLady.name == ""){
            ply->getLog()->addLog("MISC","Cannot open Basar. No Basar Lady on Map");
            ply->getBazar()->changeAutoBuy(false);
            ply->getBazar()->changeAutoSell(false);
            return;
        }
        entity player = ply->getMap()->getPlayerById(ply->getId());
        QPoint dist = QPoint(basarLady.x,basarLady.y)-QPoint(player.x,player.y);
        if(basarLady.name!=""){
            if(abs(dist.x())<=2&&abs(dist.y())<=2){
                ply->getNet()->send("n_run 60 0 2 "+QString::number(basarLady.id));
            }else{
                ply->getMap()->moveTo(QPoint(basarLady.x+(-1+qrand()%2),basarLady.y+(-1+qrand()%2)));
                QTimer::singleShot(Settings::getDelay("other"),this,SLOT(tryToOpenBasar()));
            }
        }
    }
}

void Misc::closeBasar()
{
    if(basarOpen){
        setBasarOpen(false);
        ply->getNet()->send("c_close 0");
    }
}

bool Misc::getIsCollecting() const
{
    return isCollecting;
}

void Misc::collectNextItem()
{
    if(curCol.isEmpty()){
        isCollecting = false;
        return;
    }
    if(curColPos>=curCol.size()-1){
        curColPos = 0;
    }
    int currid = curCol.at(curColPos);
    if(isCollecting){
        if(ply->getMap()->checkIfNpcExsist(currid)){
            ply->getNet()->send("npc_req 2 "+QString::number(currid));
            curColPos++;
        }
    }
}

bool Misc::getBasarOpen() const
{
    return basarOpen;
}

void Misc::setBasarOpen(bool value)
{
    basarOpen = value;
}

void Misc::joinAOT()
{
    AOTstatus = 0;
    if(ply->getLvl()<30){
        ply->getLog()->addLog("AOT","Cannot join AOT. Level < 30");
        return;
    }
    if(ply->getGold()< 500){
        ply->getLog()->addLog("AOT","Not enough Gold to join Arena. Gold < 500");
        return;
    }
    if(ply->getSettings()->getSetting("login/channel").toInt()!=2&&ply->getSettings()->getSetting("login/channel").toInt()!=3){
        ply->getLog()->addLog("AOT","This channel doesn't have AOT. Channel != 2 || 3");
        return;
    }
    if(ply->getMap()->getMapId()!=2006){
        ply->getLog()->addLog("AOT","Joining Arena");
        ply->getMisc()->joinArena();
        AOTstatus = 1;
    }else{
        AOTstatus = 2;
        QTimer::singleShot(4000,this,SLOT(joinAOThelper()));
    }
}

void Misc::joinAOThelper(){
    if(AOTstatus == 2){
        ply->getMap()->moveTo(QPoint(37,37));
        QTimer::singleShot(4000,this,SLOT(joinAOThelper()));
        AOTstatus = 3;
    }else{
        if(ply->getMap()->checkIfNpcExsist(13788)){
            ply->getNet()->send("npc_req 2 13788");
            ply->getNet()->send("n_run 135 0 2 13788");
        }
    }

}

void Misc::icHelper()
{
    if(Settings::getGlobalSettings()->getSetting("ic/staticPos").toInt()!=1){
        if(ply->getMap()->getPlayerCount()>7){
            ply->getMap()->moveTo(ply->getMap()->getPlayerMassPoint());
        }
    }else{
        icHelpTimer->stop();
        QPoint p = Settings::getGlobalSettings()->getSetting("ic/Pos").toPoint();
        ply->getMap()->moveTo(p);
        ply->getLog()->addLog("IC","Using static position "+ QString::number(p.x())+":"+QString::number(p.y()) +"!");
    }
}

void Misc::rcvStatusUpdate(int id)
{
    if(id==1){

    }
}

void Misc::craftingUse()
{
    if(ply->getInv()->getItemCount(1013)!=0){
        int veredeler = 0;
        if(ply->getInv()->getItemCount(1036)!=0){
            veredeler = 1036;
        }else if(ply->getInv()->getItemCount(1037)!=0){
            veredeler = 1037;
        }else if(ply->getInv()->getItemCount(1038)!=0){
            veredeler = 1038;
        }
        if(veredeler!=0){
            if(ply->getInv()->getFreeInvSlots()>0){
                ply->getLog()->addLog("MISC","Using "+QString::number(veredeler));
                ply->getInv()->useItem(veredeler);
            }else{
                ply->getLog()->addLog("CRAFTING","Inventory full! Pausing crafting.");
                craftTimer->stop();
            }
        }else{
            ply->getLog()->addLog("CRAFTING","Not enough Production Items! Pausing crafting.");
            craftTimer->stop();
        }
    }else{
        ply->getLog()->addLog("CRAFTING","Not enough Gillion Stones! Pausing crafting.");
        craftTimer->stop();
    }
}

void Misc::invUpdate()
{
    if(craftStatus==1){
        craftTimer->start(1000);
    }
}

void Misc::delayHelper()
{
    delayCurrent += delaySteps;
    if(delayCurrent<97){
        ply->getNet()->send("guri 5 1 "+QString::number(ply->getId())+" "+QString::number(delayCurrent)+" "+delayVar);
    }else{
        delayT->stop();
        ply->getNet()->send("guri 5 1 "+QString::number(ply->getId())+" 100 "+delayVar);
        ply->getNet()->send(delayCommand);
        if(isCollecting){
            collectNextItem();
        }
    }
}

void Misc::getMasterId(int id, QString name, int x, int y)
{
    //ply->getLog()->addLog("MISC","Checking if Master");
    if(name == ply->getSettings()->getSetting("misc/master").toString()){
        ply->getSettings()->changeSetting("misc/masterid",id);
        stopMasterSearch();
        ply->getLog()->addLog("MISC","Master id updated!");
    }
}


void Misc::startCrafting()
{
    craftStatus = 1;
    craftTimer->start(1000);
    ply->getLog()->addLog("MISC","Starting Crafting");
    emit craftingStatus(1);
}

void Misc::stopCrafting()
{
    craftStatus = 0;
    craftTimer->stop();
    ply->getLog()->addLog("MISC","Stopping Crafting");
    emit craftingStatus(0);
}

void Misc::pauseCrafting(bool value)
{
    if(value){
        if(craftStatus == 1){
            craftStatus = 2;
            ply->getLog()->addLog("MISC","Pausing Crafting");
            craftTimer->stop();
            emit craftingStatus(2);
        }
    }else{
        if(craftStatus == 2){
            craftStatus = 1;
            ply->getLog()->addLog("MISC","Unpausing Crafting");
            craftTimer->start(1000);
            emit craftingStatus(1);
        }
    }
}

void Misc::openMoneyUI()
{
    if(!mui){
        mui = new MoneyUI(ply);
    }else{
        mui->activateWindow();
    }

}

void Misc::toggleCrafting()
{
    switch (craftStatus) {
    case 1:
        stopCrafting();
        break;
    case 0:
        startCrafting();
        break;
    case 2:
        stopCrafting();
        break;
    default:
        break;
    }
}

void Misc::acceptTrade(QStringList packet)
{
    if(packet[2].toInt() != -1){
        ply->getNet()->send("req_exc 3");
        ply->getLog()->addLog("MISC","Accepting Trade offer!");
    }
}

void Misc::delay(QStringList packet)
{
    if(!delayT->isActive()){
        delaySteps = 100/(packet[0].toInt()/1000);
        delayVar = packet[1];
        delayCommand = packet[2];
        delayCurrent = 0;

        ply->getNet()->send("guri 2");
        ply->getNet()->send("guri 5 1 "+QString::number(ply->getId())+" "+QString::number(delayCurrent)+" "+delayVar);


        delayT->start(1000);
    }

}

void Misc::customDelay(int steps,QString packet,QString var)
{
    if(!delayT->isActive()){
        delaySteps = 100/steps;
        delayVar = var;
        delayCommand = packet;
        delayCurrent = 0;

        ply->getNet()->send("guri 2");
        ply->getNet()->send("guri 5 1 "+QString::number(ply->getId())+" "+QString::number(delayCurrent)+" "+delayVar);


        delayT->start(1000);
    }
}

void Misc::startMasterSearch()
{
    ply->getLog()->addLog("MISC","Starting Master Id search!");
    int id = ply->getMap()->getPlayerIdByName(ply->getSettings()->getSetting("misc/master").toString());
    if(id == 0){
        masterSearch = true;
        connect(ply->getMap(),SIGNAL(playerSpawned(int,QString,int,int)),this,SLOT(getMasterId(int,QString,int,int)));
    }else{
        ply->getLog()->addLog("MISC","Master Id found! "+ QString::number(id));
        ply->getSettings()->changeSetting("misc/masterid",id);
    }

}

void Misc::stopMasterSearch()
{
    if(masterSearch){
        ply->getLog()->addLog("MISC","Stopping Master Id search!");
        disconnect(ply->getMap(),SIGNAL(playerSpawned(int,QString,int,int)),this,SLOT(getMasterId(int,QString,int,int)));
    }
    masterSearch = false;
}

void Misc::toggleFollower()
{
    if(ply->getSettings()->getSetting("misc/followMaster").toBool()){
        if(!followS){
            ply->getLog()->addLog("MISC","Starting Follower!");
            connect(ply->getMap(),SIGNAL(playerMoved(int,int,int)),this,SLOT(follow(int,int,int)));
            connect(ply->getMap(),SIGNAL(playerDespawned(int)),this,SLOT(followPortal(int)));
        }
        followS = true;
    }else{
        if(followS){
            ply->getLog()->addLog("MISC","Stopping Follower!");
            disconnect(ply->getMap(),SIGNAL(playerMoved(int,int,int)),this,SLOT(follow(int,int,int)));
            disconnect(ply->getMap(),SIGNAL(playerDespawned(int)),this,SLOT(followPortal(int)));
        }
        followS = false;
    }
}

void Misc::joinArena()
{
    ply->getLog()->addLog("MISC","Joining Arena!");
    ply->getNet()->send("n_run 18");
}

void Misc::statHandler(QStringList packet)
{
    int maxHealth = packet.at(1).toInt();
    int health = packet.at(0).toInt();
    int percent = ply->getSettings()->getSetting("misc/autoHealPercent").toInt();
    QList<int> items = ply->getSettings()->getSetting("misc/healItems").value<QList<int>>();
    bool autoHeal = ply->getSettings()->getSetting("misc/autoHeal").toBool();
    if(health<=maxHealth/100*percent&&autoHeal&&!items.isEmpty()){
        foreach (int i, items) {
            if(ply->getInv()->getItemCount(i)!=0){
                ply->getInv()->useItem(i);
                return;
            }
            ply->getLog()->addLog("MISC","No Healing Item found. RIP");
        }
    }
}

void Misc::npcReqHandler(QStringList packet)
{
    switch (packet.at(2).toInt()) {
    case 17:
        ply->getNet()->send("n_run 17 0 1 "+packet[1]);
        ply->getNet()->send("#arena^0^1");
        break;
    default:
        break;
    }
}

void Misc::useSkill(int id, int cd, int target)
{
    int t = target;
    if(t == 0){
        t = ply->getId();
    }

    if(skillcd.contains(id)){
        if(skillcd[id].elapsed()/1000>=cd){
            ply->getLog()->addLog("SKILL","Used skill with id: "+QString::number(id));
            skillcd[id] = QTime::currentTime();
            ply->getNet()->send("u_s "+QString::number(id)+" 1 "+QString::number(t));
        }else{
            ply->getLog()->addLog("SKILL","Skill is on cooldown: "+QString::number(id));
        }
    }else{
        skillcd.insert(id,QTime::currentTime());
        ply->getLog()->addLog("SKILL","Used skill with id: "+QString::number(id));
        ply->getNet()->send("u_s "+QString::number(id)+" 1 "+QString::number(t));
    }
}

bool Misc::isBusy()
{
    if(isCollecting){
        return true;
    }else if(basarOpen){
        return true;
    }
    return delayT->isActive();
}

QStringList Misc::getCollectables()
{
    QStringList ret;
    QMap<int,entity> npcs = ply->getMap()->getNpcMap();
    entity me = ply->getMap()->getPlayerById(ply->getId());
    foreach(entity npc,npcs){

        int dist = (QPoint(npc.x,npc.y)-QPoint(me.x,me.y)).manhattanLength();
        //ply->getLog()->addLog("TEST",npc.name+" "+QString::number(dist));
        if(dist<=3){
            if(npc.name == "858"){
                ret.append("Fragrant Grass");
            }else if(npc.name == "881"){
                ret.append("Ore");
            }else if(npc.name == "880"){
                ret.append("Wheat");
            }else if(npc.name == "862"){
                ret.append("Blossom Tree");
            }else if(npc.name == "2365"){
                ret.append("Carrot (Summer Event)");
            }
        }
    }
    return ret;
}

void Misc::startCollecting()
{
    curCol = getCollectableIds();
    curColPos = 0;
    isCollecting = true;
    collectNextItem();
}

void Misc::stopCollecting()
{
    isCollecting = false;
}

void Misc::funRaidStatusPacket(QStringList packet)
{
    ply->getLog()->addLog("TEST","Raid Status: "+packet.join(" "));
    if(packet.at(0).toInt()==2&&packet.at(1).toInt()==1&&!inSheep){
        ply->getLog()->addLog("MISC","Not enough players for Sheep Raid");
        ply->getBazar()->pause(false);
        //disconnect(this,SLOT(mapChanged(int)));
        pauseCrafting(false);
    }
}

void Misc::umiHandler(QStringList packet)
{
    foreach (QString item, packet) {
        int id = item.split(".").at(0).toInt();
        ply->getLog()->addLog("MISC","Player has active "+NosStrings::getItemName(id));
        if(id==5060||id==5061||id==5062||id == 9066 || id == 9067 || id == 9068){
            ply->getLog()->addLog("MISC","Player has NosBazar Medal");
            ply->setHasMedal(true);
        }
    }
}

void Misc::qnaHandler(QStringList packet)
{
    if(packet[0].contains("rd^")){
        if(ply->getSettings()->getSetting("misc/acceptRaidRequest").toBool()){
            ply->getNet()->send(packet.at(0));
        }
    }
}

void Misc::rlHandler(QStringList packet)
{
   if(packet.first().toInt()!=0&&packet.first().toInt()!=3)return;
   packet.removeFirst();
   QList<raid> raidList;
   foreach (QString raidS, packet) {
       QStringList raid = raidS.split(".");
        int id = raid.at(0).toInt();
        QString owner = raid.at(3);
        int plyCount = raid.at(8).toInt();
        raidList.append({id,owner,plyCount});
   }
   emit raidListUpdate(raidList);
}

void Misc::guriHandler(QStringList packet)
{
    if(packet.at(0)=="1"&&packet.at(1)=="5"){
        if(ply->getMap()->checkIfPlayerExsist(ply->getMasterId())){
            entity master = ply->getMap()->getPlayerById(ply->getMasterId());
            ply->getNet()->send(QString("guri 1 0 4 %1 %2").arg(master.x).arg(master.y));
        }
    }
    //guri 6 1 ***** 14 0
    if(packet.at(0)=="6"&&packet.at(1)=="1"&&packet.at(2).toInt()==ply->getMasterId()){
        if(!ply->getSettings()->getSetting("misc/mimicSkills").toBool())return;
        ply->getLog()->addLog("MISC","Paja skill by Master??");
        if(packet.at(3)=="14"){
            ply->getBmanager()->useSkill(801,ply->getId(),TargetType::self);
        }
    }
}

void Misc::createSheepRaid()
{
    ply->getLog()->addLog("SHEEP","Creating Sheep raid");
    ply->getInv()->useItem(5854);
}

void Misc::tryCollect()
{
    if(ply->getSettings()->getSetting("misc/autoCollect").toBool()&&!collectTimer->isActive()){
        entity me = ply->getMap()->getPlayerById(ply->getId());
        int id = ply->getMap()->getItemInRange(me.x,me.y);
        if(id != 0){
            ply->getNet()->send("get 1 "+QString::number(ply->getId())+" "+QString::number(id));
            collectTimer->start(Settings::getDelay("other"));
        }
    }
}

QList<int> Misc::getCollectableIds()
{
    QStringList cb = {"858","881","880","862","2365"};
    QList<int> ret;
    QMap<int,entity> npcs = ply->getMap()->getNpcMap();
    entity me = ply->getMap()->getPlayerById(ply->getId());
    foreach(entity npc,npcs){
        if((QPoint(npc.x,npc.y)-QPoint(me.x,me.y)).manhattanLength()<=3){
            if(cb.contains(npc.name)){
                ret.append(npc.id);
            }
        }
    }
    return ret;
}

void Misc::follow(int id, int x, int y)
{
    if(id == ply->getSettings()->getSetting("misc/masterid").toInt()){
        ply->getMap()->moveTo(QPoint(x+randInt(-1,1),y+randInt(-1,1)),Settings::getDelay("follow"));
        lastFollowerPos = QPoint(x,y);
    }
}

void Misc::followPortal(int id)
{
    if(id == ply->getSettings()->getSetting("misc/masterid").toInt()){
        ply->getLog()->addLog("FOLLOWER","Following Player disappeared!");

        if(ply->getMap()->isPortal(lastFollowerPos)){
            entity portal = ply->getMap()->getPortalByPos(lastFollowerPos);
            ply->getMap()->moveTo(QPoint(portal.x,portal.y));
            ply->getLog()->addLog("FOLLOWER","FPlayer used Portal using it too.");
        }else{
            entity me = ply->getMap()->getPlayerById(ply->getId());
            int portalid = ply->getMap()->getClosestPortal(me.x,me.y);
            entity portal = ply->getMap()->getPortalById(portalid);
            if((QPoint(me.x,me.y)-QPoint(portal.x,portal.y)).manhattanLength()<7){
                ply->getLog()->addLog("FOLLOWER","Following through Portal!");
                ply->getMap()->moveTo(QPoint(portal.x,portal.y));
            }
        }


    }
}

void Misc::startKillingSheep()
{
    ply->getLog()->addLog("SHEEP","Trying to start killing sheeps.");
    if(!killSkillUp){
        killSkillUp = true;
        QTimer::singleShot(qrand()%5000,this,SLOT(attackClosestSheep()));
    }
}

int Misc::getCraftStatus() const
{
    return craftStatus;
}
