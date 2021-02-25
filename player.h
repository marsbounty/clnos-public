#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>


class PlayerUI;
class Settings;
class Logger;
class NetworkManager;
class Map;
class SettingsUI;
class Misc;
class BazarManager;
class Inventory;
class chatmng;
class BattleManager;

class Player : public QObject
{
    Q_OBJECT
public:
    explicit Player(QString name,bool autologin,QObject *parent = nullptr);
    virtual ~Player();
    PlayerUI *getUI();
    void login(QString username, QString password, QString ip, qint16 port, bool old_version=false, int server=0);
    Settings *getSettings() const;
    Logger *getLog() const;
    NetworkManager *getNet() const;
    void logout();
    void resetGoldTracker();
    void startGFlogin();
    void gfLoginError(QString err);

    void init();
    void setName(const QString &value);
    QString getName() const;
    int getLvl() const;
    void setLvl(int value, int value2);
    int getJlvl() const;
    int getGold() const;
    void setGold(int value);
    int getGold_start() const;
    int getId() const;
    void setId(int value);
    Map *getMap() const;
    void openSettings();
    void closeSettings();
    Misc *getMisc() const;
    int getSpeed() const;
    void setSpeed(int value);
    BazarManager *getBazar() const;
    Inventory *getInv() const;
    bool getConnected() const;
    void setConnected(bool value);

    bool getAutologin() const;
    void setAutologin(bool value);

    QString getServerName() const;
    void setServerName(const QString &value);

    bool getHasMedal() const;
    void setHasMedal(bool value);

    int getKlasse() const;
    void setKlasse(int value);

    void wearSP();

    chatmng *getChat() const;

    bool getIsOldVersion() const;
    void setIsOldVersion(bool value);
    void setSkills(QStringList packet);
    bool hasSkill(int id);

    bool getIsDead() const;
    void setIsDead(bool value);

    int getMasterId();

    BattleManager *getBmanager() const;

    QList<int> getAvailSkills() const;

signals:
    void connectionStatus(int id);
    void remove(Player* ply);
    void moneyUpdate();

public slots:

private:
    Logger *profileLog;
    Settings *profileSettings;
    QString profileName;
    PlayerUI *botUi;
    NetworkManager *net;
    Map *map;
    SettingsUI *sett;
    BazarManager *bazar;  
    Inventory *inv;
    Misc *misc;
    chatmng *chat;
    BattleManager *bmanager;



    QString serverName;
    QString name;
    int lvl;
    int jlvl;
    int gold;
    int gold_start;
    int id;
    int speed;
    bool connected;
    bool autologin;
    bool hasMedal;
    int klasse;
    bool isOldVersion;
    bool isDead;
    QList<int> availSkills;
};

#endif // PLAYER_H
