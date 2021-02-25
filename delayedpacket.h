#ifndef DELAYEDPACKET_H
#define DELAYEDPACKET_H

#include <QObject>

class NetworkManager;

class delayedPacket : public QObject
{
    Q_OBJECT
public:
    explicit delayedPacket(QString packet, int msec, NetworkManager *owner, QObject *parent = nullptr);

signals:

public slots:

private slots:
    void trigger();

private:
    int delay;
    NetworkManager* owner;
    QString packet;
};

#endif // DELAYEDPACKET_H
