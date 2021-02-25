#include "skill.h"

skill::skill(QObject *parent) : QObject(parent)
{
    setBasic(true);
}


int skill::getRange() const
{
    return range;
}

void skill::setRange(int value)
{
    range = value;
}

int skill::getCastTime() const
{
    return castTime;
}

void skill::setCastTime(int value)
{
    castTime = value;
}

int skill::getCooldownTime() const
{
    return cooldownTime;
}

void skill::setCooldownTime(int value)
{
    cooldownTime = value;
}

int skill::getManaCost() const
{
    return manaCost;
}

void skill::setManaCost(int value)
{
    manaCost = value;
}

int skill::getUseId() const
{
    return useId;
}

void skill::setUseId(int value)
{
    useId = value;
}

int skill::getSkillId() const
{
    return skillId;
}

void skill::setSkillId(int value)
{
    skillId = value;
}

QString skill::getName() const
{
    return name;
}

void skill::setName(const QString &value)
{
    name = value;
}

int skill::getTargetType() const
{
    return targetType;
}

void skill::setTargetType(int value)
{
    targetType = value;
}

int skill::getSkillType() const
{
    return skillType;
}

void skill::setSkillType(int value)
{
    skillType = value;
}

int skill::getType() const
{
    return type;
}

void skill::setType(int value)
{
    type = value;
}

bool skill::getBasic() const
{
    return basic;
}

void skill::setBasic(bool value)
{
    basic = value;
}

int skill::getEffId() const
{
    return effId;
}

void skill::setEffId(int value)
{
    effId = value;
}
