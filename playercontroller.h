#ifndef PLAYERCONTROLLER_H
#define PLAYERCONTROLLER_H

#include <QObject>
class Player;
class PlayerUI;

class PlayerController : public QObject
{
    Q_OBJECT
public:
    explicit PlayerController(QObject *parent = nullptr);
    virtual ~PlayerController();
    void newBot(bool load);
    void createDefault();
    QList<Player *> &getPlayerList();
    void saveCurrentSession();
    void loadLastSession();
    void loadSession();
    void showAotUI();
    static PlayerController* getPC();
    static void setPC(PlayerController *pc);
    bool entityIsBot(QString name);
    int getCurrentGold();
    int getEarnedGold();
    int getTotalEarnedGold();
    void useAllBuffs();
    void sendOnAllBots(QString packet);

    void backupCurrentSession();
signals:
    void addTab(QString name,PlayerUI *plyui);
    void botCountChanged();
    void botCount(int curr,int max);
    void moneyStats(int total,int earned);

public slots:
    void dialogClicked(QString name,bool autologin,bool abort);
    void removeBot(Player* ply);
    void moneyUpdate();
    void recvSessDialog(QStringList profiles);

private slots:
    void createNext();

private:
    void addBot(QString name,bool autologin);
    QList<Player*> players;
    int totalEarned;
    static PlayerController* mainPC;
    QStringList creationQueue;
    bool queueRunning;

};

#endif // PLAYERCONTROLLER_H
