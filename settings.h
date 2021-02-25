#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QVariant>

class itemlist;


class Settings : public QObject
{
    Q_OBJECT
public:
    explicit Settings(QString name, bool isStandard = false,QObject *parent = nullptr);
    static Settings *getGlobalSettings();
    static void setPath(QString path);
    void updateProxy();
    QVariant getSetting(QString key);
    void saveSettings(QString name);
    void loadSettings(QString ext = "");
    void changeSetting(QString setting, QVariant value);
    QString getName();
    static QStringList getAllConfigNames();
    static QString getPsa();
    static void setPsa(const QString &value);
    static int getDelay(QString type);
signals:

public slots:

private:
    static QMap<QString,QVariant> current_global_settings;
    static Settings *global_settings;
    static QString path;
    QMap<QString,QVariant> current_profile_settings;
    bool is_global_settings;
    QString name;
    void initializeSettings();
    static QString psa;

};

#endif // SETTINGS_H
