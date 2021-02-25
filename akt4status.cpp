#include "akt4status.h"
#include "ui_akt4status.h"
#include "player.h"
#include <QTimer>
#include "notificationsystem.h"
#include "settings.h"

QMap<QString,akt4s> akt4status::stats;

akt4status::akt4status(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::akt4status)
{
    ui->setupUi(this);
    open();
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle("Akt 4 Info");
    ui->lastupdate_time->setText("no update recieved yet");
    QTimer *test = new QTimer(this);
    test->start(1000);
    connect(test,SIGNAL(timeout()),this,SLOT(update()));
    update();
}

void akt4status::updateStat(QStringList packet, Player *ply)
{

    akt4s temp;
    temp.angel = packet[3].toInt()!=0?100:packet[2].toInt();
    temp.demon = packet[12].toInt()!=0?100:packet[11].toInt();
    temp.time = QTime::currentTime();
    temp.timeleft = packet[3].toInt()!=0?packet[4].toInt():packet[13].toInt();
    stats.insert(ply->getServerName(),temp);

    if(temp.angel==100&&temp.timeleft!=0&&Settings::getGlobalSettings()->getSetting("notifications/akt4angel").toBool()){
        notificationSystem::get().showMessage("Akt 4 Raid","Akt 4 Angel Raid for "+QString::number(temp.timeleft/60)+"min on Server "+ply->getServerName());
    }
    if(temp.demon==100&&temp.timeleft!=0&&Settings::getGlobalSettings()->getSetting("notifications/akt4demon").toBool()){
        notificationSystem::get().showMessage("Akt 4 Raid","Akt 4 Demon Raid for "+QString::number(temp.timeleft/60)+"min on Server "+ply->getServerName());
    }
}

akt4status::~akt4status()
{
    delete ui;
}

void akt4status::setLastUpdated(QTime time)
{
    ui->lastupdate_time->setText(time.toString("hh:mm"));
}

void akt4status::update()
{

    if(!stats.isEmpty()){
        akt4s curr = stats.first();
        int elaps = curr.time.elapsed()/1000;
        int tleft = (curr.timeleft-elaps);
        ui->pb_angel->setValue(curr.angel);
        ui->pb_demon->setValue(curr.demon);
        ui->servername->setText(stats.firstKey());
        ui->angel_nr->setText(curr.angel==100?"Raid!":QString::number(curr.angel));
        ui->demon_nr->setText(curr.demon==100?"Raid!":QString::number(curr.demon));
        if(curr.angel==100){
            ui->angel_nr->setStyleSheet("color:red");
        }else{
            ui->angel_nr->setStyleSheet("color:black");
        }
        if(curr.demon==100){
            ui->demon_nr->setStyleSheet("color:red");
        }else{
            ui->demon_nr->setStyleSheet("color:black");
        }
        if(curr.demon==100||curr.angel==100){
            ui->raidinfo->setText(QDateTime::fromTime_t(tleft).toUTC().toString("mm:ss"));
        }else{
            ui->raidinfo->setText("");
        }
        setLastUpdated(curr.time);
        ui->lastupdate_time->setToolTip("Last packet recieved "+QDateTime::fromTime_t(elaps).toUTC().toString("hh:mm:ss")+" ago");
    }
}

