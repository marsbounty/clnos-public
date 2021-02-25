#include "notificationsystem.h"
#include <QSystemTrayIcon>
#include "settings.h"

notificationSystem& notificationSystem::get()
{
    static notificationSystem nS;
    return nS;
}

notificationSystem::~notificationSystem()
{
    tray->hide();
    tray->deleteLater();
}

void notificationSystem::showMessage(QString title,QString msg)
{
    if(Settings::getGlobalSettings()->getSetting("notifications/enabled").toBool()){
        tray->showMessage(title,msg,QIcon(QPixmap(":/icons/ressources/Gold.png")),30000);
    }
}

notificationSystem::notificationSystem(QObject *parent) : QObject(parent)
{
    tray = new QSystemTrayIcon(QIcon(QPixmap(":/icons/ressources/Gold.png")),this);
    tray->show();
    showMessage("CLNos","Started");
    tray->setToolTip("Bot by marsbounty");
}
