#ifndef STATTRACKER_H
#define STATTRACKER_H

#include <QObject>

class stattracker : public QObject
{
    Q_OBJECT
public:
    static stattracker* getTracker();
    void icJoined();
    void icWon();
    int getIcJoined();
    int getIcWon();

private:
    explicit stattracker(QObject *parent = nullptr);
    static stattracker *main;

signals:

public slots:
};

#endif // STATTRACKER_H
