#include "playerui.h"
#include "ui_playerui.h"
#include "networkmanager.h"
#include "settings.h"
#include "nosstrings.h"
#include "player.h"
#include <QDebug>
#include "logger.h"
#include "map.h"
#include "misc.h"
#include "bazarmanager.h"
#include "chatmng.h"

PlayerUI::PlayerUI(Player *ply,QWidget *parent,QString username,QString password,int server) :
    QWidget(parent),
    ui(new Ui::PlayerUI),
    ply(ply)
{
    Q_UNUSED(server);
    ui->setupUi(this);
    autologin = false;
    ui->inp_password->setText(password);
    ui->inp_username->setText(username);
    ui->cmb_login_version->setCurrentIndex(ply->getSettings()->getSetting("login/loginserver").toInt());
    ui->stk_login_char->setCurrentIndex(0);
    connect(ply->getMap(),SIGNAL(mapChanged(int)),this,SLOT(mapChanged(int)));
    connect(ply->getBazar(),SIGNAL(buyStatus(int)),this,SLOT(buyStatusUpdate(int)));
    connect(ply->getBazar(),SIGNAL(sellStatus(int)),this,SLOT(sellStatusUpdate(int)));

    connect(ply->getMisc(),SIGNAL(craftingStatus(int)),this,SLOT(craftStatusUpdate(int)));
    connect(ply->getMisc(),SIGNAL(icStatus(int)),this,SLOT(icStatusUpdate(int)));

    chars.clear();
    if(password!=""&&username!=""){
        if(ply->getAutologin()){
            ui->login_login->click();
            autologin = true;
        }
    }
}

PlayerUI::~PlayerUI()
{
    delete ui;
}

void PlayerUI::showWindow(int id)
{
    ply->getLog()->addLog("UI","Changing ui to "+QString::number(id));
    if(ply->getIsOldVersion()){
        ui->vendetta->setText("Vendetta");
    }else{
        ui->vendetta->setText("");
        //ply->getLog()->addLog("UI","Not Vendetta!");
    }
    if(id == 1){

    }else if(id == 2){

        if(chars.isEmpty()){
            displayLoginError("99");
        }else{
            ui->stk_login_char->setCurrentIndex(2);
            ui->login_char->setCurrentIndex(ply->getSettings()->getSetting("login/char").toInt());
            if(autologin){
                ui->btn_select_char->click();
            }
        }
    }else if(id == 4){
        ui->stk_login_char->setCurrentIndex(4);
    }
}

void PlayerUI::showServerSelection(QMap<int, server> &srv)
{
    servers = srv;
    serverids.clear();
    ui->cmb_server->clear();
    //qDebug()<<servers.count()<<srv.count();
    QMapIterator<int, server> i(servers);
    while (i.hasNext()) {
        i.next();
        ui->cmb_server->addItem(i.value().name);
        serverids.insert(i.value().name,i.key());
        //qDebug()<<i.key()<<i.value().ip;
    }
    ui->stk_login_char->setCurrentIndex(1);
    ui->cmb_server->setCurrentIndex(ply->getSettings()->getSetting("login/server").toInt());
    ui->cmb_channel->setCurrentIndex(ply->getSettings()->getSetting("login/channel").toInt());
    if(autologin){
        ui->btn_select_server->click();
    }
}

void PlayerUI::addChar(QString name, int id)
{
    chars.insert(name,id);
    ui->login_char->addItem(name);
}

void PlayerUI::displayLoginError(QString errorstr)
{
    QString msg;
    switch (errorstr.toInt()) {
    case 1:
        msg = "Client outdated! Try again later.";
        break;
    case 3:
        msg = "Servers are in maintanance Mode! Try again later.";
        break;
    case 4:
        msg = "Profile already in use!";
        break;
    case 5:
        msg = "Wrong Username/Password!";
        break;
    case 7:
        msg = "You are banned! Sorry";
        break;
    case 99:
        msg = "You don't have any Char on this Server! Use the orignal Client to create one.";
        break;
    default:
        if(errorstr!="Server full!"){
            msg = "Unknown error Nr."+errorstr;
        }else{
            msg = errorstr;
        }
        if(Settings::getGlobalSettings()->getSetting("login/autoreconnect").toBool()){
            reconnectTime = 61;
            runReconnect();
        }else{
            msg += "\n Autoreconnect Disabled";
        }


        break;
    }
    ply->getNet()->disconnect();
    ply->getLog()->addLog("NET",msg);
    ui->login_error->setText(msg);
    ui->stk_login_char->setCurrentIndex(3);
    ui->btn_select_server->setEnabled(true);
    ui->btn_select_char->setEnabled(true);
    autologin = false;



}

void PlayerUI::setUsername(QString name)
{
    ui->cinfo_username->setText(name);
}

void PlayerUI::setLvl(int lvl, int jlvl)
{
    ui->cinfo_lvl->setText(QString::number(lvl));
    ui->cinfi_jlvl->setText(QString::number(jlvl));
}

void PlayerUI::setCharImage(int i)
{
    ui->cinfo_charimage->setPixmap(":/profiles/ressources/"+QString::number(i+1)+".png");
}

void PlayerUI::setGold(int v1, int v2)
{
    ui->cinfo_gold->setText(QLocale().toString(v1));
    if(v2 >=0){
        ui->cinfo_goldwin->setText("+"+QLocale().toString(v2));
        ui->cinfo_goldwin->setStyleSheet("color:green;");
    }else{
        ui->cinfo_goldwin->setText(QLocale().toString(v2));
        ui->cinfo_goldwin->setStyleSheet("color:red;");
    }
}

void PlayerUI::showGFLoginError(QString err)
{
    ui->stk_login_char->setCurrentIndex(5);
    ui->gfloginStatus->setText(err);
}

void PlayerUI::icStatusUpdate(int i)
{
    switch (i) {
    case 0:
    {
        QPixmap pmap(":/profiles/ressources/statusred.png");
        ui->status_ic->setPixmap(pmap);
        break;
    }
    case 1:
    {
        QPixmap pmap(":/profiles/ressources/statusgreen.png");
        ui->status_ic->setPixmap(pmap);
        break;
    }
    case 2:
    {
        QPixmap pmap(":/profiles/ressources/statusorange.png");
        ui->status_ic->setPixmap(pmap);
        break;
    }
    default:
        break;
    }
}

void PlayerUI::buyStatusUpdate(int i)
{
    switch (i) {
    case 0:
    {
        QPixmap pmap(":/profiles/ressources/statusred.png");
        ui->status_buy->setPixmap(pmap);
        break;
    }
    case 1:
    {
        QPixmap pmap(":/profiles/ressources/statusgreen.png");
        ui->status_buy->setPixmap(pmap);
        break;
    }
    case 2:
    {
        QPixmap pmap(":/profiles/ressources/statusorange.png");
        ui->status_buy->setPixmap(pmap);
        break;
    }
    default:
        break;
    }
}

void PlayerUI::sellStatusUpdate(int i)
{
    switch (i) {
    case 0:
    {
        QPixmap pmap(":/profiles/ressources/statusred.png");
        ui->status_sell->setPixmap(pmap);
        break;
    }
    case 1:
    {
        QPixmap pmap(":/profiles/ressources/statusgreen.png");
        ui->status_sell->setPixmap(pmap);
        break;
    }
    case 2:
    {
        QPixmap pmap(":/profiles/ressources/statusorange.png");
        ui->status_sell->setPixmap(pmap);
        break;
    }
    default:
        break;
    }
}

void PlayerUI::craftStatusUpdate(int i)
{
    switch (i) {
    case 0:
    {
        QPixmap pmap(":/profiles/ressources/statusred.png");
        ui->status_crafting->setPixmap(pmap);
        break;
    }
    case 1:
    {
        QPixmap pmap(":/profiles/ressources/statusgreen.png");
        ui->status_crafting->setPixmap(pmap);
        break;
    }
    case 2:
    {
        QPixmap pmap(":/profiles/ressources/statusorange.png");
        ui->status_crafting->setPixmap(pmap);
        break;
    }
    default:
        break;
    }
}

void PlayerUI::mapChanged(int id)
{
    ui->cinfo_mapname->setText(NosStrings::getMapName(id));
}

void PlayerUI::on_login_login_clicked()
{
    QList<int> ports = {4000,4001,4002,4003,4004,4005,4006,4008,4007,4000};
    ui->login_login->setEnabled(false);
    int currIndex = ui->cmb_login_version->currentIndex();
    QString username = ui->inp_username->text();
    QString password = ui->inp_password->text();
    QString ip = "79.110.84.75";
    bool oldversion = false;
    int port = ports.value(currIndex);
    if(currIndex == 9){
        ip = "46.105.115.121";
        oldversion = true;
    }
    if(currIndex == 10){
        ip = Settings::getGlobalSettings()->getSetting("customServer/ip").toString();
        port = Settings::getGlobalSettings()->getSetting("customServer/port").toInt();
    }
    ply->login(username,password,ip,port,oldversion,currIndex);
    ply->getSettings()->changeSetting("login/username",username);
    ply->getSettings()->changeSetting("login/password",password);
    ply->getSettings()->changeSetting("login/loginserver",currIndex);
}


void PlayerUI::on_cmb_server_currentIndexChanged(int index)
{
    if(index == -1)return;
    index = index +1;
    ui->cmb_channel->clear();
    if(servers.contains(index)){
        QMapIterator<int, int> i(servers.value(index).channels);
        while (i.hasNext()) {
                i.next();
                ui->cmb_channel->addItem("Channel "+QString::number(i.key()));
        }
    }else if (serverids.contains(ui->cmb_server->currentText())) {
        QMapIterator<int, int> i(servers.value(serverids.value(ui->cmb_server->currentText())).channels);
        while (i.hasNext()) {
                i.next();
                ui->cmb_channel->addItem("Channel "+QString::number(i.key()));
        }
    }else{
        if(index!=-1){
            ply->getLog()->addLog("UI","Serverid "+QString::number(index)+" not found!");
        }
    }
}

void PlayerUI::on_btn_select_server_clicked()
{
    //if(!ui->cmb_server->currentText().contains("Fernon",Qt::CaseInsensitive)){
        ui->btn_select_server->setEnabled(false);
        int srvindex = serverids.value(ui->cmb_server->currentText());//ui->cmb_server->currentIndex();
        int chindex = ui->cmb_channel->currentIndex();
        QString ip = servers.value(srvindex).ip;
        int port = servers.value(srvindex).channels.value(chindex+1);
        ply->getSettings()->changeSetting("login/server",ui->cmb_server->currentIndex());
        ply->getSettings()->changeSetting("login/channel",chindex);
        ply->setServerName(ui->cmb_server->currentText());
        servers.clear();
        ply->getNet()->connectToGame(ip,port);
    //}else{
    //    displayLoginError("This Server is not Supported at the moment!");
    //}
}

void PlayerUI::on_btn_select_char_clicked()
{
    ui->btn_select_char->setEnabled(false);
    int charid = chars.value(ui->login_char->currentText());
    ply->getNet()->selectChar(charid);
    ply->getSettings()->changeSetting("login/char",ui->login_char->currentIndex());
    chars.clear();
}

void PlayerUI::on_login_fail_ok_clicked()
{
    ui->stk_login_char->setCurrentIndex(0);
    ui->login_login->setEnabled(true);
    ui->btn_select_char->setEnabled(true);
}



void PlayerUI::on_cinfo_logout_clicked()
{
    ply->logout();
}

void PlayerUI::on_cinfi_minimap_clicked()
{
    ply->getMap()->showMinimap();
}

void PlayerUI::on_cinfo_settings_clicked()
{
    ply->openSettings();
}

void PlayerUI::on_login_cancel_clicked()
{
    delete ply;
}

void PlayerUI::on_login_cancel_2_clicked()
{
    delete ply;
}

void PlayerUI::on_cinfo_money_clicked()
{
    ply->getMisc()->openMoneyUI();
}

void PlayerUI::on_close_clicked()
{
    delete ply;
    stopReconnect();
}

void PlayerUI::on_loginFailRetry_clicked()
{
    on_login_login_clicked();
    stopReconnect();
}

void PlayerUI::on_loginFailClose_clicked()
{
    stopReconnect();
    ply->logout();
}

void PlayerUI::runReconnect()
{
    reconnectTime--;
    ui->loginFailRetry->setText("Retry ("+QString::number(reconnectTime)+")");
    if(reconnectTime>0){
        QTimer::singleShot(1000, this, SLOT(runReconnect()));
    }else if(reconnectTime==0){
        autologin = true;
        on_loginFailRetry_clicked();
    }
}

void PlayerUI::stopReconnect()
{
    reconnectTime = -1;
}

void PlayerUI::on_toolButton_4_clicked()
{
    ply->getChat()->openchat();
}


void PlayerUI::on_gfloginstart_clicked()
{
    ply->startGFlogin();
    ui->stk_login_char->setCurrentIndex(5);
}

void PlayerUI::on_gfloginStop_clicked()
{
    ply->logout();
}

void PlayerUI::on_cmb_login_version_currentIndexChanged(int index)
{
    if(index == 9){
        //ui->gfloginstart->hide();
    }else{
        //ui->gfloginstart->show();
    }
}
