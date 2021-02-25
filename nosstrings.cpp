#include "nosstrings.h"
#include "logger.h"
#include <QFile>
#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QTextDecoder>
#include "nosobjectmanager.h"
#include "objects/skill.h"

QMap<int,QString> NosStrings::maps;
QMap<int,QString> NosStrings::items;
QStringList NosStrings::itemNames;
QMap<int,int> NosStrings::buffs;

NosStrings::NosStrings(QObject *parent) : QObject(parent)
{
}

void NosStrings::initialize(QString lang)
{
    maps.clear();
    items.clear();
    itemNames.clear();
    buffs.clear();

    loadFromFile(lang,"maps");
    loadFromFile(lang,"items");
    loadFromFile(lang,"skills");


    Logger::getGlobalLogger()->addLog("LANG",QString::number(maps.count())+" Maps loaded.");
    Logger::getGlobalLogger()->addLog("LANG",QString::number(items.count())+" Items loaded.");

    buffs.insert(272,30);
    buffs.insert(273,60);
    buffs.insert(819,70); //Moral
    buffs.insert(928,300); //Wolf
    buffs.insert(931,300); //Bear
    buffs.insert(874,100); //Holy Wep
    buffs.insert(875,150);  //Holy Bless
    buffs.insert(871,120); //holy Cure All
}

QString NosStrings::getItemName(int id)
{
    if(items.contains(id)){
        return items.value(id);
    }else{
        Logger::getGlobalLogger()->addLog("LANG","Item with ID "+QString::number(id)+" not found.");
        //qDebug()<<id<<"Not found in Item List";
        return "Unknown";
    }
}

int NosStrings::getItemId(QString name)
{
    QMapIterator<int, QString> i(items);
    while (i.hasNext()) {
        i.next();
        if(i.value()==name){
            return i.key();
        }
    }
    return 0;

}

QString NosStrings::getMapName(int id)
{
    if(maps.contains(id)){
        return maps.value(id);
    }else{
        Logger::getGlobalLogger()->addLog("LANG","Map with ID "+QString::number(id)+" not found.");
        //qDebug()<<id<<"not found in Maps List";
        return QString::number(id);
    }
}

QStringList &NosStrings::getItemNames()
{
    return itemNames;
}

QString NosStrings::getPath()
{
    return qApp->applicationDirPath();
}

QStringList NosStrings::getLanguages()
{
    QStringList out;
    QDir directory(getPath()+"/lang/");
    QStringList langs = directory.entryList(QStringList() << "*.txt",QDir::Files);
    foreach(QString filename, langs) {
        QStringList temp = filename.split("_");
        if(temp.count()==2){
            if(!out.contains(temp.at(0))){
                out.append(temp.at(0));
            }
        }
    }
    return out;
}

void NosStrings::loadFromFile(QString language, QString type)
{
    QString path = qApp->applicationDirPath()+"/lang/"+language.toLower()+"_"+type.toLower()+".txt";

    if(!QFile::exists(path)){
         path = qApp->applicationDirPath()+"/lang/english_"+type.toLower()+".txt";
         Logger::getGlobalLogger()->addLog("LANG",QString("%1_%2 doesn't exist falling back to english!").arg(language).arg(type));
         language = "english";
    }

    QFile inputFile(path);
    if (!inputFile.open(QIODevice::ReadOnly))
    {
        Logger::getGlobalLogger()->addLog("LANG","Failed to open "+language+" file for "+type);
        return;
    }
    QByteArray data = inputFile.readAll();
    QTextDecoder* decoder = QTextCodec::codecForName("Windows-1250")->makeDecoder();

    //QStringList temp = QString::fromLatin1(inputFile.readAll()).split('\n');
    QStringList temp = decoder->toUnicode(data,data.length()).split('\n');



    int i;
    for(i = 0;i<temp.count()-1;i++)
    {
        QStringList temp2 = temp[i].split('\t');
        if(type=="maps"){
            NosStrings::maps.insert(temp2[0].toInt(),temp2[1]);
        }else if(type == "items"){
            itemNames.append(temp2[1]);
            NosStrings::items.insert(temp2[0].toInt(),temp2[1]);
        }else if(type=="skills"){
            if(nosObjectManager::getSkills().contains(temp2[0].toInt())){
                nosObjectManager::getSkills().value(temp2[0].toInt())->setName(temp2[1]);
            }
        }
    }
    inputFile.close();
}
