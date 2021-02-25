#ifndef MINIMAP_H
#define MINIMAP_H

#include <QDialog>
#include <QGraphicsPixmapItem>

namespace Ui {
class Minimap;
}

class Player;
class MinimapScene;


class Minimap : public QDialog
{
    Q_OBJECT

public:
    explicit Minimap(Player* ply,QWidget *parent = 0);
    ~Minimap();
    void rebuild();
    int toMap(int input);
    int fromMap(int input);

private slots:
    void playerSpawned(int id ,QString name,int x ,int y);
    void playerDespawned(int id);
    void playerMoved(int id, int x, int y);
    void enemySpawned(int id ,QString name,int x ,int y);
    void enemyDespawned(int id);
    void enemyMoved(int id, int x, int y);
    void portalSpawned(int id ,QString name, int x,int y);
    void mapChanged(int id);
    void on_showPlayers_clicked(bool checked);
    void mapClicked(QPoint pos);
    void npcSpawned(int id, QString name, int x, int y);

    void on_scale_valueChanged(int value);

    void on_showEnemies_toggled(bool checked);

private:
    Ui::Minimap *ui;
    Player *ply;
    MinimapScene *scene;
    QGraphicsPixmapItem* bg;
    QMap<int,QGraphicsItem*> players;
    QMap<int,QGraphicsItem*> npcs;
    QMap<int,QGraphicsItem*> portals;
    QMap<int,QGraphicsItem*> enemies;
    QPixmap currentMap;
    void clear();
    void removeEntitys();
    void hidePlayers(bool v);
    int scale;
    void updatePlayerList();
    QStringList playerNamesList;
    void addToPlayerList(QString name);
    void removeFromPlayerList(QString name);
    void hideEnemies(bool v);
};

#endif // MINIMAP_H
