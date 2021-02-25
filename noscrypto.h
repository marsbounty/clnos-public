#ifndef NOSCRYPTO_H
#define NOSCRYPTO_H
#include <string>
#include <vector>
#include <QObject>
#include <QByteArray>

class QTextEncoder;
class QTextDecoder;

class NosCrypto : public QObject
{
    Q_OBJECT
public:
    explicit NosCrypto(QObject *parent = nullptr);
    QByteArray encryptGamePacket(const QString& buf, int session, bool is_session_packet = 0);
    std::vector<unsigned char> encryptLoginPacket(const QString& buf);
    QString decryptLoginPacket(const QByteArray& ba, std::size_t len);
    QStringList decryptGamePacket(const QByteArray& inp, std::size_t len);
    QString createLoginHash(const QString& user) const;
    QString createLoginVersion(int langid =0) const;
    int randomNumber(int min, int max) const;
    QString Sha512(const QString plainText) const;
    QByteArray createLoginPacket(const QString& username, const QString& password);
    QByteArray createGFLoginPacket(const QString& uid, QString username, int langid =0);
    void randomize() const;

    static void setDxhash(const QString &value);
    static void setGlhash(const QString &value);
    static void setVersion(const QString &value);

    static void setVversion(const QString &value);
    static void setVhash(const QString &value);

    static void readVendettaFix();

    bool getOld_version() const;
    void setOld_version(bool value);

    void resetVCounter();

signals:

public slots:

private:
    static QString dxhash;
    static QString glhash;
    static QString version;

    QTextEncoder* encoder;
    QTextDecoder* decoder;

    static QByteArray encryptTable;
    static QByteArray decryptTable;

    int decCounter;
    int encCounter;

    static QString vversion;
    static QString vhash;

    void completeGamePacketEncrypt(std::vector<unsigned char>& buf, int session, bool is_session_packet = 0) const;
    QByteArray bigPacket;
    std::string last_current_packet;
    std::vector<unsigned char> last_buf;
    QString pwEncrypt(QString pw) const;
    bool old_version;
};
#endif // NOSCRYPTO_H
