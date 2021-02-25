#include "minimapscene.h"

#include <QGraphicsSceneMouseEvent>
#include <QDebug>

void MinimapScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    //qDebug() << "Custom scene clicked."<<event->scenePos();
    if((event->scenePos().x()>=0)&&(event->scenePos().y()>=0)){
        if((event->scenePos().x()<=width())&&(event->scenePos().y()<=height())){
            emit mapClicked(event->scenePos().toPoint());
        }else{
            //qDebug()<<"Point not on the Map +";
        }
    }else{
        //qDebug()<<"Point not on the Map -";
    }
    //qDebug()<<event->scenePos().x()<<event->scenePos().y();
}
