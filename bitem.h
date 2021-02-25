#ifndef BITEM_H
#define BITEM_H

#include <QObject>

class bItem : public QObject
{
    Q_OBJECT
public:
    explicit bItem(QObject *parent = nullptr);
    explicit bItem(QStringList load,QObject *parent = nullptr);
    explicit bItem(int itemid,int ammount,int price, int maxammount, int type = 0,int currentprice = 0,int timeleft = 0,QString owner = "",QObject *parent = nullptr);
    int getItemid() const;
    void setItemid(int value);
    int getAmmount() const;
    void setAmmount(int value);
    int getPrice() const;
    void setPrice(int value);
    int getMaxammount() const;
    void setMaxammount(int value);
    int getType() const;
    void setType(int value);
    int getCurrentprice() const;
    void setCurrentprice(int value);
    int getTimeleft() const;
    void setTimeleft(int value);
    QString getOwner() const;
    void setOwner(const QString &value);
    QStringList toQStringList();

signals:

public slots:

private:
    int itemid;
    int ammount;
    int price;
    int maxammount;
    int type;
    int currentprice;
    int timeleft;
    QString owner;
};

#endif // BITEM_H
