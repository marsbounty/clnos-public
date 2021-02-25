#ifndef GFLOGIN_H
#define GFLOGIN_H

#include <QObject>
#include "player.h"

class gflogin : public QObject
{
    Q_OBJECT
public:
    explicit gflogin(Player* ply,QObject *parent = nullptr);
    ~gflogin();

signals:

public slots:

private slots:
    void getId();
    void getUID();

private:
    Player* ply;
    static QList<gflogin*> line;
    bool firstrun;
    QString path;
};

#endif // GFLOGIN_H
