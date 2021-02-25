#ifndef BAZARMANAGER_H
#define BAZARMANAGER_H

#include <QObject>
#include <QMap>
#include <QMapIterator>

class bItem;
class Player;
class QTimer;
class itemlist;
class QElapsedTimer;

class BazarManager : public QObject
{
    Q_OBJECT
public:
    explicit BazarManager(Player *ply,QObject *parent = nullptr);
    ~BazarManager();
    void searchItem(int id, int typeN=0);
    void recieveBlist(QStringList packet);
    void recieveSlist(QStringList packet);
    void buyItem(int id,int ammount);
    void collectItem(int id);
    void buyWhen(bItem *item);
    QString getCheapestSeller(int id);
    int getCheapestPrice(int id);
    void sellStack(int id,int price);
    void sellItem(QPair<int,int> slot,int ammount,int price,int tax ,bool isPackage,bool medal);
    void addToBuylist(int id, int ammount ,int price);
    void toggleAutobuy();
    void pause(bool st);
    void toggleAutosell();
    bool getAutoBuyStatus() const;
    bool getAutoSellStatus() const;
    void changeAutoBuy(bool status);
    void changeAutoSell(bool status);
    bool getPaused() const;
    itemlist *getBuylist();
    itemlist *getSelllist();
    void rc_buy(QStringList packet);
    void checkSold();
    void saveItemLists();
    void loadItemLists();

signals:
    void bsearchFinished(int id);
    void currentSellList(QList<QList<int>> list);
    void buyStatus(int status);
    void sellStatus(int status);
    void updateBazarList(QMap<int,bItem*> list);

public slots:

private slots:
    void autobuy();
    void autosell();
    void autoCheckSold();

private:
    Player *ply;
    QMap<int,bItem*> blist;
    QMap<int,bItem*> slist;
    int getCheapest(int id);
    int lastsearch;
    int type;
    bItem* searchitem;
    bool searching;
    bool autoBuyStatus;
    bool autoSellStatus;
    bool paused;
    QTimer* autoBuyTimer;
    QTimer* autoSellTimer;
    QElapsedTimer* timeout;
    QTimer* autoCollectTimer;

    QPair<int,int> currSlot;
    itemlist* buylist;
    itemlist* selllist;
};

#endif // BAZARMANAGER_H
