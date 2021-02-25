#include "bitem.h"

bItem::bItem(QObject *parent):
    QObject(parent)
{
    itemid = 0;
    ammount =0;
    price = 0;
    maxammount = 0;
    type = 0;
    currentprice = 0;
}

bItem::bItem(QStringList load, QObject *parent):
    QObject(parent)
{
    itemid = load.at(0).toInt();
    ammount =load.at(0).toInt();
    price = load.at(0).toInt();
    maxammount = load.at(0).toInt();
    type = load.at(0).toInt();
    currentprice = load.at(0).toInt();
}

bItem::bItem(int itemid, int ammount, int price, int maxammount, int type, int currentprice, int timeleft, QString owner, QObject *parent) :
    QObject(parent),
    itemid(itemid),
    ammount(ammount),
    price(price),
    maxammount(maxammount),
    type(type),
    currentprice(currentprice),
    timeleft(timeleft),
    owner(owner)
{

}

int bItem::getItemid() const
{
    return itemid;
}

void bItem::setItemid(int value)
{
    itemid = value;
}

int bItem::getAmmount() const
{
    return ammount;
}

void bItem::setAmmount(int value)
{
    ammount = value;
}

int bItem::getPrice() const
{
    return price;
}

void bItem::setPrice(int value)
{
    price = value;
}

int bItem::getMaxammount() const
{
    return maxammount;
}

void bItem::setMaxammount(int value)
{
    maxammount = value;
}

int bItem::getType() const
{
    return type;
}

void bItem::setType(int value)
{
    type = value;
}

int bItem::getCurrentprice() const
{
    return currentprice;
}

void bItem::setCurrentprice(int value)
{
    currentprice = value;
}

int bItem::getTimeleft() const
{
    return timeleft;
}

void bItem::setTimeleft(int value)
{
    timeleft = value;
}

QString bItem::getOwner() const
{
    return owner;
}

void bItem::setOwner(const QString &value)
{
    owner = value;
}

QStringList bItem::toQStringList()
{
    QStringList out;
    out.append(QString::number(itemid));
    out.append(QString::number(ammount));
    out.append(QString::number(price));
    out.append(QString::number(maxammount));
    out.append(QString::number(type));
    out.append(QString::number(currentprice));
    return out;
}
