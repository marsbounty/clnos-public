#ifndef NOSOBJECTMANAGER_H
#define NOSOBJECTMANAGER_H

#include <QObject>
#include <QMap>

class skill;
class item;

class nosObjectManager : public QObject
{
    Q_OBJECT
public:
    explicit nosObjectManager(QObject *parent = nullptr);

    void generateSkills();

    static QMap<int, skill *>& getSkills() ;

    static QList<int>& getBuffs();

signals:

public slots:

private:
    static QMap<int,skill* > skills;
    static QMap<int,item* > items;
    static QList<int> buffs;

    void parseSkill(QString input);
};

#endif // NOSOBJECTMANAGER_H
