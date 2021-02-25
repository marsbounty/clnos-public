#include "nosobjectmanager.h"
#include <QFile>
#include <QDebug>
#include <QApplication>
#include "objects/skill.h"
#include "logger.h"

QMap<int,skill *> nosObjectManager::skills;
QList<int> nosObjectManager::buffs;

nosObjectManager::nosObjectManager(QObject *parent) : QObject(parent)
{
    generateSkills();
}

void nosObjectManager::generateSkills()
{
    Logger::getGlobalLogger()->addLog("NosImporter","Importing Skills");
    QFile file(qApp->applicationDirPath()+"/data/Skill.dat");
    if (!file.open(QIODevice::ReadOnly )){
        Logger::getGlobalLogger()->addLog("NosImporter",file.errorString());
        return;
    }
    parseSkill(file.readAll());
}

QMap<int, skill *>& nosObjectManager::getSkills()
{
    return skills;
}

QList<int>& nosObjectManager::getBuffs()
{
    return buffs;
}

void nosObjectManager::parseSkill(QString input)
{
    QStringList tempskills = input.split("#=========================================================");

    foreach (QString tempskill, tempskills) {
        skill* neu = new skill();
        QStringList temp = tempskill.split("\r");
        temp.removeAll(QString(""));
        foreach (QString temp2, temp) {
            QStringList temp3 = temp2.split("\t");
            temp3.removeAll(QString(""));
            QString type = temp3.at(0);
            if(type == "VNUM"){
                neu->setSkillId(temp3.at(1).toInt());
            }else if(type == "NAME"){
                neu->setName(temp3.at(1));
            }else if(type == "DATA"){
                neu->setCastTime(temp3.at(5).toInt());
                neu->setCooldownTime(temp.at(6).toInt());
                neu->setManaCost(temp3.at(9).toInt());
                neu->setRange(temp3.at(12).toInt());
            }else if(type == "TARGET"){
                neu->setTargetType(temp3.at(1).toInt());
                neu->setSkillType(temp3.at(5).toInt());
                if(neu->getSkillType()==2){
                    buffs.append(neu->getSkillId());
                }
            }else if(type == "TYPE"){
                neu->setUseId(temp3.at(2).toInt());
                neu->setType(temp3.at(1).toInt());
            }else if(type == "EFFECT"){
                neu->setEffId(temp3.at(5).toInt());
            }else if(type == "BASIC"){
                if(temp3.at(1).toInt()!=0){
                    temp3.removeAll("0");
                    if(temp3.size()>2){
                        neu->setBasic(false);
                    }
                }
            }
        }
        skills.insert(neu->getSkillId(),neu);

    }

}
