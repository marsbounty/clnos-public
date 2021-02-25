#include "delayedpacket.h"
#include "networkmanager.h"
#include <QTimer>

delayedPacket::delayedPacket(QString packet,int msec, NetworkManager *owner, QObject *parent):
    QObject(parent),
    delay(msec),
    owner(owner),
    packet(packet)
{
    QTimer::singleShot(delay,this,SLOT(trigger()));
}

void delayedPacket::trigger()
{
    owner->send(packet);
    deleteLater();
}
