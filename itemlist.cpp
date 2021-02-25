#include "itemlist.h"
#include "bitem.h"
#include "nosstrings.h"
#include <QLocale>

itemlist::itemlist(int type, QObject *parent):
    type(type)
{
    Q_UNUSED(parent);
    counter=-1;
}

itemlist::~itemlist()
{
    clear();
}

int itemlist::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return items.count();
}

int itemlist::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 4;
}

QVariant itemlist::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();
    if (role != Qt::DisplayRole) return QVariant();
    bItem *item = items[index.row()];
    switch (index.column()) {
    case 0:
        return NosStrings::getItemName(item->getItemid());
        break;
    case 1:
        if(type == 0){
            return QLocale().toString(item->getPrice());
            //return item->getPrice();
        }else{
            QString ret = QString::number(item->getPrice());
            if(item->getMaxammount()!=-1){
                ret+="  (lowest -"+QString::number(item->getMaxammount())+")";
            }
            return ret;
        }

        break;
    case 2:
        if(type == 0){
            if(item->getMaxammount()<0){
                return QString::number(item->getAmmount())+"/"+QString("\u221e");
            }else{
                return QString::number(item->getAmmount())+"/"+QString::number(item->getMaxammount());
            }
        }else{
            if(item->getAmmount()!=-1){
                return item->getAmmount();
            }else{
                return "All";
            }
        }
        break;
    case 3:
        if(type == 0){
            return QLocale().toString(item->getCurrentprice());
            //return item->getCurrentprice();
        }else{
            return item->getType()==1?"true":"false";
        }
        break;
    default:
        return QVariant();
        break;
    }

}

QVariant itemlist::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) return QVariant();
    if (orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
            return tr("Item");
            break;
        case 1:
            if(type == 0){
                return tr("Max Price");
            }else{
                return tr("Price/Beat Comp");
            }
            break;
        case 2:
            if(type == 0){
                return tr("Ammount");
            }else{
                return tr("Stack Size");
            }
            break;
        case 3:
            if(type == 0){
                return tr("Current Price");
            }else{
                return tr("Beat Myself");
            }
            break;
        default:
            return "Unknown";
        }
    }
    return QVariant();

}

void itemlist::addItem(bItem *item)
{
    beginInsertRows(QModelIndex(),items.count(),items.count());
    items.append(item);
    endInsertRows();
}

void itemlist::updateItem(bItem *item)
{
    int row = getById(item->getItemid());
    if(row != -1){
        delete(items[row]);
        items[row] = item;
        dataChanged(createIndex(row,0),createIndex(row,4));
    }
}

int itemlist::getById(int id)
{
    for(int i = 0;i<items.count();i++){
        if(items[i]->getItemid() == id){
            return i;
        }
    }
    return -1;
}

void itemlist::removeItem(int row)
{
    beginRemoveRows(QModelIndex(),row,row);
    delete(items[row]);
    items.removeAt(row);
    endRemoveRows();
}

void itemlist::clear()
{
    if(!items.isEmpty()){
        beginRemoveRows(QModelIndex(),0,items.count()-1);
        qDeleteAll(items);
        items.clear();
        endRemoveRows();
    }
}

void itemlist::addUpdate(bItem *item)
{
    if(getById(item->getItemid())!=-1){
        updateItem(item);
    }else{
        addItem(item);
    }
}

bItem *itemlist::getNext()
{
    counter++;
    if(counter>=items.count()){
        counter = 0;
    }
    return items[counter];
}

int itemlist::count()
{
    return items.count();
}

void itemlist::updateId(int id)
{
    int row = getById(id);
    if(row != -1){
        dataChanged(createIndex(row,0),createIndex(row,4));
    }
}

bItem *itemlist::getItemById(int id)
{
    int row = getById(id);
    if(row != -1){
        return items[row];
    }
    return nullptr;
}

QList<QList<int> > itemlist::toList()
{
    QList<QList<int>> out;
    foreach (bItem *item, items) {
        QList<int> temp;
        temp.append(item->getType());
        temp.append(item->getItemid());
        temp.append(item->getAmmount());
        temp.append(item->getMaxammount());
        temp.append(item->getPrice());
        temp.append(item->getCurrentprice());
        temp.append(item->getTimeleft());
        out.append(temp);
    }
    return out;
}

void itemlist::loadList(QList<QList<int> > temp)
{
    foreach (QList<int> item, temp) {
        bItem *newI = new bItem(item.at(1),item.at(2),item.at(4),item.at(3),item.at(0),item.at(5),item.at(6),"");
        addUpdate(newI);
    }
}

