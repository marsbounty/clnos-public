#include "packetlogger.h"
#include "ui_packetlogger.h"
#include "playercontroller.h"
#include "player.h"
#include "networkmanager.h"
#include "bazarmanager.h"
#include <QTime>

PacketLogger::PacketLogger(PlayerController *pCon, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PacketLogger),
    recvModel(new QStringListModel({"mv","in","out"},this)),
    sendModel(new QStringListModel({"pulse"},this)),
    pCon(pCon)
{
    ui->setupUi(this);
    open();
    ui->filter_recieve->setModel(recvModel);
    ui->filter_send->setModel(sendModel);
    ply  = nullptr;
    setWindowTitle("PacketLogger");

    connect(pCon,SIGNAL(botCountChanged()),this,SLOT(updatePList()));
    setAttribute(Qt::WA_DeleteOnClose);
    updatePList();
    setWindowIcon(QIcon(QPixmap(":/icons/ressources/settings.png")));
}

PacketLogger::~PacketLogger()
{
    delete ui;
}

void PacketLogger::on_clear_packetView_clicked()
{
    ui->packetView->clear();
}

void PacketLogger::on_add_filter_recieve_clicked()
{
    int row = recvModel->rowCount();
    recvModel->insertRows(row,1);
    QModelIndex index = recvModel->index(row);
    ui->filter_recieve->setCurrentIndex(index);
    ui->filter_recieve->edit(index);
}

void PacketLogger::on_remove_filter_recieve_clicked()
{
    recvModel->removeRows(ui->filter_recieve->currentIndex().row(),1);
}

void PacketLogger::on_add_filter_send_clicked()
{
    int row = sendModel->rowCount();
    sendModel->insertRows(row,1);
    QModelIndex index = sendModel->index(row);
    ui->filter_send->setCurrentIndex(index);
    ui->filter_send->edit(index);
}

void PacketLogger::on_remove_filter_send_clicked()
{
    sendModel->removeRows(ui->filter_send->currentIndex().row(),1);
}

void PacketLogger::updatePList()
{
    ui->botname->clear();
    for(int i = 0;i<pCon->getPlayerList().size();i++){
        ui->botname->addItem(pCon->getPlayerList().value(i)->getName());
    }
}

void PacketLogger::recieveRecievePacket(QStringList packet)
{
    if(!recvModel->stringList().contains(packet[0])){
        ui->packetView->append(getTimestamp()+"Recv> "+packet.join(" "));
    }
}

void PacketLogger::recieveSendPacket(QStringList packet)
{
    if(!sendModel->stringList().contains(packet[0])){
        ui->packetView->append(getTimestamp()+"Send> "+packet.join(" "));
    }
}

void PacketLogger::on_sendPacket_clicked()
{
    ply->getNet()->send(ui->sendpacket->toPlainText());
}

void PacketLogger::selectChar(QString name)
{
    if(ply&&ply!=nullptr){
        disconnect(ply->getNet(),SIGNAL(packetLogSend(QStringList)),this,SLOT(recieveSendPacket(QStringList)));
        disconnect(ply->getNet(),SIGNAL(packetLogRecv(QStringList)),this,SLOT(recieveRecievePacket(QStringList)));
    }
    if(name!=""){
        for(int i = 0;i<pCon->getPlayerList().size();i++){
            if(pCon->getPlayerList().value(i)->getName()==name){
                ply = pCon->getPlayerList().value(i);
            }
        }
    }
    if(ply){
        ui->userid->setText(QString::number(ply->getId()));
        ui->cryptoversion->setText(ply->getIsOldVersion()?"True":"False");
        connect(ply->getNet(),SIGNAL(packetLogSend(QStringList)),this,SLOT(recieveSendPacket(QStringList)));
        connect(ply->getNet(),SIGNAL(packetLogRecv(QStringList)),this,SLOT(recieveRecievePacket(QStringList)));
        ui->packetView->clear();
    }

}

QString PacketLogger::getTimestamp()
{
    if(ui->showTimestamps->isChecked()){
        return QTime::currentTime().toString("hh:mm:ss.zzz ");
    }else{
        return "";
    }

}

void PacketLogger::on_botname_currentTextChanged(const QString &arg1)
{
    selectChar(arg1);
}
