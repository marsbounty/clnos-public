#include "playercontroller.h"
#include "newbotdialog.h"
#include "playerui.h"
#include "logger.h"
#include "settings.h"
#include <QMessageBox>
#include "player.h"
#include <QStringList>
#include <QString>
#include "notificationsystem.h"
#include <QNetworkProxy>
#include <QTimer>
#include <session.h>
#include <adtui.h>
#include "battlemanager.h"
#include "networkmanager.h"

PlayerController* PlayerController::mainPC = nullptr;

PlayerController::PlayerController(QObject *parent) : QObject(parent)
{
    Settings::getGlobalSettings()->updateProxy();
    totalEarned=Settings::getGlobalSettings()->getSetting("stats/totalEarned").toInt();
    queueRunning = false;
}

PlayerController::~PlayerController()
{
    for(int i = 0; i < players.count();i++){
        players.value(i)->logout();
        players.removeAt(i);
    }
}

void PlayerController::newBot(bool load)
{
    if(!load){
        createDefault();
    }else{
        NewBotDialog *dialog = new NewBotDialog(Settings::getAllConfigNames());
        connect(dialog,SIGNAL(clicked(QString,bool,bool)),this,SLOT(dialogClicked(QString,bool,bool)));
        dialog->show();
    }
}

void PlayerController::createDefault()
{
    addBot(Settings::getGlobalSettings()->getSetting("profiles/default").toString(),false);
}

QList<Player *> &PlayerController::getPlayerList()
{
    return players;
}

void PlayerController::saveCurrentSession()
{
    QStringList lastSession;
    for(int i = 0; i < players.count();i++){
        lastSession.append(players.value(i)->getSettings()->getName());
        Logger::getGlobalLogger()->addLog("BOT CTRL","Current Session += "+players.value(i)->getSettings()->getName());
    }
    session *sessDialog = new session(false,lastSession);
    sessDialog->show();

}

void PlayerController::backupCurrentSession(){
        QStringList lastSession;
        for(int i = 0; i < players.count();i++){
            lastSession.append(players.value(i)->getSettings()->getName());
            Logger::getGlobalLogger()->addLog("BOT CTRL","Current Session += "+players.value(i)->getSettings()->getName());
        }
        Settings::getGlobalSettings()->changeSetting("profiles/lastSession",lastSession);
        Settings::getGlobalSettings()->saveSettings("clnos");
}

void PlayerController::loadLastSession()
{ 
    if(Settings::getGlobalSettings()->getSetting("profiles/lastSession").isValid()){
        creationQueue.append(Settings::getGlobalSettings()->getSetting("profiles/lastSession").toStringList());
        if(queueRunning)return;
        createNext();
    }

}

void PlayerController::loadSession()
{
    session *sessDialog = new session(true,{});
    sessDialog->show();
    connect(sessDialog,SIGNAL(load(QStringList)),this,SLOT(recvSessDialog(QStringList)));
}

void PlayerController::showAotUI()
{
    adtui *adtDialog = new adtui(players);
    adtDialog->show();
}

void PlayerController::createNext()
{
    queueRunning = true;
    addBot(creationQueue.first(),true);
    creationQueue.removeFirst();
    int delay = Settings::getDelay("login");
    if(!creationQueue.isEmpty()){
        QTimer::singleShot(delay,this,SLOT(createNext()));
    }else{
        queueRunning = false;
    }
}

PlayerController *PlayerController::getPC()
{
    return mainPC;
}

void PlayerController::setPC(PlayerController *pc)
{
    mainPC = pc;
}

bool PlayerController::entityIsBot(QString name)
{
    foreach (Player* ply, players) {
        if(ply->getName()== name){
            return true;
        }
    }
    return false;
}

int PlayerController::getCurrentGold()
{
    int totalAmmount = 0;
    foreach (Player* ply, players) {
        totalAmmount+= ply->getGold();
    }
    return totalAmmount;
}

int PlayerController::getEarnedGold()
{
    int earnedAmmount = 0;
    foreach (Player* ply, players) {
        earnedAmmount+=(ply->getGold()-ply->getGold_start());
    }
    return earnedAmmount;
}

int PlayerController::getTotalEarnedGold()
{
    int earnedAmmount = totalEarned;
    foreach (Player* ply, players) {
        earnedAmmount+=(ply->getGold()-ply->getGold_start());
    }
    return earnedAmmount;
}

void PlayerController::useAllBuffs()
{
    foreach (Player* ply, players) {
        ply->getBmanager()->useAllBuffs();
    }
}

void PlayerController::sendOnAllBots(QString packet)
{
    int currdelay = 10;
    foreach (Player* ply, players) {
        ply->getNet()->sendDelayed(packet,currdelay);
        currdelay += Settings::getDelay("other");
    }
}

void PlayerController::removeBot(Player *ply)
{
    int max = Settings::getGlobalSettings()->getSetting("max_botcount").toInt();
    Logger::getGlobalLogger()->addLog("BOT CTRL","Removed Bot");
    players.removeAll(ply);
    emit botCountChanged();
    emit botCount(players.count(),max);
    backupCurrentSession();
}

void PlayerController::moneyUpdate()
{
    int totalAmmount = 0;
    int earnedAmmount = 0;
    foreach (Player* ply, players) {
        totalAmmount+= ply->getGold();
        earnedAmmount+=(ply->getGold()-ply->getGold_start());
    }
    Settings::getGlobalSettings()->changeSetting("stats/totalEarned",earnedAmmount);
    emit moneyStats(totalAmmount,earnedAmmount);
}

void PlayerController::recvSessDialog(QStringList profiles)
{
    creationQueue.append(profiles);
    if(queueRunning)return;
    createNext();
}

void PlayerController::dialogClicked(QString name, bool autologin, bool abort)
{
    disconnect(this,SLOT(dialogClicked(QString,bool,bool)));
    if(!abort){
        addBot(name,autologin);
    }
}

void PlayerController::addBot(QString name, bool autologin)
{
    int max = Settings::getGlobalSettings()->getSetting("max_botcount").toInt();
    if(players.count()<max){
        Logger::getGlobalLogger()->addLog("BOT CTRL","Loading Bot "+name+".");
        Player *newPlayer = new Player(name,autologin);
        connect(newPlayer,SIGNAL(remove(Player*)),this,SLOT(removeBot(Player*)));
        connect(newPlayer,SIGNAL(moneyUpdate()),this,SLOT(moneyUpdate()));
        players.append(newPlayer);
        emit addTab(name,newPlayer->getUI());
        emit botCountChanged();
        emit botCount(players.count(),max);
        backupCurrentSession();
    }else{
        notificationSystem::get().showMessage("CLNos","You reached the MAXIMUM of currently allowed Bots");
    }
}
