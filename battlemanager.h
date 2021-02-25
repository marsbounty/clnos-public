#ifndef BATTLEMANAGER_H
#define BATTLEMANAGER_H

#include <QObject>
//#include "skillz.h"
#include <QMap>

enum class Status
  {
     Ready,
     NotReady
  };

enum class TargetType
  {
     Mob,
     Npc,
    Player,
    self
  };

struct Target
{
    TargetType type;
    int id;
    int hp;
};

class Player;
class skill;

class BattleManager : public QObject
{
    Q_OBJECT
public:
    explicit BattleManager(Player* ply,QObject *parent = nullptr);
    static void initSkills();
    static void printSkills();

    void attackClosestEnemy();
    void removeCooldown(int id);
    void mirrorSkill(const QStringList &list );
    void loadAASkillId();
    void useSkill(int id, int target, TargetType type = TargetType::Mob, int delay = 0);
    void useSkillSimple(int id);

    void setTarget(int id, TargetType type);
    void stPacketHandler(QStringList packet);
    void killTarget();
    void attackClosestPlayer();
    void cancelSkill(int id);
signals:

public slots:
    void useAllBuffs();

private slots:
    void trackTarget();
private:
    static QMap<int,skill *> skills;
    QList<int> onCooldown;
    QList<int> usedBuffs;

    Player* ply;

    bool isOnCooldown(int id);

    Status buffStatus;
    int aaSkillId;
    Target currentTarget;

};

#endif // BATTLEMANAGER_H
