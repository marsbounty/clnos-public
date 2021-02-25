#ifndef GFNETWORKLOGIN_H
#define GFNETWORKLOGIN_H

#include <QObject>

class QNetworkAccessManager;
class QNetworkReply;
class Player;
class QSslError;

class gfNetworkLogin : public QObject
{
    Q_OBJECT
public:
    explicit gfNetworkLogin(Player *ply, QObject *parent = nullptr);
    void startAuth(QString username, QString password, int server);
signals:

public slots:

private slots:
    void recieveToken(QNetworkReply* reply);

    void recieveCode(QNetworkReply *reply);
    void getAccounts(QNetworkReply *reply);

    void sslErrors(QNetworkReply *reply, const QList<QSslError> &errors);
    void recieveToken2(QNetworkReply *reply);
private:
    QNetworkAccessManager* mng;
    int server;
    Player* ply;
    QString username;
    QString token;

};

#endif // GFNETWORKLOGIN_H
