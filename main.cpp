#include "mainwindow.h"
#include <QApplication>
#include "logger.h"
#include "settings.h"
#include "newbotdialog.h"
#include "versionloader.h"
#include "runguard.h"
#include <QMessageBox>
#include "notificationsystem.h"
#include "noscrypto.h"
#include "gfnetworklogin.h"
#include <iostream>


Q_DECLARE_METATYPE(QList<QList<int>>)
Q_DECLARE_METATYPE(QList<int>)

int main(int argc, char *argv[])
{
    std::cout<<QT_VERSION<<std::endl;
    QApplication a(argc, argv);
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    qApp->setApplicationVersion("0.19.00");
    #ifdef QT_DEBUG
      RunGuard guard( "marsbounty_clnos_debug" );
    #else
      RunGuard guard( "marsbounty_clnos" );
    #endif
    bool noLogs = false;

    if(argc >=1){
      for(int i = 0; i<argc;i++) {
          if(QString(argv[i]).contains("-NoLogs")){
            noLogs = true;
          }
          std::cout<<argv[i]<<std::endl;
      }
    }



    qRegisterMetaTypeStreamOperators<QList<QList<int>>>("QList<QList<int>>");
    qRegisterMetaTypeStreamOperators<QList<int>>("QList<int>");

    qInstallMessageHandler(Logger::DebugMessageHandler);
    Logger::setPath(qApp->applicationDirPath()+"/logs/");
    Logger::disableLogs(noLogs);
    Logger::getGlobalLogger()->addLog("main","Starting Application");


    Settings::setPath(qApp->applicationDirPath()+"/configs/");
    Settings::getGlobalSettings();

    versionLoader* VL = new versionLoader();
    VL->exec();

    notificationSystem::get();

    if ( !guard.tryToRun() ){
        QMessageBox::warning(nullptr, "Warning!", "An instance of this application is already running!");
        return 0;
    }

    NosCrypto::readVendettaFix();

    MainWindow w;
    w.show();

    Settings::getGlobalSettings()->saveSettings("clnos");

    return a.exec();
}
