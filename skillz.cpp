#include "skillz.h"
#include <QStringList>
#include <QString>


skillz::skillz(int skillId,int useId, int manaCost, int cdTime, int castTime, int range):
    skillId(skillId),useId(useId),manaCost(manaCost),
    cooldownTime(cdTime),castTime(castTime),range(range){}

skillz::skillz(int skillId, int useid, QString DATA):skillId(skillId),useId(useid)
{
    QStringList temp = DATA.split("	");
    manaCost = temp.at(8).toInt();
    cooldownTime = temp.at(5).toInt();
    castTime = temp.at(4).toInt();
    range = temp.at(11).toInt();
}

int skillz::getSkillId() const
{
    return skillId;
}

void skillz::setSkillId(int value)
{
    skillId = value;
}

int skillz::getUseId() const
{
    return useId;
}

void skillz::setUseId(int value)
{
    useId = value;
}

int skillz::getManaCost() const
{
    return manaCost;
}

void skillz::setManaCost(int value)
{
    manaCost = value;
}

int skillz::getCooldownTime() const
{
    return cooldownTime;
}

void skillz::setCooldownTime(int value)
{
    cooldownTime = value;
}

int skillz::getCastTime() const
{
    return castTime*1000;
}

void skillz::setCastTime(int value)
{
    castTime = value;
}

int skillz::getRange() const
{
    return range;
}

void skillz::setRange(int value)
{
    range = value;
}
