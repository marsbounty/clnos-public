#ifndef NOSSTRINGS_H
#define NOSSTRINGS_H

#include <QObject>

class QCompleter;

class NosStrings : public QObject
{
    Q_OBJECT
public:
    explicit NosStrings(QObject *parent = nullptr);
    static void initialize(QString lang);
    static QString getItemName(int id);
    static int getItemId(QString name);
    static QString getMapName(int id);
    static QStringList& getItemNames();
    static QString getPath();
    static QStringList getLanguages();

private:
    static QMap<int,QString> items;
    static QMap<int,QString> maps;
    static QStringList itemNames;
    static QMap<int,int> buffs;

    static void loadFromFile(QString language, QString type);
};

#endif // NOSSTRINGS_H
