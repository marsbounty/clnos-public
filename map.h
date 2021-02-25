#ifndef MAP_H
#define MAP_H

#include <QObject>
#include <QMap>
#include <QImage>
#include <QTimer>

class Player;
class Minimap;

struct entity{
    QString name;
    int x;
    int y;
    int id;
    int sp;
    bool visible;
};

class Map : public QObject
{
    Q_OBJECT
public:
    explicit Map(Player* ply,QObject *parent = nullptr);
    virtual ~Map();
    void addPlayer(int id, QString name, int x, int y, int sp = 0);
    void addNpc(int id,QString name,int x,int y,int type);
    void addPortal(int id,QString name,int x,int y);
    void addItem(int id,int x,int y);
    void addEnemy(int id,int x, int y);
    void setNPCName(int id,QString name);
    void removePlayer(int id);
    void removeEnemy(int id);
    void updatePlayerPos(int id,int x,int y);
    void updateEnemyPos(int id,int x,int y);
    void setPlayerSp(int id, int sp);
    void clear();
    void showMinimap();
    QMap<int,entity>& getPlayerMap();
    QMap<int,entity>& getNpcMap();
    QMap<int,entity>& getEnemyMap();
    QMap<int, entity> &getPortalMap();
    int getMapId() const;
    void setMapId(int value);
    void mapout();
    void hideMinimap();
    void moveTo(QPoint pos, int delay =0);
    QPoint getPlayerMassPoint();
    bool isPortal(QPoint pos);
    entity getPortalByPos(QPoint pos);
    int getPlayerCount();
    QString getPlayerNameById(int id);
    int getPlayerIdByName(QString name);
    entity getPlayerById(int id);
    entity getNpcById(int id);
    entity getEnemyById(int id);
    entity getPortalById(int id);
    bool checkIfNpcExsist(int id);
    void removeItem(int id);
    int getItemInRange(int x, int y);
    int getClosestEnemy(int x  = -1, int y = -1);
    int getClosestPortal(int x, int y);
    void stopWalking();
    entity getNpcByName(QString name);
    entity getNpcByType(int type);
    int getEnemyDistance(int id);
    int getPlayerDistance(int id);
    bool checkIfPlayerExsist(int id);
    int getClosestPlayer(int x = -1, int y = -1);
signals:
    void playerSpawned(int id,QString name, int x ,int y);
    void playerMoved(int id, int x, int y);
    void playerDespawned(int id);

    void enemySpawned(int id,QString name, int x ,int y);
    void enemyMoved(int id, int x, int y);
    void enemyDespawned(int id);

    void mapChanged(int id);
    void npcSpawned(int id,QString name, int x ,int y);
    void portalSpawned(int id,QString name, int x,int y);

private slots:
    void walkHelper();

private:
    QMap<int,entity> players;
    QMap<int,entity> npcs;
    QMap<int,entity> portals;
    QMap<int,entity> items;
    QMap<int,entity> enemies;
    QStringList playerNames;
    Player* ply;
    int mapId;
    Minimap *mmap;
    QImage mapImage;
    QTimer *walkTime;
    QPoint dest;
    QImage currmap;


    void sendWalkPacket(QPoint pos);
};

#endif // MAP_H
