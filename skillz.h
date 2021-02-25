#ifndef SKILL_H
#define SKILL_H

#include <QString>

class skillz
{
public:
    skillz(int skillId,int useId, int manaCost, int cdTime, int castTime, int range);
    skillz(int skillId,int useid, QString DATA);

    int getSkillId() const;
    void setSkillId(int value);

    int getUseId() const;
    void setUseId(int value);

    int getManaCost() const;
    void setManaCost(int value);

    int getCooldownTime() const;
    void setCooldownTime(int value);

    int getCastTime() const;
    void setCastTime(int value);

    int getRange() const;
    void setRange(int value);

private:
    int skillId;
    int useId;
    int manaCost;
    int cooldownTime;
    int castTime;
    int range;
};

#endif // SKILL_H
