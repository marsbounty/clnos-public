#include "logger.h"
#include <QDebug>
#include <QApplication>
#include <QDateTime>
#include <QDir>
#include "settings.h"
#include <iostream>

Logger *Logger::global_logger = nullptr;
QString Logger::path = "";
bool Logger::logsDisabled = false;

Logger::Logger(QString file_name,bool standard, QObject *parent) :QObject(parent),file_name(file_name),file(path+file_name+".log"),is_gloabl_logger(standard)
{
    paused = true;
    initializeLogFile();
    if(!is_gloabl_logger){
        setParent(getGlobalLogger());
    }
}

Logger::~Logger()
{
    close();
}

void Logger::disableLogs(bool arg){
    logsDisabled = arg;
}

Logger* Logger::getGlobalLogger()
{
    if(global_logger==nullptr){
        qSetMessagePattern("[%{if-debug}D%{endif}%{if-info}I%{endif}%{if-warning}W%{endif}%{if-critical}C%{endif}%{if-fatal}F%{endif}] %{file}:%{line} - %{message}");
        global_logger = new Logger("clnos",true);
        global_logger->addLog("LOGGER","Creating Global logfile (this)");
    }
    return global_logger;
}

void Logger::setPath(QString path)
{
    Logger::path = path;
}

void Logger::DebugMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString str = qFormatLogMessage(type,context,msg);
    getGlobalLogger()->addLog("Debugger",str);
}

void Logger::initializeLogFile()
{
    if(logsDisabled){
        return;
    }

    QString logDir = qApp->applicationDirPath()+"/logs";
    if(!QDir(logDir).exists()){
        QDir().mkdir(logDir);
    }

    if(file.exists()){
        file.remove();
    }
    if(!is_gloabl_logger){
        if(!Settings::getGlobalSettings()->getSetting("logging/enabled").toBool()){
            paused = true;
            return;
        }
    }
    if(file.open(QIODevice::WriteOnly | QIODevice::Append)){
        paused = false;
        log_stream = new QTextStream(&file);
        addLog("Logger","Started Logfile "+file_name+".log");
        if(!is_gloabl_logger){
            getGlobalLogger()->addLog("Logger","Started Logfile "+file_name+".log");
        }

    }else{
        paused = true;
        qDebug()<<file.errorString();
        if(!is_gloabl_logger){
            getGlobalLogger()->addLog("Logger","Failed to open "+file_name+".log");
        }
    }
}

void Logger::addLog(QString prefix, QString msg)
{
    if(!(paused||logsDisabled)){
        QDateTime date_time;
        QString prefix_date_time = date_time.currentDateTime().toString("dd.MM.yyyy hh:mm:ss - ");
        prefix.append(" - ");
        *log_stream<<prefix_date_time<<prefix<<msg<< endl;
        std::cout<<prefix_date_time.toStdString()<<prefix.toStdString()<<msg.toStdString()<< std::endl;
    }else{
        //qDebug()<<"Trying to add log while paused";
    }
}

void Logger::close()
{
    if(logsDisabled){
        return;
    }

    if(is_gloabl_logger){
        qDeleteAll(this->children());
    }

    addLog("Logger","Closing Logfile "+file_name+".log");
    if(this!=global_logger){
        getGlobalLogger()->addLog("Logger","Closing Logfile "+file_name+".log");
    }
    file.close();
    if(!paused){
        delete log_stream;
    }
    log_stream = nullptr;
}
