#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QStringList>
#include <QMap>

class networker;
class Player;
class Logger;
class QTimer;


class NetworkManager : public QObject
{
    Q_OBJECT
    typedef void (NetworkManager::*packetHandler)(QStringList);
public:
    explicit NetworkManager(Player *ply, QObject *parent = nullptr);
    virtual ~NetworkManager();

    void login(QString username, QString password, QString ip, qint16 port, bool oldversion=false);
    void connectToGame(QString ip,qint16 port);
    void connectionChanged(bool connected);
    void selectChar(int id);
    void logout();

    void send(QString msg, bool log = true);

    void recievePacket(QStringList packet);
    Logger *getLog();
    QString getUsername() const;
    QString getPassword() const;

    int getCharid() const;

    bool getIsSwitchingServer() const;
    void timeout();
    void gfLogin(QString id, int port, int locale);

    bool getGfVersion() const;

    QString getUid() const;

    void sendDelayed(QString msg, int delay, bool log = true);
    int getLangid() const;

signals:
    void packetLogRecv(QStringList packet);
    void packetLogSend(QStringList packet);
    void recievedQnaml(QStringList packet);
    void recievedDlg(QStringList packet);

public slots:

private slots:
    void sendKeepAlive();

private:
    networker *net;
    QString username;
    QString password;
    QString serverip;
    int port;
    int charid;
    Player *ply;
    QTimer *keepAlive;
    bool isSwitchingServer;
    bool old_version;
    bool gfVersion;
    QString uid;

    QMap<QString,packetHandler> packetTypes;
    void generateHwId();


    int state;
    int runtime;
    bool dcExpected;
    void setState(int i);
    int langid;

    //Packet Handlers
    void fillPacketTypes();
    void fail(QStringList packet);
    void info(QStringList packet);
    void nstest(QStringList packet);
    void clist(QStringList packet);
    void clist_end(QStringList packet);
    void ok(QStringList packet);
    void cinfo(QStringList packet);
    void lev(QStringList packet);
    void mz(QStringList packet);
    void pclear(QStringList packet);
    void gold(QStringList packet);
    void in(QStringList packet);
    void out(QStringList packet);
    void mv(QStringList packet);
    void at(QStringList packet);
    void qnaml(QStringList packet);
    void dlg(QStringList packet);
    void shop(QStringList packet);
    void gp(QStringList packet);
    void cond(QStringList packet);
    void rc_blist(QStringList packet);
    void rc_slist(QStringList packet);
    void inv(QStringList packet);
    void rc_buy(QStringList packet);
    void exc_list(QStringList packet);
    void fc(QStringList packet);
    void umi(QStringList packet);
    void delay(QStringList packet);
    void msg(QStringList packet);
    void csp(QStringList packet);
    void say(QStringList packet);
    void spk(QStringList packet);
    void npc_req(QStringList packet);
    void ski(QStringList packet);
    void drop(QStringList packet);
    void get(QStringList packet);
    void bsinfo(QStringList packet);
    void die(QStringList packet);
    void sh_o(QStringList packet);
    void revive(QStringList packet);
    void sh_c(QStringList packet);
    void wopen(QStringList packet);
    void stat(QStringList packet);
    void qna(QStringList packet);
    void c_mode(QStringList packet);
    void ct(QStringList packet);
    void mapout(QStringList packet);
    void rl(QStringList packet);
    void tp(QStringList packet);
    void ta_m(QStringList packet);
    void sr(QStringList packet);
    void cancel(QStringList packet);
    void st(QStringList packet);
    void guri(QStringList packet);
    void su(QStringList packet);
};


#endif // NETWORKMANAGER_H
