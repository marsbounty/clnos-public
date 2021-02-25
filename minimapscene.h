#ifndef MINIMAPSCENE_H
#define MINIMAPSCENE_H

#include <QGraphicsScene>

class MinimapScene : public QGraphicsScene
{
    Q_OBJECT

signals:
    void mapClicked(QPoint pos);
protected:
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
};

#endif // MINIMAPSCENE_H
