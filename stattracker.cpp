#include "stattracker.h"
#include "settings.h"
#include "logger.h"

stattracker* stattracker::main = nullptr;

stattracker *stattracker::getTracker()
{
    if(main==nullptr){
        main = new stattracker();
    }
    return main;
}

void stattracker::icJoined()
{
    int oamm = Settings::getGlobalSettings()->getSetting("stats/icJoined").toInt()+1;
    Settings::getGlobalSettings()->changeSetting("stats/icJoined",oamm);
}

void stattracker::icWon()
{
    int oamm = Settings::getGlobalSettings()->getSetting("stats/icWon").toInt()+1;
    Settings::getGlobalSettings()->changeSetting("stats/icWon",oamm);
}

int stattracker::getIcJoined()
{
    return Settings::getGlobalSettings()->getSetting("stats/icJoined").toInt();
}

int stattracker::getIcWon()
{
    return Settings::getGlobalSettings()->getSetting("stats/icWon").toInt();
}

stattracker::stattracker(QObject *parent) : QObject(parent)
{
    Logger::getGlobalLogger()->addLog("STATS","Stattracker started!");
}
