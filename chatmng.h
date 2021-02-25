#ifndef CHATMNG_H
#define CHATMNG_H

#include <QObject>

class QStringListModel;
class QAbstractItemModel;
class Player;

class chatmng : public QObject
{
    Q_OBJECT
public:
    explicit chatmng(Player *ply, QObject *parent = nullptr);
    void addChat(QStringList msg,int type=0);
    void openchat();
    void sendNormal(QString msg);
    void sendWhisper(QString player,QString message);
    QAbstractItemModel *getWhisperNames();
    QStringList getHistory();

signals:
    void showChat(QString msg,int type);

private:
    Player *ply;
    QStringListModel *whisperNamesModel;
    QStringList whisperNames;
    QStringList chatHistory;

    void addWhisperName(QString name);

public slots:
};

#endif // CHATMNG_H
