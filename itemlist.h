#ifndef ITEMLIST_H
#define ITEMLIST_H

#include <QAbstractTableModel>
#include <QObject>
#include "bazarmanager.h"
class bItem;

class itemlist : public QAbstractTableModel
{
public:
    itemlist(int type = 0,QObject *parent = 0);
    virtual ~itemlist();
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    void addItem(bItem* item);
    void updateItem(bItem* item);
    int getById(int id);
    void removeItem(int row);
    void clear();
    void addUpdate(bItem* item);
    bItem *getNext();
    int count();
    void updateId(int id);
    bItem *getItemById(int id);
    QList<QList<int>> toList();
    void loadList(QList<QList<int>> temp);

private:
    QList<bItem*> items;
    int type;
    int counter;
};

#endif // ITEMLIST_H
