#include "inventory.h"
#include <QDebug>
#include "player.h"
#include "logger.h"
#include "settings.h"
#include "networkmanager.h"
#include "nosstrings.h"

Inventory::Inventory(Player *ply,QObject *parent) : QObject(parent),ply(ply)
{

}

void Inventory::addPacket(QStringList packet)
{
    QString inv = packet[0];
    packet.removeFirst();
    foreach (QString nextItem, packet) {
        QStringList temp = nextItem.split(".");
        QString invslot = (inv+":"+temp[0]);
        if(temp.count()>1){
            if(items.contains(invslot)){
                items[invslot].itemid = temp[1].toInt();
                items[invslot].ammount = temp[2].toInt();
            }else{
                invItem newItem;
                newItem.ammount =temp[2].toInt();
                newItem.itemid = temp[1].toInt();
                items.insert(invslot,newItem);
            }
        }

    }
    emit updateInv();
}

int Inventory::getItemCount(int id)
{
    int count = 0;
    QHashIterator<QString, invItem> i(items);
    while (i.hasNext()) {
        i.next();
        QStringList pos = i.key().split(":");
        if(i.value().itemid==id){
            if(pos[0].toInt()!=0){
                count+=i.value().ammount;
            }else{
                count++;
            }
        }
    }
    return count;
}



QPair<int, int> Inventory::getItemPos(int id)
{
    QHashIterator<QString, invItem> i(items);
    while (i.hasNext()) {
        i.next();
        if(i.value().itemid==id){
            QStringList pos = i.key().split(":");
            return {pos[0].toInt(),pos[1].toInt()};
        }
    }
    return {-1,-1};
}

void Inventory::useItem(int id)
{
    int type = -1;
    int slot = -1;

    QHashIterator<QString, invItem> i(items);
    while (i.hasNext()) {
        i.next();
        if(i.value().itemid==id){
            QStringList pos = i.key().split(":");
            type = pos[0].toInt();
            slot = pos[1].toInt();
            break;
        }
    }

    if(type!=-1&&slot != -1){
        ply->getLog()->addLog("INV","Using Item "+QString::number(id));
        ply->getNet()->send("u_i 1 "+QString::number(ply->getId())+" "+QString::number(type)+" "+QString::number(slot)+" 0 0");
    }else{
        ply->getLog()->addLog("INV","Cannot use Item "+QString::number(id));
    }

}

QPair<int,int> Inventory::getFullestStack(int id)
{
    QPair<int,int> res = {0,0};
    int ammount =0;
    QHashIterator<QString, invItem> i(items);
    while (i.hasNext()) {
        i.next();
        if(i.value().itemid==id){
            QStringList pos = i.key().split(":");
            int amm = i.value().ammount;
            if(pos[0].toInt()==0){
                amm =1;
            }
            if(amm == 999){
                return {pos[0].toInt(),pos[1].toInt()};
            }else if(amm > ammount){
                res = {pos[0].toInt(),pos[1].toInt()};
                ammount = amm;
            }
        }
    }
    return res;
}

int Inventory::getSlotCount(int inv,int slot)
{
    QString key = QString::number(inv)+":"+QString::number(slot);
    if(items.contains(key)){
        if(inv == 0){
            return 1;
        }
        return items.value(key).ammount;
    }
    return 0;
}

int Inventory::getFreeInvSlots()
{
    int ammount =0;
    QHashIterator<QString, invItem> i(items);
    while (i.hasNext()) {
        i.next();
        int inv = i.key().split(":").at(0).toInt();
        if(inv == 1){
            ammount++;
        }
    }

    return 48 - ammount;
}

QStringList Inventory::getInvList(int inv)
{
    QStringList output;
    QHashIterator<QString, invItem> i(items);
    while (i.hasNext()) {
        i.next();
        if(i.key().split(":")[0].toInt()==inv){
            if(inv!=0){
                output.append(NosStrings::getItemName(i.value().itemid)+" ("+QString::number(i.value().ammount)+")");
            }else{
                int ammount = i.value().ammount;
                if(ammount != 0){
                    output.append(NosStrings::getItemName(i.value().itemid)+" (R"+QString::number(i.value().ammount)+")");
                }else{
                    output.append(NosStrings::getItemName(i.value().itemid));
                }

            }

        }
    }
    return output;
}
