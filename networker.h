#ifndef NETWORKER_H
#define NETWORKER_H

#include <QObject>
#include <QTcpSocket>
class QTimer;
class NosCrypto;
class NetworkManager;

class networker : public QObject
{
    Q_OBJECT
public:
    explicit networker(NetworkManager *manager,QObject *parent = nullptr);
    virtual ~networker();
    void connectTo(QString ip, qint16 port, bool loginServer = false);
    void sendString(QString packet, bool log = true);
    void sendPacket(QByteArray packet);
    void login(QString username, QString password, bool isSwitching, bool old_version=false ,int langid = 0);
    void setSessionID(int value);
    void close();


    bool getIsLoginServer() const;

signals:

private slots:
    void recievePacket();
    void recieveError(QAbstractSocket::SocketError err);
    void connected();
    void disconnected();
    void loginHelper();
    void timeout();

private:
    bool isLoginServer;
    QTcpSocket *socket;
    NosCrypto *crypt;
    NetworkManager *manager;

    int getSessionID();
    int getPacketID();


    int sessionID;
    int packetID;

    QStringList decrypt(QByteArray buf);
    QByteArray encrypt(QString inp);
    QTimer* timeoutTimer;



    void sendPacketID();
    bool logLogin;
};

#endif // NETWORKER_H
