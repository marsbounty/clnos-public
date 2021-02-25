#include "battlemanager.h"
#include <QDebug>
#include "player.h"
#include "networkmanager.h"
#include "map.h"
#include "logger.h"
#include "settings.h"
#include "QTimer"
#include "QRandomGenerator"
#include "objects/skill.h"
#include "nosobjectmanager.h"

QMap<int,skill *> BattleManager::skills;

BattleManager::BattleManager(Player *ply, QObject *parent) : QObject(parent),ply(ply),buffStatus(Status::Ready)
{
    onCooldown.clear();
    currentTarget.id = 0;
    aaSkillId = 0;
}

void BattleManager::initSkills()
{
//    //id useId mana cd casttime range


//    //Auto Attacks
//    skills.insert(833,new skillz(833,0,0,8,0,8));
//    skills.insert(260,new skillz(260,0,5,10,4,6));
//    skills.insert(200,new skillz(200,0,0,6,2,1));
//    skills.insert(220,new skillz(220,0,"0	0	0	0	0	8	0	0	0	0	0	1	0	0	0"));
//    skills.insert(240,new skillz(240,0,"0	0	0	0	0	8	0	0	0	0	0	8	0	0	0"));
//    skills.insert(800,new skillz(800,0,"0	1	0	0	0	4	0	0	0	0	0	1	0	0	0"));
//    skills.insert(811,new skillz(811,0,"0	2	0	0	0	8	0	0	0	0	0	1	0	0	0"));
//    skills.insert(822,new skillz(822,0,"0	3	0	0	0	4	0	0	0	0	0	1	0	0	0"));
//    skills.insert(833,new skillz(833,0,"0	4	0	0	0	8	0	0	0	0	0	11	0	0	0"));
//    skills.insert(844,new skillz(844,0,"0	5	0	0	0	3	0	0	0	0	0	1	0	0	0"));
//    skills.insert(855,new skillz(855,0,"0	6	0	0	2	8	0	0	40	0	0	13	1	0	0"));
//    skills.insert(866,new skillz(866,0,"0	7	0	0	4	8	0	0	0	0	0	11	0	0	0"));
//    skills.insert(877,new skillz(877,0,"0	8	0	0	0	8	0	0	0	0	0	1	0	0	0"));
//    skills.insert(880,new skillz(880,0,"0	9	0	0	2	8	0	0	0	0	0	5	0	0	0"));
//    skills.insert(889,new skillz(889,0,"0	10	0	0	2	8	0	0	0	0	0	11	0	0	0"));
//    skills.insert(900,new skillz(900,0,"0	11	0	0	0	8	0	0	0	0	0	2	1	0	0"));
//    skills.insert(911,new skillz(911,0,"0	12	0	0	2	8	0	0	0	0	0	8	0	0	0"));
//    skills.insert(922,new skillz(922,0,"0	13	0	0	1	7	0	0	0	0	0	6	0	0	0"));
//    skills.insert(933,new skillz(933,0,"0	14	0	0	3	8	0	0	30	0	0	7	0	0	0"));
//    skills.insert(944,new skillz(944,0,"0	15	0	0	1	7	0	0	10	0	0	6	0	0	0"));
//    skills.insert(1049,new skillz(1049,0,"0	16	0	0	1	5	0	0	0	0	0	6	0	0	0")); //Pirat
//    skills.insert(1056,new skillz(1056,0,"0	17	0	0	0	5	0	0	0	0	0	2	0	0	0"));
//    skills.insert(1067,new skillz(1067,0,"0	18	0	0	3	9	0	0	0	0	0	11	0	0	0"));
//    skills.insert(1078,new skillz(1078,0,"0	19	0	0	3	9	0	0	60	0	0	9	1	0	0"));
//    skills.insert(1089,new skillz(1089,0,"0	20	0	0	1	7	0	0	0	0	0	2	0	0	0"));
//    skills.insert(1102,new skillz(1102,0,"0	22	0	0	3	9	0	0	50	0	0	10	1	0	0"));
//    skills.insert(1114,new skillz(1114,0,"0	21	0	0	1	4	0	0	0	0	0	8	0	0	0"));
//    skills.insert(1128,new skillz(1128,0,"0	25	0	0	4	9	0	0	55	0	0	9	0	0	0"));
//    skills.insert(1154,new skillz(1154,0,"0	24	0	0	0	6	0	0	0	0	0	10	0	0	0"));
//    skills.insert(1168,new skillz(1168,0,"0	23	0	0	0	6	0	0	0	0	0	2	0	0	0"));
//    skills.insert(1180,new skillz(1180,0,"0	27	0	0	1	4	0	0	0	0	0	6	0	0	0"));
//    skills.insert(1322,new skillz(1322,0,"0	28	0	0	3	10	0	0	55	0	0	10	1	0	0"));
//    skills.insert(1338,new skillz(1338,0,"0	26	0	0	0	3	0	0	0	0	0	1	0	0	0"));

//    //Buffs
//    skills.insert(272,new skillz(272,12,250,300,10,5));
//    skills.insert(273,new skillz(273,13,400,600,1,5));
//    skills.insert(819,new skillz(819,8,"0	2	0	0	4	700	0	0	220	0	0	0	5	0	0"));
//    skills.insert(928,new skillz(928,6,"0	13	0	0	2	3000	0	0	120	0	0	0	4	0	0"));
//    skills.insert(931,new skillz(931,9,"0	13	0	0	0	3000	0	0	180	0	0	0	4	0	0"));
//    skills.insert(874,new skillz(874,8,"0	7	0	0	8	3000	0	0	150	0	0	0	4	0	0"));
//    skills.insert(875,new skillz(875,9,"0	7	0	0	8	3000	0	0	150	0	0	0	4	0	0"));
//    skills.insert(871,new skillz(871,5,"0	7	0	0	6	350	0	0	140	0	0	0	3	0	0"));
//    skills.insert(873,new skillz(873,7,"0	7	0	0	4	200	0	0	250	0	0	0	4	0	0"));

//    skills.insert(897,new skillz(897,8,"0	10	0	0	12	1800	0	0	100	0	0	0	4	0	0"));
//    skills.insert(898,new skillz(898,9,"0	10	0	0	12	1800	0	0	120	0	0	0	4	0	0"));
//    skills.insert(861,new skillz(861,6,"0	6	0	0	12	3000	0	0	570	0	0	0	5	0	0"));
//    skills.insert(940,new skillz(940,7,"0	14	0	0	12	3000	0	0	550	0	0	0	3	0	0"));
//    skills.insert(1105,new skillz(1105,3,"0	22	0	0	6	3000	0	0	250	0	0	0	4	0	0"));
//    skills.insert(1083,new skillz(1083,7,"0	19	0	0	4	900	0	0	300	0	0	0	5	0	0"));
//    skills.insert(949,new skillz(949,5,"0	15	0	0	10	3000	0	0	160	0	0	0	2	0	0"));
//    skills.insert(1173,new skillz(1173,5,"0	23	0	0	0	1500	0	0	280	0	0	0	4	0	0"));


//    //Jaja
//    skills.insert(880,new skillz(880,0,"0	9	0	0	2	8	0	0	0	0	0	5	0	0	0"));
//    skills.insert(881,new skillz(881,1,"0	9	0	0	4	60	0	0	25	0	0	8	1	0	0"));
//    skills.insert(882,new skillz(882,2,"0	9	0	0	2	140	0	0	55	0	0	3	0	0	0"));
//    skills.insert(883,new skillz(883,3,"0	9	0	0	5	250	0	0	40	0	0	5	2	0	0"));
//    //skills.insert(884,new skill(884,4,"0	9	0	0	4	200	0	0	85	0	0	0	0	0	0"));
//    skills.insert(885,new skillz(885,5,"0	9	0	0	8	1000	0	0	200	0	0	0	0	0	0"));
//    skills.insert(886,new skillz(886,6,"0	9	0	0	5	400	0	0	150	0	0	1	3	0	0"));
//    skills.insert(887,new skillz(887,7,"0	9	0	0	8	550	0	0	300	0	0	5	0	0	0"));
//    skills.insert(888,new skillz(888,8,"0	9	0	0	30	270	0	0	180	0	0	2	0	0	0"));

    skills = nosObjectManager::getSkills();
    Logger::getGlobalLogger()->addLog("NosImporter","Skills imported: " + QString().number(skills.count()));
}

void BattleManager::printSkills()
{
    qDebug()<<"Skills Loaded: "<<skills.count();
    QMapIterator<int, skill*> i(skills);
    while (i.hasNext()) {
        i.next();
        skill *curr = i.value();
        Logger::getGlobalLogger()->addLog("REMOVE",QString("Skill ID: %1 UseID: %4 Name: %2 Type: %3").arg(curr->getSkillId()).arg(curr->getName()).arg(curr->getType()).arg(curr->getUseId()));
    }
}

void BattleManager::attackClosestEnemy()
{
    ply->getLog()->addLog("BATTLE","Attacking closest enemy.");
    int cT = ply->getMap()->getClosestEnemy();
    if(cT != 0){
        ply->getLog()->addLog("BATTLE",QString("Closest enemyid = %1").arg(cT));
        useSkill(aaSkillId,cT);
    }
}

bool BattleManager::isOnCooldown(int id)
{
    if(!skills.contains(id)){
        return false;
    }

    return onCooldown.contains(skills.value(id)->getUseId());
}

void BattleManager::cancelSkill(int id){
    if(onCooldown.contains(id)){
        onCooldown.removeAll(id);
        ply->getLog()->addLog("BATTLE",QString("skill %1 got cancelled").arg(id));
    }
}

void BattleManager::removeCooldown(int id){
    if(onCooldown.contains(id)){
        onCooldown.removeAll(id);
        ply->getLog()->addLog("BATTLE",QString("Removing cooldown for skill %1").arg(id));
    }
    killTarget();
}

void BattleManager::mirrorSkill(const QStringList &list)
{
    if(!ply->getSettings()->getSetting("misc/mimicSkills").toBool())return;

    bool experimental = ply->getSettings()->getSetting("misc/mimicSkillsExperimental").toBool();
    int casterId = list.at(1).toInt();
    int targetType = list.at(2).toInt();
    int targetid = list.at(3).toInt();
    int skillId = list.at(6).toInt();

    //QList<int> autoAttackIds = {260,833};

    if((casterId = ply->getMasterId())){
        if(targetType==3){
            if(experimental&&skills.contains(skillId)){
                if(skills.value(skillId)->getUseId()!=0){
                    useSkill(skillId,targetid,TargetType::Mob,Settings::getDelay("mimic"));
                }else{
                    useSkill(aaSkillId,targetid,TargetType::Mob,Settings::getDelay("mimic"));
                }
            }else{
                useSkill(aaSkillId,targetid,TargetType::Mob,Settings::getDelay("mimic"));
            }
        }else if (targetType==1&&casterId==targetid&&experimental) {
            useSkill(skillId,ply->getId(),TargetType::self,Settings::getDelay("mimic"));
        }
    }
}

void BattleManager::loadAASkillId()
{
    QList<int> temp = ply->getAvailSkills();
    foreach (int skillid, temp) {
        if(skills.contains(skillid)){
            if(skills.value(skillid)->getUseId() == 0 && skills.value(skillid)->getType()==1){
                aaSkillId = skillid;
            }
        }
    }
}

void BattleManager::attackClosestPlayer(){
    setTarget(ply->getMap()->getClosestPlayer(),TargetType::Player);
}

void BattleManager::useAllBuffs()
{
    const QList<int> buffs = nosObjectManager::getBuffs();
    //buffs = {272,273,819,928,931,874,875,871,873,897,898,861,940,1105,1083,949,1173};
    int delay = Settings::getDelay("buff");/*= QRandomGenerator::global()->bounded(400);

    if(Settings::getGlobalSettings()->getSetting("autoBuffDelay").toBool()){
        delay += 2000;
    }*/
    foreach (int curr, buffs) {
        if(usedBuffs.contains(curr)){
            continue;
        }
        if(ply->hasSkill(curr)){
            if(!isOnCooldown(curr)){
                if(skills.value(curr)->getRange()==0){
                    useSkill(curr,ply->getId(),TargetType::self);
                }else{
                    useSkill(curr,ply->getMasterId(),TargetType::Player);
                }
                QTimer::singleShot(delay,this,SLOT(useAllBuffs()));
                usedBuffs.append(curr);
                return;
            }
        }
    }
    usedBuffs.clear();
}

void BattleManager::killTarget(){
    if(currentTarget.id!=0){
        useSkill(aaSkillId,currentTarget.id,currentTarget.type);
    }
}

void BattleManager::setTarget(int id, TargetType type){
    ply->getLog()->addLog("BATTLE","Setting current Target");
    int oldID = currentTarget.id;
    currentTarget.id = id;
    currentTarget.type = type;
    currentTarget.hp = 100;

    if(oldID==0){
        trackTarget();
    }
    killTarget();
}

void BattleManager::trackTarget(){
    if(currentTarget.id!=0){
        if(ply->getMap()->checkIfPlayerExsist(currentTarget.id)){
            if(currentTarget.type == TargetType::Player){
                ply->getNet()->send(QString("ncif 1 %1").arg(currentTarget.id));
                QTimer::singleShot(1000,this,SLOT(trackTarget()));
            }
        }else{
            currentTarget.id = 0;
            ply->getLog()->addLog("BATTLE","Target not found. Did he leave the map?");
        }
    }
}

void BattleManager::stPacketHandler(QStringList packet){
    currentTarget.hp = packet.at(4).toInt();
    ply->getLog()->addLog("BATTLE",QString("Target health: %1 %    %2 HP").arg(currentTarget.hp).arg(packet.at(6)));
    if(packet.at(6).toInt() == 0){
        ply->getLog()->addLog("BATTLE","Target is dead.");
        currentTarget.id = 0;
    }
}


void BattleManager::useSkill(int id, int target, TargetType type,int delay){
    if(!skills.contains(id)){
        ply->getLog()->addLog("BATTLE",QString("Skill with id %1 unknown.").arg(id));
        return;
    }

    skill* currSkill = skills.value(id);

    ply->getLog()->addLog("BATTLE",QString("Trying to use skill %1 on %2").arg(id).arg(target));
    if(!currSkill->getBasic()){
        ply->getLog()->addLog("BATTLE",QString("Skill %1 is no Basic Skill").arg(id));
        //return;
    }


    if(ply->hasSkill(id)){
        ply->getLog()->addLog("BATTLE",QString("Player has skill %1").arg(id));
        if(!isOnCooldown(id)){
            ply->getLog()->addLog("BATTLE",QString("Skill %1 is not on cooldown").arg(id));
            switch (type) {
            case TargetType::self :
                ply->getNet()->sendDelayed(QString("u_s %1 %2 %3").arg(currSkill->getUseId()).arg(1).arg(target),delay);
                onCooldown.append(currSkill->getUseId());
                ply->getLog()->addLog("BATTLE",QString("Skill %1 used").arg(currSkill->getUseId()));
                break;
            case TargetType::Mob :
                if(ply->getMap()->getEnemyDistance(target)<=currSkill->getRange()){
                    ply->getLog()->addLog("BATTLE",QString("Target %1 is in range.").arg(target));
                    ply->getNet()->sendDelayed(QString("u_s %1 %2 %3").arg(currSkill->getUseId()).arg(3).arg(target),delay);
                    onCooldown.append(currSkill->getUseId());
                    ply->getLog()->addLog("BATTLE",QString("Skill %1 used").arg(currSkill->getUseId()));
                }else{
                    ply->getLog()->addLog("BATTLE",QString("Target distance is %1. Skill range is %2").arg(ply->getMap()->getEnemyDistance(target)).arg(currSkill->getRange()));
                }
                break;
            case TargetType::Player :
                if(!ply->getMap()->checkIfPlayerExsist(target)){return;}
                if(ply->getMap()->getPlayerDistance(target)<=currSkill->getRange()){
                    ply->getLog()->addLog("BATTLE",QString("Player %1 is in range.").arg(target));
                    ply->getNet()->sendDelayed(QString("u_s %1 %2 %3").arg(currSkill->getUseId()).arg(1).arg(target),delay);
                    onCooldown.append(currSkill->getUseId());
                    ply->getLog()->addLog("BATTLE",QString("Skill %1 used").arg(currSkill->getUseId()));
                }else{
                    ply->getLog()->addLog("BATTLE",QString("Player distance is %1. Skill range is %2").arg(ply->getMap()->getEnemyDistance(target)).arg(currSkill->getRange()));
                }
                break;
            default:
                break;
            }
        }else{
            ply->getLog()->addLog("BATTLE",QString("Skill %1 is on cooldown").arg(id));
        }
    }else{
        ply->getLog()->addLog("BATTLE",QString("Player has no Skill %1").arg(id));
    }
}

void BattleManager::useSkillSimple(int id)
{
    skill* curr = skills.value(id);
    if(curr->getTargetType()==1||curr->getTargetType()==2){
        useSkill(id,ply->getId(),TargetType::self);
    }else{
        if(curr->getSkillType()==0){
            int enemy = ply->getMap()->getClosestEnemy();
            useSkill(id,enemy,TargetType::Mob);
        }
    }
}


