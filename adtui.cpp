#include "adtui.h"
#include "ui_adtui.h"
#include <QCheckBox>

adtui::adtui(QList<Player *> players,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::adtui),
    players(players)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle("Arena Of Talents Setup");
    loadPlayerlist();
}

adtui::~adtui()
{
    delete ui;
}

void adtui::loadPlayerlist(){
    ui->playerList->clear();
    ui->playerList->setRowCount(players.count());
    ui->playerList->setColumnCount(5);
    int i = 0;
    foreach (Player* ply, players) {
        ui->playerList->setCellWidget(i,0,new QCheckBox());
        ui->playerList->setItem(i,1,new QTableWidgetItem(ply->getServerName()));
        ui->playerList->setItem(i,2,new QTableWidgetItem(ply->getName()));
        ui->playerList->setItem(i,3,new QTableWidgetItem(QString::number(ply->getLvl())));
        ui->playerList->setItem(i,4,new QTableWidgetItem("Comming soon"));
        i++;
    }
}
