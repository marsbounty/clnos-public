#include "player.h"
#include "playerui.h"
#include "logger.h"
#include "settings.h"
#include "map.h"
#include "networkmanager.h"
#include "settingsui.h"
#include "misc.h"
#include "bazarmanager.h"
#include "inventory.h"
#include "chatmng.h"
#include "gflogin.h"
#include "battlemanager.h"
#include "gfnetworklogin.h"

Player::Player(QString name, bool autologin, QObject *parent) :
    QObject(parent),
    profileLog(new Logger(name)),
    profileSettings(new Settings(name)),
    profileName(name),
    net(new NetworkManager(this)),
    map(new Map(this)),
    bazar(new BazarManager(this)),
    inv(new Inventory(this)),
    misc(new Misc(this)),
    chat(new chatmng(this,this)),
    bmanager(new BattleManager(this,this)),
    autologin(autologin)

{
    this->name = "";
    lvl = 0;
    jlvl = 0;
    gold = 0;
    id = 0;
    speed = 0;
    gold_start = -1;
    isDead = false;
    sett = nullptr;
    connected = false;
    hasMedal = false;
    klasse = 0;
    isOldVersion = false;
    QString username = profileSettings->getSetting("login/username").toString();
    QString password = profileSettings->getSetting("login/password").toString();
    int server = profileSettings->getSetting("login/server").toInt();
    botUi = new PlayerUI(this,0,username,password,server);
}

Player::~Player()
{
    getBazar()->saveItemLists();
    QString name = getSettings()->getName();
    if(Settings::getGlobalSettings()->getSetting("profiles/default").toString()!=name){
        getSettings()->saveSettings(name);
    }
    delete botUi;
    delete bmanager;
    delete bazar;
    emit remove(this);
    if(sett){
        delete sett;
    }
    delete inv;
    delete misc;
    delete map;
    delete net;
    delete profileSettings;
    delete profileLog;
    deleteLater();
}

PlayerUI *Player::getUI()
{
    return botUi;
}

void Player::login(QString username, QString password, QString ip, qint16 port, bool old_version, int server)
{
    profileLog->addLog("LOGIN",username+" *********** "+ip+" "+QString::number(port));
    if(server <= 8 && Settings::getGlobalSettings()->getSetting("experimentalLogin").toBool()){
        gfNetworkLogin* gf = new gfNetworkLogin(this,this);
        gf->startAuth(username,password,server);
    }else{
        isOldVersion = old_version;
        net->login(username,password,ip,port,old_version);
    }
}

Settings *Player::getSettings() const
{
    return profileSettings;
}

Logger *Player::getLog() const
{
    return profileLog;
}

NetworkManager *Player::getNet() const
{
    return net;
}

void Player::logout()
{
    net->logout();
    deleteLater();
}

void Player::resetGoldTracker()
{
    gold_start = -1;
}

void Player::startGFlogin()
{
    new gflogin(this,this);
}

void Player::gfLoginError(QString err)
{
    getUI()->showGFLoginError(err);
}

void Player::init()
{
    if(getSettings()->getSetting("misc/master").toString()!=""&&getSettings()->getSetting("misc/masterid").toInt()==0){
        getMisc()->startMasterSearch();
    }
    getMisc()->toggleFollower();
}

void Player::setName(const QString &value)
{
    name = value;
    getUI()->setUsername(value);
}

int Player::getLvl() const
{
    return lvl;
}

void Player::setLvl(int value, int value2)
{
    lvl = value;
    jlvl = value2;
    getUI()->setLvl(value,value2);
}

int Player::getJlvl() const
{
    return jlvl;
}

int Player::getGold() const
{
    return gold;
}

int Player::getGold_start() const
{
    return gold_start;
}

int Player::getId() const
{
    return id;
}

void Player::setId(int value)
{
    id = value;
}

Map *Player::getMap() const
{
    return map;
}

void Player::openSettings()
{
    if(!sett){
        sett = new SettingsUI(this);
    }else{
        sett->activateWindow();
    }
}

void Player::closeSettings()
{
    sett = nullptr;
}

Misc *Player::getMisc() const
{
    return misc;
}

int Player::getSpeed() const
{
    return speed;
}

void Player::setSpeed(int value)
{
    speed = value;
}

BazarManager *Player::getBazar() const
{
    return bazar;
}

Inventory *Player::getInv() const
{
    return inv;
}

bool Player::getConnected() const
{
    return connected;
}

void Player::setConnected(bool value)
{
    connected = value;
    if(value){
        emit connectionStatus(1);
    }else{
        emit connectionStatus(0);
    }
}

bool Player::getAutologin() const
{
    return autologin;
}

void Player::setAutologin(bool value)
{
    autologin = value;
}

QString Player::getServerName() const
{
    return serverName;
}

void Player::setServerName(const QString &value)
{
    serverName = value;
}

bool Player::getHasMedal() const
{
    return hasMedal;
}

void Player::setHasMedal(bool value)
{
    hasMedal = value;
}

int Player::getKlasse() const
{
    return klasse;
}

void Player::setKlasse(int value)
{
    klasse = value;
}

void Player::wearSP()
{
    if(getSettings()->getSetting("misc/wearSP").toBool()){
        getLog()->addLog("PLAYER","Trying to wear SP");
        getNet()->send("sl 0");
    }

}

chatmng *Player::getChat() const
{
    return chat;
}

bool Player::getIsOldVersion() const
{
    return isOldVersion;
}

void Player::setIsOldVersion(bool value)
{
    isOldVersion = value;
    getLog()->addLog("PLY","Crypt version updated");
}

void Player::setSkills(QStringList packet)
{
    availSkills.clear();
    foreach (QString skill, packet) {
        availSkills.append(skill.toInt());
    }
}

bool Player::hasSkill(int id)
{
    return availSkills.contains(id);
}

bool Player::getIsDead() const
{
    return isDead;
}

void Player::setIsDead(bool value)
{
    getLog()->addLog("PLY","Player is "+(isDead?QString("Dead"):QString("Alive")));
    isDead = value;
}

int Player::getMasterId()
{
    return getSettings()->getSetting("misc/masterid").toInt();
}

BattleManager *Player::getBmanager() const
{
    return bmanager;
}

QList<int> Player::getAvailSkills() const
{
    return availSkills;
}

QString Player::getName() const
{
    return name;
}

void Player::setGold(int value)
{
    gold = value;
    if(gold_start==-1){
        gold_start = value;
    }
    emit moneyUpdate();
    getUI()->setGold(value,value-gold_start);
}

