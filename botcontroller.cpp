#include "botcontroller.h"
#include "ui_botcontroller.h"
#include "playercontroller.h"
#include "battlemanager.h"
#include "player.h"
#include "map.h"

BotController::BotController(PlayerController *pCon,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BotController),pCon(pCon)
{
    ui->setupUi(this);
    show();
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle("All Bot Controlls");
    //this->setModal(true);
    fillPlayerLists();
    setModal(false);
}

BotController::~BotController()
{
    delete ui;
}

void BotController::fillPlayerLists()
{
    ui->playerList->clear();
    foreach (Player *ply, pCon->getPlayerList()) {
        foreach (entity temp, ply->getMap()->getPlayerMap()) {
            if(!names.contains(temp.name)){
                names.append(temp.name);
                ids.append(temp.id);
            }
        }
    }
    names.sort(Qt::CaseInsensitive);
    ui->playerList->addItems(names);
}


void BotController::on_healBtn_clicked()
{
    pCon->useAllBuffs();
}

void BotController::on_aotBtn_clicked()
{

}

void BotController::on_attackBtn_clicked()
{
    foreach (Player *ply, pCon->getPlayerList()) {
        ply->getBmanager()->attackClosestEnemy();
    }
}

void BotController::on_chatBtn_clicked()
{
    pCon->sendOnAllBots(QString("say %1").arg(ui->chatInput->text()));
}

void BotController::on_complimentBtn_clicked()
{
    pCon->sendOnAllBots(QString("compl 1 %1").arg(ids.value(ui->playerList->currentIndex())));
    ui->complimentBtn->setEnabled(false);
}

void BotController::on_packet_send_clicked()
{
    pCon->sendOnAllBots(ui->packet_input->text());
}
