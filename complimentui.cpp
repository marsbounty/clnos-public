#include "complimentui.h"
#include "ui_complimentui.h"
#include "playercontroller.h"
#include "player.h"
#include "map.h"

ComplimentUI::ComplimentUI(PlayerController *pCon ,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ComplimentUI),pCon(pCon)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle("Compliment Player");
    this->setModal(true);
    fillPlayerLists();
}

ComplimentUI::~ComplimentUI()
{
    delete ui;
}

void ComplimentUI::fillPlayerLists()
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
    ui->playerList->addItems(names);
}

void ComplimentUI::on_compliment_clicked()
{
    pCon->sendOnAllBots(QString("compl 1 %1").arg(ids.value(ui->playerList->currentIndex())));
    deleteLater();
}
