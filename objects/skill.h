#ifndef SKILL_H
#define SKILL_H

#include <QObject>
#include <QMap>

class skill : public QObject
{
    Q_OBJECT
public:
    explicit skill(QObject *parent = nullptr);

    bool getComplete() const;
    void setComplete(bool value);

    int getRange() const;
    void setRange(int value);

    int getCastTime() const;
    void setCastTime(int value);

    int getCooldownTime() const;
    void setCooldownTime(int value);

    int getManaCost() const;
    void setManaCost(int value);

    int getUseId() const;
    void setUseId(int value);

    int getSkillId() const;
    void setSkillId(int value);

    QString getName() const;
    void setName(const QString &value);

    int getTargetType() const;
    void setTargetType(int value);

    int getSkillType() const;

    void setSkillType(int value);

    int getType() const;
    void setType(int value);

    bool getBasic() const;
    void setBasic(bool value);

    int getEffId() const;
    void setEffId(int value);

signals:

public slots:

private:
    static QMap<int,skill *> skills;

    int skillId;
    int useId;
    int manaCost;
    int cooldownTime;
    int castTime;
    int range;
    QString name;
    int targetType;
    int skillType;
    int type;
    bool basic;
    int effId;

};

#endif // SKILL_H
