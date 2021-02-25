#include "map.h"
#include "player.h"
#include "minimap.h"
#include "nosstrings.h"
#include "logger.h"
#include "networkmanager.h"
#include "playercontroller.h"
#include "misc.h"
#include <QVector2D>
#include <QTimer>

Map::Map(Player *ply, QObject *parent) : QObject(parent),ply(ply)
{
    playerNames.clear();
    players.clear();
    mmap = nullptr;
    dest = {-1,-1};
    walkTime = new QTimer(this);
    walkTime->setSingleShot(true);
    connect(walkTime,SIGNAL(timeout()),this,SLOT(walkHelper()));
}

Map::~Map()
{
    playerNames.clear();
    players.clear();
    delete mmap;
}

void Map::addPlayer(int id, QString name, int x, int y, int sp)
{
    players.insert(id,{name,x,y,id,sp,true});
    playerNames.append(name);
    emit playerSpawned(id ,name, x,y);
}

void Map::addNpc(int id, QString name, int x, int y,int type)
{
    npcs.insert(id,{name,x,y,id,type,false});
    //ply->getLog()->addLog("MAP","Adding npc with id "+QString::number(id));
}

void Map::addPortal(int id, QString name, int x, int y)
{
    portals.insert(id,{name,x,y,id,0,true});
    emit portalSpawned(id,name,x,y);
    ply->getLog()->addLog("MAP","Adding Portal with id "+QString::number(id));
}

void Map::addItem(int id, int x, int y)
{
    items.insert(id,{"",x,y,id,0,false});
}

void Map::addEnemy(int id, int x, int y)
{
    enemies.insert(id,{"",x,y,id,0,true});
    emit enemySpawned(id ,"", x,y);
}

void Map::removeItem(int id){
    if(items.contains(id)){
        items.remove(id);
    }
}

int Map::getItemInRange(int x, int y)
{
    QMapIterator<int, entity> i(items);
    while (i.hasNext()) {
        i.next();
        if(abs(i.value().x-x)<=1&&abs(i.value().y-y)<=1){
            return i.key();
        }
    }
    return 0;
}

int Map::getClosestEnemy(int x, int y)
{
    if(x == -1 || y == -1){
       x = getPlayerById(ply->getId()).x;
       y = getPlayerById(ply->getId()).y;
    }
    int dist = -1;
    int currid = 0;
    QMapIterator<int, entity> i(enemies);
    while (i.hasNext()) {
        i.next();
        int ndist = QVector2D(abs(i.value().x-x),abs(i.value().y-y)).length();
        if(dist == -1){
            dist = ndist;
            currid = i.value().id;
        }else{
            if(ndist<dist){
                dist = ndist;
                currid = i.value().id;
            }
        }
    }
    return currid;
}

int Map::getClosestPlayer(int x, int y)
{
    if(x == -1 || y == -1){
       x = getPlayerById(ply->getId()).x;
       y = getPlayerById(ply->getId()).y;
    }
    int dist = -1;
    int currid = 0;
    QMapIterator<int, entity> i(players);
    while (i.hasNext()) {
        i.next();
        if(i.value().id == ply->getId()){
            continue;
        }
        int ndist = QVector2D(abs(i.value().x-x),abs(i.value().y-y)).length();
        if(dist == -1){
            dist = ndist;
            currid = i.value().id;
        }else{
            if(ndist<dist){
                dist = ndist;
                currid = i.value().id;
            }
        }
    }
    return currid;
}

int Map::getClosestPortal(int x, int y)
{
    int dist = -1;
    int currid = 0;
    QMapIterator<int, entity> i(portals);
    while (i.hasNext()) {
        i.next();
        int ndist = QVector2D(abs(i.value().x-x),abs(i.value().y-y)).length();
        if(dist == -1){
            dist = ndist;
            currid = i.value().id;
        }else{
            if(ndist<dist){
                dist = ndist;
                currid = i.value().id;
            }
        }
    }
    return currid;
}

void Map::stopWalking()
{
    if(walkTime->isActive()){
        walkTime->stop();
    }
}

void Map::setNPCName(int id, QString name)
{
    if(npcs.contains(id)){
        npcs[id].name = name;
        npcs[id].visible = true;
        emit npcSpawned(id,name,npcs[id].x,npcs[id].y);
    }
}

void Map::removePlayer(int id)
{
    if(players.contains(id)){
        QString search = players.value(id).name;
        for (int i = 0; i < playerNames.size(); ++i)
        {
            if(playerNames.value(i)==search){
                playerNames.removeAt(i);
            }
        }
        players.remove(id);
        emit playerDespawned(id);
    }
}

void Map::removeEnemy(int id)
{
     if(enemies.contains(id)){
         enemies.remove(id);
         emit enemyDespawned(id);
     }
}

void Map::updatePlayerPos(int id, int x, int y)
{
    if(players.contains(id)){
        players[id].x = x;
        players[id].y = y;
        emit playerMoved(id,x,y);
    }else{
        players.insert(id,{"Unknown",x,y,id,0,true});
    }
}

void Map::updateEnemyPos(int id, int x, int y)
{
    if(enemies.contains(id)){
        enemies[id].x = x;
        enemies[id].y = y;
        emit enemyMoved(id,x,y);
    }else{
        enemies.insert(id,{"",x,y,id,0,true});
    }
}

void Map::setPlayerSp(int id, int sp)
{
    if(players.contains(id)){
        players[id].sp = sp;
    }
}

void Map::clear()
{
    entity backup = players.value(ply->getId());
    playerNames.clear();
    players.clear();
    npcs.clear();
    portals.clear();
    items.clear();
    enemies.clear();
    players.insert(ply->getId(),backup);
}

void Map::showMinimap()
{
    if(!mmap){
       mmap = new Minimap(ply);
    }else{
        mmap->activateWindow();
    }
}

QMap<int, entity> &Map::getPlayerMap()
{
    return players;
}

QMap<int, entity> &Map::getNpcMap()
{
    return npcs;
}

QMap<int, entity> &Map::getEnemyMap()
{
    return enemies;
}

QMap<int, entity> &Map::getPortalMap()
{
    return portals;
}

int Map::getMapId() const
{
    return mapId;
}

void Map::setMapId(int value)
{
    mapId = value;
    currmap = QPixmap(NosStrings::getPath()+"/maps/"+QString::number(value)+".png").toImage();
    stopWalking();
    clear();
    ply->getLog()->addLog("MAP","Map changed to "+NosStrings::getMapName(mapId)+" ("+QString::number(mapId)+") ");
    emit mapChanged(value);
}

void Map::mapout()
{
    clear();
}

void Map::hideMinimap()
{
    mmap = nullptr;
}

void Map::moveTo(QPoint pos,int delay)
{
    if(walkTime->isActive()){
        ply->getLog()->addLog("MOVE","Dest: "+QString::number(pos.x())+" "+QString::number(pos.y()));
        dest = pos;
    }else{
        entity tempPly = players.value(ply->getId());
        QPoint pos2 = {tempPly.x,tempPly.y};
        if(ply->getMisc()->isBusy()){
            ply->getLog()->addLog("MOVE","Player is curently busy!");
        }else if(pos2 != pos){
            ply->getLog()->addLog("MOVE","Dest: "+QString::number(pos.x())+" "+QString::number(pos.y()));
            dest = pos;
            QTimer::singleShot(delay,this,&Map::walkHelper);
        }
    }
}

QPoint Map::getPlayerMassPoint()
{
    int x = 0;
    int y = 0;
    int i = 0;
    foreach (entity ent, players) {
        if(!PlayerController::getPC()->entityIsBot(ent.name)){
            x += ent.x;
            y += ent.y;
            i++;
        }
    }
    x = x/i;
    y = y/i;

    QPoint temp = QPoint(x,y);

    x = 0;
    y = 0;
    i = 0;

    foreach (entity ent, players) {
        if((QPoint(ent.x,ent.y)-temp).manhattanLength()<8){
            if(!PlayerController::getPC()->entityIsBot(ent.name)){
                x += ent.x;
                y += ent.y;
                i++;
            }
        }
    }
    if(i>0){
        x = x/i;
        y = y/i;
    }else{
        x = temp.rx();
        y = temp.ry();
    }

    return QPoint(x,y);
}

bool Map::isPortal(QPoint pos)
{
    foreach (entity portal, portals) {
        if((abs(portal.x-pos.x())<=2)&&(abs(portal.y-pos.y())<=2)){
            return true;
        }
    }
    return false;
}

entity Map::getPortalByPos(QPoint pos)
{
    foreach (entity portal, portals) {
        if((abs(portal.x-pos.x())<=2)&&(abs(portal.y-pos.y())<=2)){
            return portal;
        }
    }
    return {};
}

int Map::getPlayerCount()
{
    return players.count();
}

QString Map::getPlayerNameById(int id)
{
    if(players.contains(id)){
        return players.value(id).name;
    }else{
        return "";
    }
}

int Map::getPlayerIdByName(QString name)
{
    foreach (entity ent, players) {
        if(ent.name==name){
            return ent.id;
        }
    }
    return 0;
}

entity Map::getPlayerById(int id)
{
    if(players.contains(id)){
        return players.value(id);
    }
    return {};
}

entity Map::getNpcById(int id)
{
    if(npcs.contains(id)){
        return npcs.value(id);
    }
    return {};
}

entity Map::getNpcByName(QString name)
{
    foreach (entity ent, npcs) {
        if(ent.name==name){
            return ent;
        }
    }
    return {};
}

entity Map::getNpcByType(int type)
{
    foreach (entity ent, npcs) {
        if(ent.sp==type){
            return ent;
        }
    }
    return {};
}

int Map::getEnemyDistance(int id)
{
    entity enemy = getEnemyById(id);
    entity player = getPlayerById(ply->getId());
    return QPoint(player.x-enemy.x,player.y-enemy.y).manhattanLength();
}

int Map::getPlayerDistance(int id)
{
    entity enemy = getPlayerById(id);
    entity player = getPlayerById(ply->getId());
    return QPoint(player.x-enemy.x,player.y-enemy.y).manhattanLength();
}

entity Map::getEnemyById(int id)
{
    if(enemies.contains(id)){
        return enemies.value(id);
    }
    return {};
}

entity Map::getPortalById(int id)
{
    if(portals.contains(id)){
        return portals.value(id);
    }
    return {};
}

bool Map::checkIfNpcExsist(int id)
{
    return npcs.contains(id);
}

bool Map::checkIfPlayerExsist(int id)
{
    return players.contains(id);
}

void Map::walkHelper()
{
    entity tempPly = players.value(ply->getId());
    QPoint pos = {tempPly.x,tempPly.y};
    int speed = ply->getSpeed();
    if(pos == dest){
        ply->getLog()->addLog("MOVE","Arrived at destination");
        walkTime->stop();
        if(isPortal(pos)){
            ply->getNet()->send("preq");
        }
    }else{
        QVector2D path(dest-pos);
        if(path.length()<=4){
            sendWalkPacket(dest);
        }else{
            path = path.normalized()*4;
            QPoint tempDest = pos + path.toPoint();
            sendWalkPacket(tempDest);
        }
        walkTime->start(500 + (static_cast<int>(path.length()) * (1000.0 / speed)));
    }
    ply->getMisc()->tryCollect();
}

void Map::sendWalkPacket(QPoint pos)
{
    int speed = ply->getSpeed();
    bool isVendetta = ply->getIsOldVersion();
    int lvl = ply->getLvl();
    QString VendettaAddition = QString::number((pos.x()+pos.y())*(lvl+pos.x()));
    int var3 = ((pos.x()+pos.y())%3)%2;
    QColor tempColor = QColor::fromRgb(currmap.pixel(pos));
    if(tempColor == Qt::white){
        updatePlayerPos(ply->getId(),pos.x(),pos.y());
        if(isVendetta){
            ply->getNet()->send("walk "+QString::number(pos.x())+" "+QString::number(pos.y())+" "+QString::number(var3)+" "+QString::number(speed)+" "+VendettaAddition);
        }else{
            ply->getNet()->send("walk "+QString::number(pos.x())+" "+QString::number(pos.y())+" "+QString::number(var3)+" "+QString::number(speed));
        }

    }else{
        entity tempPly = players.value(ply->getId());
        QPoint pos = {tempPly.x,tempPly.y};
        dest = pos;
    }
}
