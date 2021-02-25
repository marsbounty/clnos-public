#ifndef NOTIFICATIONSYSTEM_H
#define NOTIFICATIONSYSTEM_H

#include <QObject>

class QSystemTrayIcon;

class notificationSystem : public QObject
{
    Q_OBJECT
public:
    static notificationSystem& get();
    ~notificationSystem();
    void showMessage(QString title, QString msg);

private:
    explicit notificationSystem(QObject *parent = nullptr);
    QSystemTrayIcon* tray;

signals:

public slots:

};

#endif // NOTIFICATIONSYSTEM_H
