#include "networker.h"
#include <QByteArray>
#include <noscrypto.h>
#include <QTimer>
#include "logger.h"
#include "networkmanager.h"
#include "settings.h"

networker::networker(NetworkManager *manager, QObject *parent) : QObject(parent)
{
    isLoginServer = true;
    socket = new QTcpSocket(this);
    crypt = new NosCrypto(this);
    packetID = -1;
    sessionID = 0;
    socket->setReadBufferSize(8000);
    this->manager = manager;
    timeoutTimer = new QTimer(this);
    timeoutTimer->setSingleShot(true);
    socket->setSocketOption(QAbstractSocket:: KeepAliveOption, 1);
    connect(socket,SIGNAL(readyRead()),this,SLOT(recievePacket()));
    connect(socket,SIGNAL(connected()),this,SLOT(connected()));
    connect(socket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(recieveError(QAbstractSocket::SocketError)));
    connect(socket,SIGNAL(disconnected()),this,SLOT(disconnected()));
    connect(timeoutTimer,SIGNAL(timeout()),this,SLOT(timeout()));
    logLogin = Settings::getGlobalSettings()->getSetting("").toBool();
}

networker::~networker()
{
    disconnect(socket,SIGNAL(disconnected()),this,SLOT(disconnected()));
    socket->close();
    delete socket;
    delete crypt;
}

void networker::connectTo(QString ip, qint16 port, bool loginServer)
{
    isLoginServer = loginServer;
    socket->connectToHost(ip,port);
    packetID = -1;
    manager->getLog()->addLog("TCP","Conneting to "+ip+":"+QString::number(port)+" LoginServer:"+QString::number(loginServer));
}

void networker::sendString(QString packet, bool log)
{
    if(socket->isOpen()){
        socket->write(encrypt(QString::number(getPacketID())+" "+packet));
        if(log||logLogin){
            manager->getLog()->addLog("TCP","Sending "+packet);
        }
    }else{
        manager->getLog()->addLog("TCP","Cannot send Packet "+packet+" Socket not Connected!");
    }
}

void networker::sendPacket(QByteArray packet)
{
    socket->write(packet);
    manager->getLog()->addLog("TCP","Sending Byte Array");
}

void networker::login(QString username, QString password , bool isSwitching, bool old_version, int langid)
{
    crypt->setOld_version(old_version);
    manager->getLog()->addLog("CRYPT","Using old version = "+old_version?"true":"false");

    if(isLoginServer){
        manager->getLog()->addLog("TCP","Creating Login Packet");
        if(manager->getGfVersion()){
            sendPacket(crypt->createGFLoginPacket(manager->getUid(),username,langid));
        }else{
            sendPacket(crypt->createLoginPacket(username,password));
        }

    }else{
        if(isSwitching){
            manager->getLog()->addLog("TCP","Logging in to New Server");
            sendPacketID();
            QTimer::singleShot(800, this, SLOT(loginHelper()));
        }else{
            manager->getLog()->addLog("TCP","Loging in");
            sendPacketID();
            QTimer::singleShot(800, this, SLOT(loginHelper()));
        }
    }
}

void networker::recievePacket()
{
    timeoutTimer->start(120000);
    QStringList packets = decrypt(socket->readAll());
    foreach (QString packet, packets) {
        manager->recievePacket(packet.replace("\n","").split(" "));
    }

    //manager->getLog()->addLog("TCP","Recieved Packet");
}

void networker::recieveError(QAbstractSocket::SocketError)
{
    manager->getLog()->addLog("TCP","Error "+socket->errorString());
}

void networker::connected()
{
    crypt->resetVCounter();
    manager->getLog()->addLog("TCP","Connected to Server");
    manager->connectionChanged(true);
}

void networker::disconnected()
{
    manager->getLog()->addLog("TCP","Disconnected from Server");
    manager->connectionChanged(false);
}

void networker::loginHelper()
{
    if(manager->getIsSwitchingServer()){
        sendString("DAC "+manager->getUsername()+" "+QString::number(manager->getCharid()));
    }else{

        manager->send(manager->getUsername()+" GF "+QString::number(manager->getLangid()),true);
        manager->send(manager->getPassword(),false);
    }
}

int networker::getSessionID()
{
    return sessionID;
}

int networker::getPacketID()
{
    if(packetID==-1){
        packetID= crypt->randomNumber(0,65535);
        manager->getLog()->addLog("TCP","Generating Packet ID: "+QString::number(packetID));
    }
    if(packetID == 65535){
        packetID = 0;
    }else{
        packetID++;
    }
    return packetID;
}

void networker::setSessionID(int value)
{
    manager->getLog()->addLog("TCP","Setting Session ID: "+QString::number(value));
    sessionID = value;
}

void networker::close()
{
    socket->close();
}

void networker::sendPacketID()
{
    socket->write(crypt->encryptGamePacket(QString::number(getPacketID())+" "+QString::number(getSessionID()),getSessionID(),true));
}

QStringList networker::decrypt(QByteArray buf)
{

    if(isLoginServer){
        QString temp;
        temp = crypt->decryptLoginPacket(buf,buf.length());
        return {temp};
    }else{
        return crypt->decryptGamePacket(buf,buf.size());
    }

}

QByteArray networker::encrypt(QString inp)
{
    if(!isLoginServer){
        return crypt->encryptGamePacket(inp,getSessionID(),false);
    }
    return "";
}

void networker::timeout()
{
    manager->timeout();
}

bool networker::getIsLoginServer() const
{
    return isLoginServer;
}
