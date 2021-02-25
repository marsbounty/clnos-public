#include "networkmanager.h"
#include "networker.h"
#include "logger.h"
#include "player.h"
#include "playerui.h"
#include "map.h"
#include "bazarmanager.h"
#include "nosstrings.h"
#include <QMetaObject>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QTimer>
#include "inventory.h"
#include "misc.h"
#include "akt4status.h"
#include "settings.h"
#include "chatmng.h"
#include <QUuid>
#include <QSettings>
#include "delayedpacket.h"
#include "battlemanager.h"

NetworkManager::NetworkManager(Player *ply, QObject *parent) : QObject(parent)
{
    net = new networker(this);
    this->ply = ply;
    dcExpected = false;
    state = 0;
    fillPacketTypes();
    keepAlive = new QTimer();
    runtime = 0;
    isSwitchingServer = false;
    charid = 0;
    serverip = "";
    port = 0;
    langid = 0;
    old_version=false;
    gfVersion = false;
    qsrand(uint(QDateTime::currentMSecsSinceEpoch() & 0xFFFF));
    generateHwId();
    connect(keepAlive, SIGNAL(timeout()), this, SLOT(sendKeepAlive()));
}

NetworkManager::~NetworkManager()
{
    delete net;
}



void NetworkManager::login(QString username, QString password, QString ip, qint16 port,bool oldversion)
{
    this->username = username;
    this->password = password;
    this->old_version = oldversion;
    ply->setIsOldVersion(oldversion);
    if(old_version){
        ply->getLog()->addLog("NET","Using old Nostale Crypto!");
    }
    net->connectTo(ip,port,true);
    setState(1);
}

void NetworkManager::connectToGame(QString ip, qint16 port)
{
    net->connectTo(ip,port);
    setState(5);
}

void NetworkManager::connectionChanged(bool connected)
{
    if(connected){
        switch (state) {
        case 1:
            net->login(username,password,isSwitchingServer,old_version,langid);
            ply->getLog()->addLog("NET","Connected to Login Server");
            setState(2);
            break;
        case 5:     
            setState(6);
            net->login(username,password,isSwitchingServer,old_version,langid);
        default:
            break;
        }
    }else{
        if(state==3){
            setState(4);
        }else if(state == 6)
        {
            if(isSwitchingServer){
                connectToGame(serverip,port);
            }
        }
        if(!dcExpected){
            ply->getLog()->addLog("NET","Not Planned Disconnect");
        }
        if(keepAlive->isActive()){
            keepAlive->stop();
            runtime = 0;
        }
    }
    ply->setConnected(false);
    dcExpected = false;
}

void NetworkManager::selectChar(int id)
{
    charid = id;
    send("select "+QString::number(id));
}

void NetworkManager::logout()
{
    ply->getLog()->addLog("NET","Logging out!");
    keepAlive->stop();
    send("c_close");
    send("f_stash_end");
    dcExpected = true;
}

void NetworkManager::send(QString msg,bool log)
{
    emit packetLogSend(msg.split(" "));
    net->sendString(msg,log);
}

void NetworkManager::sendDelayed(QString msg,int delay,bool log)
{
    //emit packetLogSend(msg.split(" "));
    new delayedPacket(msg,delay,this,this);
}

void NetworkManager::recievePacket(QStringList packet)
{
    //getLog()->addLog("PLY","Crypt version " + (old_version?QString("old"):QString("old")));
    //getLog()->addLog("PLY","Crypt version2 " + (ply->getIsOldVersion()?QString("old"):QString("old")));
    emit packetLogRecv(packet);
    if(Settings::getGlobalSettings()->getSetting("logging/packets").toBool()){
        if(packet.at(0)!="mv"){
            ply->getLog()->addLog("PACKET",packet.join(" "));
        }
    }

    QString type = packet[0].toLower();

    if(packetTypes.contains(type)){
        packet.removeFirst();
        (this->*packetTypes.value(type))(packet);
    }else{
        //ply->getLog()->addLog("NET","Unhandled Packet "+type);
    }
    //ply->getLog()->addLog("PACKET",packet.join(" "));
}

Logger *NetworkManager::getLog()
{
    return ply->getLog();
}

QString NetworkManager::getPassword() const
{
    return password;
}

void NetworkManager::sendKeepAlive()
{
    runtime++;
    send("pulse "+QString::number(runtime*60)+" 0",false);
}

QString NetworkManager::getUid() const
{
    return uid;
}

bool NetworkManager::getGfVersion() const
{
    return gfVersion;
}

void NetworkManager::generateHwId()
{
    QString hwid=Settings::getGlobalSettings()->getSetting("hwid").toString();
    if(hwid==""||QUuid(hwid).version()!=4){
        QSettings NosSettings("HKEY_CURRENT_USER\\Software\\Gameforge4d\\TNTClient\\MainApp",QSettings::NativeFormat);
        QString nhwid = NosSettings.value("InstallationId","NO").toString();
        if(nhwid=="NO"){
            Settings::getGlobalSettings()->changeSetting("hwid",QUuid::createUuid().toString(QUuid::WithoutBraces));
        }else{
            Settings::getGlobalSettings()->changeSetting("hwid",nhwid);
        }
    }
}

bool NetworkManager::getIsSwitchingServer() const
{
    return isSwitchingServer;
}

void NetworkManager::timeout()
{
    logout();
    net->close();
    ply->getUI()->displayLoginError("Connection closed!");
}

void NetworkManager::gfLogin(QString id,int port,int locale)
{
    langid=locale;
    if(port == -1){


    int portId = Settings::getGlobalSettings()->getSetting("gfLoginRegion").toInt();
    QList<int> ports = {4000,4001,4002,4003,4004,4005,4006,4008,4007};

    if(portId==0||ports.size()<portId){
        ply->gfLoginError("Please set your GF Login Region in the Settings!");
        ply->getLog()->addLog("GF","GF Login Region not set!. Stopping login");
        return;
    }
    langid = portId;
    port = ports.at(portId-1);
    }
    ply->getLog()->addLog("GF","GF Mode on");
    ply->getLog()->addLog("GF","id: "+id);
    ply->getLog()->addLog("GF","port: "+QString::number(port));
    gfVersion = true;
    uid = id;
    serverip = "79.110.84.75";

//    this->username = username;
//    if(username==""){this->username="NONE_SESSION_ID";}
    this->port = port;
    this->password = "thisisgfmode";
    net->connectTo(serverip,port,true);
    setState(1);
}

int NetworkManager::getCharid() const
{
    return charid;
}

QString NetworkManager::getUsername() const
{
    return username;
}

void NetworkManager::setState(int i)
{
    ply->getLog()->addLog("NET","State: "+QString::number(i));
    state = i;
}

int NetworkManager::getLangid() const
{
    return langid;
}

void NetworkManager::fillPacketTypes()
{
    packetTypes.insert("fail",&NetworkManager::fail);
    packetTypes.insert("failc",&NetworkManager::fail);
    packetTypes.insert("info",&NetworkManager::info);
    packetTypes.insert("nstest",&NetworkManager::nstest);
    packetTypes.insert("clist_end",&NetworkManager::clist_end);
    packetTypes.insert("clist",&NetworkManager::clist);
    packetTypes.insert("ok",&NetworkManager::ok);
    packetTypes.insert("c_info",&NetworkManager::cinfo);
    packetTypes.insert("lev",&NetworkManager::lev);
    packetTypes.insert("mz",&NetworkManager::mz);
    packetTypes.insert("p_clear",&NetworkManager::pclear);
    packetTypes.insert("gold",&NetworkManager::gold);
    packetTypes.insert("in",&NetworkManager::in);
    packetTypes.insert("out",&NetworkManager::out);
    packetTypes.insert("mv",&NetworkManager::mv);
    packetTypes.insert("at",&NetworkManager::at);
    packetTypes.insert("qnaml",&NetworkManager::qnaml);
    packetTypes.insert("dlg",&NetworkManager::dlg);
    packetTypes.insert("shop",&NetworkManager::shop);
    packetTypes.insert("gp",&NetworkManager::gp);
    packetTypes.insert("cond",&NetworkManager::cond);
    packetTypes.insert("rc_blist",&NetworkManager::rc_blist);
    packetTypes.insert("rc_slist",&NetworkManager::rc_slist);
    packetTypes.insert("inv",&NetworkManager::inv);
    packetTypes.insert("ivn",&NetworkManager::inv);
    packetTypes.insert("rc_buy",&NetworkManager::rc_buy);
    packetTypes.insert("exc_list",&NetworkManager::exc_list);
    packetTypes.insert("fc",&NetworkManager::fc);
    packetTypes.insert("umi",&NetworkManager::umi);
    packetTypes.insert("delay",&NetworkManager::delay);
    packetTypes.insert("msg",&NetworkManager::msg);
    //packetTypes.insert("csp",&csp);
    packetTypes.insert("say",&NetworkManager::say);
    packetTypes.insert("spk",&NetworkManager::spk);
    packetTypes.insert("npc_req",&NetworkManager::npc_req);
    packetTypes.insert("ski",&NetworkManager::ski);
    packetTypes.insert("drop",&NetworkManager::drop);
    packetTypes.insert("get",&NetworkManager::get);
    packetTypes.insert("bsinfo",&NetworkManager::bsinfo);
    packetTypes.insert("die",&NetworkManager::die);
    packetTypes.insert("sh_o",&NetworkManager::sh_o);
    packetTypes.insert("revive",&NetworkManager::revive);
    packetTypes.insert("sh_c",&NetworkManager::sh_c);
    packetTypes.insert("wopen",&NetworkManager::wopen);
    packetTypes.insert("stat",&NetworkManager::stat);
    packetTypes.insert("qna",&NetworkManager::qna);
    packetTypes.insert("c_mode",&NetworkManager::c_mode);
    packetTypes.insert("ct",&NetworkManager::ct);
    packetTypes.insert("mapout",&NetworkManager::mapout);
    packetTypes.insert("rl",&NetworkManager::rl);
    packetTypes.insert("tp",&NetworkManager::tp);
    packetTypes.insert("ta_m",&NetworkManager::ta_m);
    packetTypes.insert("sr",&NetworkManager::sr);
    packetTypes.insert("cancel",&NetworkManager::cancel);
    packetTypes.insert("st",&NetworkManager::st);
    packetTypes.insert("guri",&NetworkManager::guri);
    packetTypes.insert("su",&NetworkManager::su);
}

void NetworkManager::fail(QStringList packet)
{
    setState(0);
    QString error = packet.join(" ");
    ply->getLog()->addLog("ERROR ",error);
    ply->getUI()->displayLoginError(error);
    dcExpected = true;
}

void NetworkManager::info(QStringList packet)
{
    Q_UNUSED(packet)
    if(!dcExpected&&!ply->getConnected()&&net->getIsLoginServer()){
        ply->getLog()->addLog("NET","Recieved info packet: "+packet.join(" "));
        logout();
        net->close();
        ply->getUI()->displayLoginError("Server full!");
    }
}

void NetworkManager::nstest(QStringList packet)
{
    dcExpected=true;
    net->close();
    ply->getLog()->addLog("NET","Recieved NsTeSt Packet");
    QMap<int,server> serverlist;
    setState(3);
    int skip;
    if(old_version){
        net->setSessionID(packet[0].toInt());
        skip = 1;
    }else if(gfVersion){
        net->setSessionID(packet[4].toInt());
        this->username = packet.at(2);
        skip = 5;
    }else{
        net->setSessionID(packet[1].toInt());
        skip = 2;
    }
    ply->getLog()->addLog("NET","Userid: "+packet[0]);
    for(int i = skip;i<packet.size()-1;i++){
        QStringList temp = packet[i].split(":");
        if(temp.count()==4){
            QStringList temp2 = temp[3].split(".");
            if(!serverlist.contains(temp2[0].toInt())){
                server newSrv;
                newSrv.name = temp2[2];
                newSrv.ip = temp[0];
                newSrv.channels.insert(temp2[1].toInt(),temp[1].toInt());
                serverlist.insert(temp2[0].toInt(),newSrv);
                //ply->getLog()->addLog("NET","Server id "+QString::number(temp2[0].toInt()));
            }else{
                serverlist[temp2[0].toInt()].channels.insert(temp2[1].toInt(),temp[1].toInt());
            }
        }
    }
    ply->getLog()->addLog("NET",packet.join(" "));
    ply->getUI()->showServerSelection(serverlist);
}

void NetworkManager::clist(QStringList packet)
{
    ply->getLog()->addLog("NET","Recieved Char");
    ply->getUI()->addChar(packet[1],packet[0].toInt());
}

void NetworkManager::clist_end(QStringList)
{
    ply->getLog()->addLog("NET","Showing Char Selection");
    ply->getUI()->showWindow(2);
    net->sendString("c_close");
    net->sendString("f_stash_end");
}

void NetworkManager::ok(QStringList)
{
    send("game_start");
    send("lbs 0");
    keepAlive->start(60000);
    if(isSwitchingServer){
        isSwitchingServer=false;
    }
    ply->wearSP();
}

void NetworkManager::cinfo(QStringList packet)
{
    QString name = packet[0];
    int id = packet[5].toInt();
    int klasse = packet[10].toInt();
    int sp = packet.at(13).toInt();
    ply->setName(name);
    ply->setId(id);
    ply->setKlasse(klasse);
    ply->getUI()->setCharImage(klasse);
    ply->init();
    ply->getMap()->addPlayer(id,name,0,0,sp);
}

void NetworkManager::lev(QStringList packet)
{
    int lvl = packet[0].toInt();
    int jlvl = packet[2].toInt();
    ply->setLvl(lvl,jlvl);
}

void NetworkManager::mz(QStringList packet)
{
    logout();
    ply->getLog()->addLog("NET","Switching Server");
    serverip = packet[0];
    port = packet[1].toInt();
    isSwitchingServer = true;
}

void NetworkManager::pclear(QStringList)
{
    ply->getUI()->showWindow(4);
}

void NetworkManager::gold(QStringList packet)
{
    int gold = packet[0].toInt();
    ply->setGold(gold);
}

void NetworkManager::in(QStringList packet)
{
    int entityType = packet[0].toInt();
    if(entityType == 1){
        QString name = packet[1];
        int x = packet[4].toInt();
        int y =  packet[5].toInt();
        int id = packet[3].toInt();
        int sp = packet.at(22).toInt();
        ply->getMap()->addPlayer(id,name,x,y,sp);
    }else if(entityType == 2){
        QString name = packet[1];
        int x = packet[3].toInt();
        int y =  packet[4].toInt();
        int id = packet[2].toInt();
        int sp = packet.at(1).toInt();
        //ply->getLog()->addLog("MAP","Adding npc with id "+QString::number(id));
        ply->getMap()->addNpc(id,name,x,y,sp);
    }else if(entityType == 3){
        int id = packet.at(2).toInt();
        int x = packet.at(3).toInt();
        int y = packet.at(4).toInt();
        ply->getMap()->addEnemy(id,x,y);
    }

}

void NetworkManager::out(QStringList packet)
{
    int entityType = packet[0].toInt();
    int id = packet[1].toInt();

    switch (entityType) {
    case 1:
        ply->getMap()->removePlayer(id);
        break;
    case 3:
        ply->getMap()->removeEnemy(id);
        break;
    case 9:
        ply->getMap()->removeItem(id);
        break;
    default:
        break;
    }
}

void NetworkManager::mv(QStringList packet)
{
    int entityType = packet[0].toInt();
    int id = packet[1].toInt();
    int x = packet[2].toInt();
    int y = packet[3].toInt();
    if(entityType==1){
        ply->getMap()->updatePlayerPos(id,x,y);
    }else if(entityType ==3){
        ply->getMap()->updateEnemyPos(id,x,y);
    }
}

void NetworkManager::at(QStringList packet)
{
    int mapid = packet[1].toInt();
    int x = packet[2].toInt();
    int y = packet[3].toInt();
    ply->getMap()->setMapId(mapid);
    //ply->getMap()->addPlayer(ply->getId(),ply->getName(),x,y);
    ply->getMap()->updatePlayerPos(ply->getId(),x,y);
    ply->setConnected(true);
}

void NetworkManager::qnaml(QStringList packet)
{
    emit recievedQnaml(packet);
}

void NetworkManager::dlg(QStringList packet)
{
    emit recievedDlg(packet);
}

void NetworkManager::shop(QStringList packet)
{
    if(packet.count()>5){
        int id = packet[1].toInt();
        QString name = packet[5];
        ply->getMap()->setNPCName(id,name);
    }
}

void NetworkManager::gp(QStringList packet)
{
    int id = packet[4].toInt();
    int x = packet[0].toInt();
    int y = packet[1].toInt();
    QString dest = NosStrings::getMapName(packet[2].toInt());
    ply->getMap()->addPortal(id,dest,x,y);
}

void NetworkManager::cond(QStringList packet)
{
    int id = packet[1].toInt();
    if(id==ply->getId()){
        ply->setSpeed(packet[4].toInt());
    }
}

void NetworkManager::rc_blist(QStringList packet)
{
    ply->getBazar()->recieveBlist(packet);
}

void NetworkManager::rc_slist(QStringList packet)
{
    ply->getLog()->addLog("NET","Recieved sList");
    ply->getBazar()->recieveSlist(packet);
}

void NetworkManager::inv(QStringList packet)
{
    ply->getInv()->addPacket(packet);
}

void NetworkManager::rc_buy(QStringList packet)
{
    ply->getBazar()->rc_buy(packet);
}

void NetworkManager::exc_list(QStringList packet)
{
    ply->getMisc()->acceptTrade(packet);
}

void NetworkManager::fc(QStringList packet)
{
    akt4status::updateStat(packet,ply);
}

void NetworkManager::umi(QStringList packet)
{
    ply->getMisc()->umiHandler(packet);
}

void NetworkManager::delay(QStringList packet)
{
    ply->getMisc()->delay(packet);
}

void NetworkManager::msg(QStringList packet)
{
    ply->getChat()->addChat(packet,2);
}

void NetworkManager::csp(QStringList packet)
{
    ply->getChat()->addChat(packet,1);
}

void NetworkManager::say(QStringList packet)
{
    if(packet.at(2).toInt()==6){
        ply->getChat()->addChat(packet,4);
    }else{
        ply->getChat()->addChat(packet,0);
    }
    //ply->getLog()->addLog("CHAT","Recieved Msg!");
}

void NetworkManager::spk(QStringList packet)
{
    if(packet.at(2).toInt()==3){
        ply->getChat()->addChat(packet,3);
    }
    ply->getLog()->addLog("CHAT","Recieved Whisper!");
}

void NetworkManager::npc_req(QStringList packet)
{
    ply->getMisc()->npcReqHandler(packet);
}

void NetworkManager::ski(QStringList packet)
{
    ply->setSkills(packet);
    ply->getBmanager()->loadAASkillId();
}

void NetworkManager::drop(QStringList packet)
{
    ply->getMap()->addItem(packet.at(1).toInt(),packet.at(2).toInt(),packet.at(3).toInt());
    ply->getMisc()->tryCollect();
}

void NetworkManager::get(QStringList packet)
{
    ply->getMap()->removeItem(packet.at(2).toInt());
}

void NetworkManager::bsinfo(QStringList packet)
{
    ply->getMisc()->funRaidStatusPacket(packet);
}

void NetworkManager::die(QStringList packet)
{
    short type = packet.at(0).toShort();
    int id = packet.at(1).toInt();
    switch (type) {
    case 1:
        if(id==ply->getId()){
            ply->setIsDead(true);
        }
        break;
    case 3:
        ply->getMap()->removeEnemy(id);
        break;
    default:
        break;
    }
}

void NetworkManager::sh_o(QStringList packet)
{
    Q_UNUSED(packet);
    ply->getMisc()->startKillingSheep();
}

void NetworkManager::revive(QStringList packet)
{
    short type = packet.at(0).toShort();
    int id = packet.at(1).toInt();
    switch (type) {
    case 1:
        if(id==ply->getId()){
            ply->setIsDead(false);
        }
        break;
    default:
        break;
    }
}

void NetworkManager::sh_c(QStringList packet)
{
    Q_UNUSED(packet)
    ply->getMisc()->setKillSkillUp(false);
}

void NetworkManager::wopen(QStringList packet)
{
    if(packet.at(0).toInt()==32){
        ply->getMisc()->setBasarOpen(true);
    }
}

void NetworkManager::stat(QStringList packet)
{
    ply->getMisc()->statHandler(packet);
}

void NetworkManager::qna(QStringList packet)
{
    ply->getMisc()->qnaHandler(packet);
}

void NetworkManager::c_mode(QStringList packet)
{
    ply->getMap()->setPlayerSp(packet.at(1).toInt(),packet.at(2).toInt());
}

void NetworkManager::ct(QStringList packet)
{
    //ply->getMisc()->skillMirrorHandler(packet.at(1).toInt(),packet.at(3).toInt(),packet.at(6).toInt());
    ply->getBmanager()->mirrorSkill(packet);
}

void NetworkManager::mapout(QStringList packet)
{
    Q_UNUSED(packet);
    ply->getMap()->mapout();
}

void NetworkManager::rl(QStringList packet)
{
    ply->getMisc()->rlHandler(packet);
}

void NetworkManager::ta_m(QStringList packet)
{
    Q_UNUSED(packet)
    sendDelayed("escape",rand()%1000);
}

void NetworkManager::tp(QStringList packet)
{
    if(packet.at(0)!="1"){return;}
    int id = packet.at(1).toInt();
    int x = packet.at(2).toInt();
    int y = packet.at(3).toInt();
    //ply->getLog()->addLog("REMOVE","Recieved tp packet");
    ply->getMap()->updatePlayerPos(id,x,y);

}

void NetworkManager::sr(QStringList packet){
    ply->getBmanager()->removeCooldown(packet.at(0).toInt());
}

void NetworkManager::cancel(QStringList packet){
    ply->getBmanager()->cancelSkill(packet.at(1).toInt());
}

void NetworkManager::st(QStringList packet){
    ply->getBmanager()->stPacketHandler(packet);
}

void NetworkManager::guri(QStringList packet)
{
    ply->getMisc()->guriHandler(packet);
}

void NetworkManager::su(QStringList packet)
{
    if(packet.at(2).toInt()==3){
        if(packet.at(10).toInt()==0){
            ply->getMap()->removeEnemy(packet.at(3).toInt());
        }
    }
}
