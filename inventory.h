#ifndef INVENTORY_H
#define INVENTORY_H

#include <QObject>
#include <QList>
#include <QHash>

struct invItem{
    int itemid;
    int ammount;
};
class Player;

class Inventory : public QObject
{
    Q_OBJECT
public:
    explicit Inventory(Player *ply,QObject *parent = nullptr);
    void addPacket(QStringList packet);
    int getItemCount(int id);
    QPair<int,int> getItemPos(int id);
    void useItem(int id);
    QPair<int, int> getFullestStack(int id);
    int getSlotCount(int inv, int slot);
    int getFreeInvSlots();
    QStringList getInvList(int inv);

signals:
    void updateInv();

public slots:

private:
    QHash<QString,invItem> items;
    QList<int> trackedItems;
    Player *ply;
};

#endif // INVENTORY_H
