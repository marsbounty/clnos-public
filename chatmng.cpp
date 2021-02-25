#include "chatmng.h"
#include "chatui.h"
#include "player.h"
#include "map.h"
#include "notificationsystem.h"
#include "settings.h"
#include "logger.h"
#include "networkmanager.h"
#include <QStringListModel>

chatmng::chatmng(Player *ply,QObject *parent) : QObject(parent),
    ply(ply)
{
    whisperNamesModel = new QStringListModel(this);
    whisperNames.clear();
    chatHistory.clear();
}

void chatmng::addChat(QStringList msg, int type)
{
    QString fmsg;
    //fmsg.append(QString::number(type));

    switch (type) {
    case 0:
        fmsg.append("NORMAL ");
        fmsg.append("["+ply->getMap()->getPlayerNameById(msg[1].toInt())+"]: ");
        msg.removeFirst();
        msg.removeFirst();
        msg.removeFirst();
        fmsg.append(msg.join(" "));
        break;
    case 2:
        fmsg.append("GLOBAL ");
        msg.removeFirst();
        fmsg.append(msg.join(" "));
        break;
    case 3:
    {
        fmsg.append("WHISPER ");
        msg.removeFirst();
        msg.removeFirst();
        msg.removeFirst();
        fmsg.append("["+msg.first()+"]: ");
        QString sender = msg.first();
        addWhisperName(sender);
        msg.removeFirst();
        fmsg.append(msg.join(" "));
        if(Settings::getGlobalSettings()->getSetting("notifications/whisper").toBool()&&sender!=ply->getName()){
            notificationSystem::get().showMessage("WHISPER ALERT",fmsg);
        }
        //ply->getLog()->addLog("CHAT","Recieved Whisper2!");
        break;
    }
    case 4:
        fmsg.append("FAM ");
        msg.removeFirst();
        msg.removeFirst();
        msg.removeFirst();
        fmsg.append(msg.join(" "));
        break;
    default:
        fmsg.append("Not Implemented yet!");
        break;
    }
    if(type==3){
        chatHistory.append(fmsg);
        if(chatHistory.count()>30){
            chatHistory.removeFirst();
        }
    }
    emit showChat(fmsg, type);
}

void chatmng::openchat()
{
    new chatUI(this);
}

void chatmng::sendNormal(QString msg)
{
    if(!msg.isEmpty()){
        ply->getNet()->send("say "+msg);
        emit showChat("NORMAL ["+ply->getName()+"]: "+msg,0);
    }
}

void chatmng::sendWhisper(QString player, QString message)
{
    if(!player.isEmpty()&&!message.isEmpty()){
         ply->getNet()->send("/"+player+" "+message);
    }
}

QAbstractItemModel *chatmng::getWhisperNames()
{
    return whisperNamesModel;
}

QStringList chatmng::getHistory()
{
    return chatHistory;
}

void chatmng::addWhisperName(QString name)
{
    if(!whisperNames.contains(name)){
        whisperNames.append(name);
        whisperNamesModel->setStringList(whisperNames);
    }
}
