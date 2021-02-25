#include "minimap.h"
#include "ui_minimap.h"
#include "minimapscene.h"
#include "player.h"
#include "nosstrings.h"
#include "logger.h"
#include "map.h"
#include "settings.h"
#include <QMap>
#include <QDebug>


Minimap::Minimap(Player* ply,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Minimap),
    ply(ply),
    scene(new MinimapScene)
{
    ui->setupUi(this);
    open();
    setWindowTitle("Minimap - "+ply->getName()+" - "+NosStrings::getMapName(ply->getMap()->getMapId()));
    setAttribute(Qt::WA_DeleteOnClose);
    bg = scene->addPixmap(NosStrings::getPath()+"/maps/"+QString::number(ply->getMap()->getMapId())+".png");
    scale = 1;

    setWindowIcon(QIcon(QPixmap(":/icons/ressources/mapicon.png")));

    rebuild();

    connect(ply->getMap(),SIGNAL(playerSpawned(int,QString,int,int)),this,SLOT(playerSpawned(int,QString,int,int)));
    connect(ply->getMap(),SIGNAL(playerDespawned(int)),this,SLOT(playerDespawned(int)));
    connect(ply->getMap(),SIGNAL(playerMoved(int,int,int)),this,SLOT(playerMoved(int,int,int)));


    connect(ply->getMap(),SIGNAL(enemySpawned(int,QString,int,int)),this,SLOT(enemySpawned(int,QString,int,int)));
    connect(ply->getMap(),SIGNAL(enemyDespawned(int)),this,SLOT(enemyDespawned(int)));
    connect(ply->getMap(),SIGNAL(enemyMoved(int,int,int)),this,SLOT(enemyMoved(int,int,int)));

    connect(ply->getMap(),SIGNAL(mapChanged(int)),this,SLOT(mapChanged(int)));
    connect(ply->getMap(),SIGNAL(portalSpawned(int,QString,int,int)),this,SLOT(portalSpawned(int,QString,int,int)));
    connect(ply->getMap(),SIGNAL(npcSpawned(int, QString,int,int)),this,SLOT(npcSpawned(int,QString,int,int)));
    connect(scene,SIGNAL(mapClicked(QPoint)),this,SLOT(mapClicked(QPoint)));

    ui->minimap->setScene(scene); 
}

Minimap::~Minimap()
{
    ply->getMap()->hideMinimap();
    clear();
    delete ui;
}

void Minimap::rebuild()
{
    removeEntitys();
    bg = scene->addPixmap(NosStrings::getPath()+"/maps/"+QString::number(ply->getMap()->getMapId())+".png");
    bg->setScale(scale);
    bg->setPos(0,0);
    bg->setZValue(-1);
    scene->setSceneRect(0,0,bg->sceneBoundingRect().width(),bg->sceneBoundingRect().height());
    playerNamesList.clear();

    QMap<int,entity> tempPlayers = ply->getMap()->getPlayerMap();
    QMapIterator<int,entity> i(tempPlayers);
    while (i.hasNext()) {
        i.next();
        if(i.key()==ply->getId()){
            QBrush blueBrush(Qt::green);
            QPen blackPen(Qt::black);
            blackPen.setWidth(1);
            QGraphicsEllipseItem* elipse = scene->addEllipse(-2,-2,5,5,blackPen,blueBrush);
            elipse->setToolTip(i.value().name);
            elipse->setZValue(1);
            elipse->setPos(toMap(i.value().x),toMap(i.value().y));
            players.insert(i.key(),elipse);
        }else{
            QBrush blueBrush;
            if(i.key()==ply->getSettings()->getSetting("misc/masterid").toInt()){
                blueBrush = QBrush(Qt::green);
            }else{
                blueBrush = QBrush(Qt::lightGray);
            }
            QPen blackPen(Qt::black);
            blackPen.setWidth(1);
            QGraphicsEllipseItem* elipse = scene->addEllipse(-1,-1,3,3,blackPen,blueBrush);
            elipse->setToolTip(i.value().name);
            addToPlayerList(i.value().name);
            elipse->setPos(toMap(i.value().x),toMap(i.value().y));
            players.insert(i.key(),elipse);
        }
    }

    QMap<int,entity> tempNpcs = ply->getMap()->getNpcMap();
    QMapIterator<int,entity> npcite(tempNpcs);
    while (npcite.hasNext()) {
        npcite.next();
        if(npcite.value().visible==true){
            QBrush brush(Qt::darkMagenta);
            QPen blackPen(Qt::black);
            blackPen.setWidth(1);
            QGraphicsRectItem* npc = scene->addRect(-2,-2,5,5,blackPen,brush);
            npc->setPos(toMap(npcite.value().x),toMap(npcite.value().y));
            npc->setZValue(0.5);
            npc->setToolTip(npcite.value().name);
            npcs.insert(npcite.key(),npc);
        }
    }

    QMap<int,entity> tempEnemys = ply->getMap()->getEnemyMap();
    QMapIterator<int,entity> nmyite(tempEnemys);
    while (nmyite.hasNext()) {
        nmyite.next();
        if(nmyite.value().visible==true){
            QBrush brush(Qt::red);
            QPen blackPen(Qt::red);
            blackPen.setWidth(0);
            QGraphicsRectItem* nmy = scene->addRect(0,0,1,1,blackPen,brush);
            nmy->setPos(toMap(nmyite.value().x),toMap(nmyite.value().y));
            nmy->setZValue(0.5);
            nmy->setToolTip(nmyite.value().name);
            if(!ui->showEnemies->isChecked()){
                nmy->hide();
            }
            enemies.insert(nmyite.key(),nmy);
        }
    }

    QMap<int,entity> tempPortals = ply->getMap()->getPortalMap();
    QMapIterator<int,entity> portalite(tempPortals);
    while (portalite.hasNext()) {
        portalite.next();
        QBrush brush(Qt::red);
        QPen blackPen(Qt::black);
        blackPen.setWidth(1);
        QGraphicsEllipseItem* portal = scene->addEllipse(-2,-2,5,5,blackPen,brush);
        portal->setPos(toMap(portalite.value().x),toMap(portalite.value().y));
        portal->setZValue(0.5);
        portal->setToolTip(portalite.value().name);
        portals.insert(portalite.key(),portal);
    }
}

void Minimap::playerSpawned(int id,QString name, int x, int y)
{

    if(id==ply->getId()){
        QBrush blueBrush(Qt::green);
        QPen blackPen(Qt::black);
        blackPen.setWidth(1);
        QGraphicsEllipseItem* elipse = scene->addEllipse(-2,-2,5,5,blackPen,blueBrush);
        elipse->setToolTip(name);
        elipse->setZValue(1);
        elipse->setPos(toMap(x),toMap(y));
        players.insert(id,elipse);
    }else{
        QBrush blueBrush;
        if(id==ply->getSettings()->getSetting("misc/masterid").toInt()){
            blueBrush = QBrush(Qt::green);
        }else{
            blueBrush = QBrush(Qt::lightGray);
        }
        QPen blackPen(Qt::black);
        blackPen.setWidth(1);
        QGraphicsEllipseItem* elipse = scene->addEllipse(-1,-1,3,3,blackPen,blueBrush);
        elipse->setToolTip(name);
        elipse->setPos(toMap(x),toMap(y));
        if(!ui->showPlayers->isChecked()){
            elipse->hide();
        }
        players.insert(id,elipse);
        addToPlayerList(name);
    }
}

void Minimap::playerDespawned(int id)
{
    if(players.contains(id)){
        players[id]->hide();
        removeFromPlayerList(players[id]->toolTip());
        delete players[id];
        players.remove(id);
    }
}

void Minimap::addToPlayerList(QString name){
    if(ui->playerList->findItems(name,Qt::MatchExactly).count()==0){
        ui->playerList->addItem(name);
        ui->playerList->sortItems();
    }
}

void Minimap::removeFromPlayerList(QString name){
    QList<QListWidgetItem *> temp = ui->playerList->findItems(name,Qt::MatchExactly);
    foreach (QListWidgetItem* w, temp) {
        ui->playerList->removeItemWidget(w);
    }
}

void Minimap::playerMoved(int id, int x, int y)
{
    if(players.contains(id)){
        players[id]->setPos(toMap(x),toMap(y));
    }
}

void Minimap::enemySpawned(int id, QString name, int x, int y)
{
    if(enemies.contains(id)){
        return;
    }

    QBrush brush(Qt::red);
    QPen blackPen(Qt::red);
    blackPen.setWidth(0);
    QGraphicsRectItem* nmy = scene->addRect(0,0,1,1,blackPen,brush);
    nmy->setPos(toMap(x),toMap(y));
    nmy->setZValue(0.5);
    nmy->setToolTip(name);
    if(!ui->showEnemies->isChecked()){
        nmy->hide();
    }
    enemies.insert(id,nmy);
}

void Minimap::enemyDespawned(int id)
{
    if(enemies.contains(id)){
        enemies[id]->hide();
        delete enemies[id];
        enemies.remove(id);
    }
}

void Minimap::enemyMoved(int id, int x, int y)
{
    if(enemies.contains(id)){
        enemies[id]->setPos(toMap(x),toMap(y));
    }
}

void Minimap::portalSpawned(int id, QString name, int x, int y)
{
    QBrush brush(Qt::red);
    QPen blackPen(Qt::black);
    blackPen.setWidth(1);
    QGraphicsEllipseItem* portal = scene->addEllipse(-2,-2,5,5,blackPen,brush);
    portal->setPos(toMap(x),toMap(y));
    portal->setZValue(0.5);
    portal->setToolTip(name);
    portals.insert(id,portal);
}

void Minimap::npcSpawned(int id, QString name, int x, int y)
{
    QBrush brush(Qt::darkMagenta);
    QPen blackPen(Qt::black);
    blackPen.setWidth(1);
    QGraphicsRectItem* npc = scene->addRect(-2,-2,5,5,blackPen,brush);
    npc->setPos(toMap(x),toMap(y));
    npc->setZValue(0.5);
    npc->setToolTip(name);
    npcs.insert(id,npc);
}

void Minimap::mapChanged(int id)
{
    playerDespawned(ply->getId());
    removeEntitys();
    //bg = scene->addPixmap(NosStrings::getPath()+"/maps/"+QString::number(id)+".png");
    //scene->setSceneRect(0,0,bg->sceneBoundingRect().width(),bg->sceneBoundingRect().height());
    //qDebug()<<scene->sceneRect().width()<<scene->sceneRect().height();
    rebuild();
}

void Minimap::clear()
{
    removeEntitys();
    scene->deleteLater();
    //delete(bg);
}

void Minimap::removeEntitys()
{
    ui->playerList->clear();
    scene->clear();
}

void Minimap::hidePlayers(bool v)
{
    QMapIterator<int,QGraphicsItem*> i(players);
    while (i.hasNext()) {
        i.next();
        if(!(i.key()==ply->getId())){
            if(v){
                i.value()->hide();
            }else{
                i.value()->show();
            }
        }
    }
}


void Minimap::hideEnemies(bool v)
{
    QMapIterator<int,QGraphicsItem*> i(enemies);
    while (i.hasNext()) {
        i.next();
        if(!(i.key()==ply->getId())){
            if(v){
                i.value()->hide();
            }else{
                i.value()->show();
            }
        }
    }
}



void Minimap::on_showPlayers_clicked(bool checked)
{
    hidePlayers(!checked);
}

void Minimap::mapClicked(QPoint pos)
{
    if(ui->clickToMove->isChecked()){
        QPoint tf = QPoint(fromMap(pos.x()),fromMap(pos.y()));
        ply->getMap()->moveTo(tf);
    }
}

int Minimap::toMap(int input)
{

    return input*scale;
}

int Minimap::fromMap(int input)
{
    return input/scale;
}

void Minimap::on_scale_valueChanged(int value)
{
    scale = value;
    rebuild();
}

void Minimap::on_showEnemies_toggled(bool checked)
{
    hideEnemies(!checked);
}
