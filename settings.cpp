#include "settings.h"
#include "logger.h"
#include <QDebug>
#include <QMap>
#include <QSettings>
#include <QDir>
#include <QPoint>
#include <QNetworkProxy>
#include "itemlist.h"
#include "bitem.h"
#include "itemlist.h"
#include <QRandomGenerator>

Settings *Settings::global_settings = nullptr;
QMap<QString,QVariant> Settings::current_global_settings;
QString Settings::path = "";
QString Settings::psa = "";

Settings::Settings(QString name, bool isStandard, QObject *parent) : QObject(parent),is_global_settings(isStandard),name(name)
{
    initializeSettings();
}

Settings *Settings::getGlobalSettings()
{
    if(global_settings==nullptr){
        global_settings = new Settings("clnos",true);
    }
    return global_settings;
}

void Settings::setPath(QString path)
{
    Settings::path = path;
}

void Settings::updateProxy()
{
    if(is_global_settings){
        if(Settings::getGlobalSettings()->getSetting("proxy/enabled").toBool()){

            QString ip = Settings::getGlobalSettings()->getSetting("proxy/ip").toString();
            qint16 port = Settings::getGlobalSettings()->getSetting("proxy/port").toInt();
            QString username = Settings::getGlobalSettings()->getSetting("proxy/username").toString();
            QString password = Settings::getGlobalSettings()->getSetting("proxy/password").toString();
            Logger::getGlobalLogger()->addLog("PROXY","Connecting to Proxy on "+ip);
            QNetworkProxy appProxy = QNetworkProxy(QNetworkProxy::Socks5Proxy,ip,port,username,password);
            QNetworkProxy::setApplicationProxy(appProxy);
            if(appProxy.isTransparentProxy()){
                Logger::getGlobalLogger()->addLog("PROXY","Connected to "+QNetworkProxy::applicationProxy().hostName());
            }

        }
    }
}

QVariant Settings::getSetting(QString key)
{
    if(is_global_settings){
        if(current_global_settings.contains(key)){
            return current_global_settings.value(key);
        }else{
            qWarning()<<"Setting"<<key<<"not found in global Settings";
        }
    }else{
        if(current_profile_settings.contains(key)){
            return current_profile_settings.value(key);
        }else{
            qWarning()<<"Setting"<<key<<"not found in"<<name<<".ini";
        }
    }

    return false;
}

void Settings::saveSettings(QString name)
{
    if(name != ""){
        QSettings settings(path+name+".ini",QSettings::IniFormat);
        this->name = name;
        if(is_global_settings){
            QMapIterator<QString, QVariant> i(current_global_settings);
            while (i.hasNext()) {
                 i.next();
                 settings.setValue(i.key(),i.value());
            }
            Logger::getGlobalLogger()->addLog("Settings","Saving Global Settings to "+name+".ini");
        }else{
            QMapIterator<QString, QVariant> i(current_profile_settings);
            while (i.hasNext()) {
                 i.next();
                 settings.setValue(i.key(),i.value());
            }
            Logger::getGlobalLogger()->addLog("Settings","Saving Settings to "+name+".ini");
        }
    }
}

void Settings::loadSettings(QString ext)
{
    if(ext!= ""){
        //TODO Fill this in!
    }
    if(QFile(path+name+".ini").exists()){
        QSettings settings(path+name+".ini",QSettings::IniFormat);
        QStringList keys = settings.allKeys();
        if(is_global_settings){
            Logger::getGlobalLogger()->addLog("Settings","Loading Global Settings");
            foreach (QString key, keys) {
                if(current_global_settings.contains(key)){
                    current_global_settings[key]=settings.value(key);
                }else{
                    Logger::getGlobalLogger()->addLog("Settings","Unknown Setting "+key+". Ignoring");
                }
            }
            changeSetting("experimentalLogin",true);
        }else{
            Logger::getGlobalLogger()->addLog("Settings","Loading Settings "+name+".ini");
            foreach (QString key, keys) {
                if(current_profile_settings.contains(key)){
                    current_profile_settings[key]=settings.value(key);
                    if(key=="misc/joinIC"){
                        current_profile_settings[key]=false;
                    }
                }else{
                    Logger::getGlobalLogger()->addLog("Settings","Unknown Setting "+key+". Ignoring");
                }
            }
        }
    }else{
        Logger::getGlobalLogger()->addLog("Settings","Couldn't find "+name+".ini"+"! Using Default Settings.");
        //saveSettings();
    }
}

void Settings::changeSetting(QString setting, QVariant value)
{
    if(is_global_settings){
        if(current_global_settings.contains(setting)){
            Logger::getGlobalLogger()->addLog("Settings","Changed "+setting);
            current_global_settings[setting] = value;
        }else{
            Logger::getGlobalLogger()->addLog("Settings","Setting "+setting+" not Found");
        }
    }else{
        if(current_profile_settings.contains(setting)){
            //Logger::getGlobalLogger()->addLog("Settings","Changed "+setting);
            current_profile_settings[setting] = value;
            Logger::getGlobalLogger()->addLog("Settings","Changed Setting "+setting+". For " + name);
        }else{
            Logger::getGlobalLogger()->addLog("Settings","Unknown Setting "+setting+". Ignoring");
        }
    }
}

QString Settings::getName()
{
    return name;
}

QStringList Settings::getAllConfigNames()
{
    QStringList configs = QDir(path).entryList();
    configs.removeOne(".");
    configs.removeOne("..");
    configs.removeOne("sessions.ini");
    configs.removeOne(getGlobalSettings()->getName()+".ini");
    configs.replaceInStrings(".ini","");
    return configs;
}

void Settings::initializeSettings()
{
    if(is_global_settings){
        Logger::getGlobalLogger()->addLog("Settings","Initializing Global Settings");
        current_global_settings.insert("logging/enabled",true);
        current_global_settings.insert("logging/packets",false);
        current_global_settings.insert("language","Deutsch");
        current_global_settings.insert("profiles/default","Profile1");
        current_global_settings.insert("max_botcount","0");
        current_global_settings.insert("loginvars/dat","c74d922d77a771261ea3b57027631922");
        current_global_settings.insert("loginvars/xdat","a89aad214c1bc51ee92d5b87fd77b380");
        current_global_settings.insert("loginvars/version","0.9.3.3077");
        current_global_settings.insert("loginvars/vendettahash","0f587002f3568bbed4430ef9d49ebb9a");
        current_global_settings.insert("loginvars/vendettaversion","0.9.3.3");
        current_global_settings.insert("login/autoreconnect",true);
        current_global_settings.insert("profiles/lastSession",0);
        current_global_settings.insert("proxy/ip","");
        current_global_settings.insert("proxy/port","");
        current_global_settings.insert("proxy/username","");
        current_global_settings.insert("proxy/password","");
        current_global_settings.insert("proxy/enabled",false);
        current_global_settings.insert("notifications/enabled",false);
        current_global_settings.insert("notifications/akt4angel",false);
        current_global_settings.insert("notifications/akt4demon",false);
        current_global_settings.insert("notifications/whisper",true);
        current_global_settings.insert("notifications/mapchange",true);
        current_global_settings.insert("ic/staticPos",0);
        current_global_settings.insert("ic/Pos",QPoint(35,35));
        current_global_settings.insert("ic/autoDelay",10);
        current_global_settings.insert("stats/totalEarned",0);
        current_global_settings.insert("stats/icJoined",0);
        current_global_settings.insert("stats/icWon",0);
        current_global_settings.insert("profiles/loadDelay",1000);
        current_global_settings.insert("hwid","");
        current_global_settings.insert("ic/move",true);
        current_global_settings.insert("update/autoUpdateNostale",true);
        current_global_settings.insert("update/autoUpdateVendetta",true);
        current_global_settings.insert("autoBuffMaster",false);
        current_global_settings.insert("customServer/ip","192.178.0.1");
        current_global_settings.insert("customServer/port",4000);
        current_global_settings.insert("autoBuffDelay",true);
        current_global_settings.insert("gfLoginRegion","0");
        current_global_settings.insert("experimentalLogin",false);
        current_global_settings.insert("logging/reallyNoLogs",false);
        current_global_settings.insert("loadLastSession",false);
        current_global_settings.insert("logLoginStuff",false);
        current_global_settings.insert("delay/login",1000);
        current_global_settings.insert("delay/loginvar",1000);
        current_global_settings.insert("delay/buff",2000);
        current_global_settings.insert("delay/buffvar",500);
        current_global_settings.insert("delay/buy",4000);
        current_global_settings.insert("delay/buyvar",4000);
        current_global_settings.insert("delay/sell",4000);
        current_global_settings.insert("delay/sellvar",4000);
        current_global_settings.insert("delay/checksold",300000);
        current_global_settings.insert("delay/checksoldvar",100000);
        current_global_settings.insert("delay/ic",500);
        current_global_settings.insert("delay/icvar",500);
        current_global_settings.insert("delay/follow",500);
        current_global_settings.insert("delay/followvar",500);
        current_global_settings.insert("delay/mimic",200);
        current_global_settings.insert("delay/mimicvar",500);
        current_global_settings.insert("delay/other",1000);
        current_global_settings.insert("delay/othervar",2000);
        current_global_settings.insert("packets/minilandInv","$Einladung");
    }else{
        current_profile_settings.insert("login/username","");
        current_profile_settings.insert("login/password","");
        current_profile_settings.insert("login/server",0);
        current_profile_settings.insert("login/channel",0);
        current_profile_settings.insert("login/loginserver",0);
        current_profile_settings.insert("login/char",0);
        current_profile_settings.insert("misc/joinIC",false);
        current_profile_settings.insert("misc/joinGroups",0);
        current_profile_settings.insert("misc/acceptTrades",0);
        current_profile_settings.insert("misc/master","");
        current_profile_settings.insert("misc/masterid",0);
        current_profile_settings.insert("misc/autoCollect",false);
        current_profile_settings.insert("misc/followMaster",false);
        current_profile_settings.insert("misc/wearSP",false);
        current_profile_settings.insert("basar/buyDelay",5);
        current_profile_settings.insert("basar/sellDelay",12);
        current_profile_settings.insert("misc/goldbuffer",10000);
        current_profile_settings.insert("basar/autoCollectSold",false);
        current_profile_settings.insert("basar/buylist",QVariant());
        current_profile_settings.insert("basar/selllist",QVariant());
        current_profile_settings.insert("basar/collectIfCheaper",false);
        current_profile_settings.insert("basar/collectIfCheaperMargin",1000);
        current_profile_settings.insert("basar/collectAmmountMargin",10);
        current_profile_settings.insert("basar/collectDelay",300000);
        current_profile_settings.insert("misc/joinSheeps",false);
        current_profile_settings.insert("loot/items",QVariant());
        current_profile_settings.insert("loot/gold",-1);
        current_profile_settings.insert("misc/createSheepRaids",false);
        current_profile_settings.insert("misc/healItems",QVariant());
        current_profile_settings.insert("misc/autoHeal",false);
        current_profile_settings.insert("misc/autoHealPercent",10);
        current_profile_settings.insert("misc/acceptRaidRequest",false);
        current_profile_settings.insert("misc/mimicSkills",false);
        current_profile_settings.insert("misc/mimicSkillsExperimental",false);
    }
    loadSettings();
}

QString Settings::getPsa()
{
    return psa;
}

void Settings::setPsa(const QString &value)
{
    psa = value;
}

int Settings::getDelay(QString type)
{
    QString setting = "delay/"+type;
    if(current_global_settings.contains(setting)){
        int delay = current_global_settings.value(setting).toInt();
        int change = current_global_settings.value(setting+"var").toInt();
        return delay + QRandomGenerator::global()->bounded(change);
    }else{
        Logger::getGlobalLogger()->addLog("SETTINGS","Delay Setting for type "+type+" not found");
        return 1000;
    }
}
