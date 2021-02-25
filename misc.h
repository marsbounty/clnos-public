#ifndef MISC_H
#define MISC_H

#include <QObject>
#include <QPointer>
#include <QMap>
#include "moneyui.h"
class QTimer;
class Player;
class QTime;

class Misc : public QObject
{
    Q_OBJECT
public:
    explicit Misc(Player *ply, QObject *parent = nullptr);
    virtual ~Misc();
    void joinIC();
    void joinSheep();
    int getCraftStatus() const;
    void startCrafting();
    void stopCrafting();
    void pauseCrafting(bool value);
    void openMoneyUI();
    void toggleCrafting();
    void acceptTrade(QStringList packet);
    void delay(QStringList packet);
    void customDelay(int steps, QString packet, QString var);
    void startMasterSearch();
    void stopMasterSearch();
    void toggleFollower();
    void joinArena();
    void statHandler(QStringList packet);
    void npcReqHandler(QStringList packet);
    void useSkill(int id, int cd, int target = 0);
    bool isBusy();
    QStringList getCollectables();
    void startCollecting();
    void stopCollecting();
    void funRaidStatusPacket(QStringList packet);
    void umiHandler(QStringList packet);
    void qnaHandler(QStringList packet);
    void rlHandler(QStringList packet);
    void guriHandler(QStringList packet);

    int randInt(int low, int high);
    QList<int> getCollectableIds();
    bool getIsCollecting() const;
    void startKillingSheep();

    void setKillSkillUp(bool value);
    void skillMirrorHandler(int user, int target, int skill);

    void closeBasar();

    bool getBasarOpen() const;
    void setBasarOpen(bool value);

    void joinAOT();


signals:
    void icStatus(int status);
    void craftingStatus(int status);
    void raidListUpdate(QList<raid>);

public slots:
    void recieveQnaml(QStringList packet);
    void recieveDlg(QStringList packet);
    void mapChanged(int id);
    void icHelper();
    void rcvStatusUpdate(int id);
    void craftingUse();
    void tryCollect();
    void invUpdate();
    void delayHelper();
    void getMasterId(int id, QString name, int x, int y);
    void follow(int id, int x, int y);
    void followPortal(int id);
    void createSheepRaid();
    void tryToOpenBasar();

private slots:
    void portalSpawned(int id, QString name, int x, int y);
    void attackClosestSheep();
    void joinAOThelper();


private:
    Player* ply;
    bool inIc;
    bool inSheep;
    short AOTstatus;
    QTimer *icHelpTimer;
    QTimer *craftTimer;
    QTimer *sellTimer;
    QTimer *collectTimer;
    int craftStatus;
    QPointer<MoneyUI> mui;
    QString delayCommand;
    int delaySteps;
    int delayCurrent;
    QString delayVar;
    bool followS;
    bool masterSearch;
    QMap<int,QTime> skillcd;
    QList<int> curCol;
    int curColPos;
    bool killSkillUp;
    bool isCollecting;
    void collectNextItem();
    bool basarOpen;
    QPoint lastFollowerPos;
    QTimer* delayT;

    QMap<int,int> knownSkills;
};

#endif // MISC_H
