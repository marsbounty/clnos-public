#include "gflogin.h"
#include "memory.h"
#include <QTimer>
#include <QDebug>
#include "networkmanager.h"
#include <QSettings>
#include <QProcess>
#include "logger.h"

QList<gflogin*> gflogin::line = {};

gflogin::gflogin(Player* ply,QObject *parent) : QObject(parent),ply(ply)
{
    firstrun = true;
    QSettings m("HKEY_CURRENT_USER\\Software\\Gameforge4d\\GameforgeLive\\MainApp",QSettings::NativeFormat);
    path = m.value("PreferredInstallDir").toString();
    path += "/"+m.value("Locale").toString();
    path = path.replace("/","\\");
    //qDebug()<<path;


    getId();
}

gflogin::~gflogin()
{
    line.removeAll(this);
}

void gflogin::getId()
{
    if(!line.contains(this)){
        line.append(this);
    }
    if(line.first()!=this){
        QTimer::singleShot(2000,this,SLOT(getId()));
        ply->gfLoginError("Waiting for other client first.");
        return;
    }
    if(firstrun){
        QString fullpath = path+"\\NosTale\\Nostale.exe";
        //qDebug()<<fullpath;
        ply->getLog()->addLog("GFLOGIN","Trying to start GF Launcher");
        if(QProcess::startDetached(fullpath)){
            ply->gfLoginError("Login to your Gameforge Account. You have to run this Bot as Admin for it to work!");
            ply->getLog()->addLog("GFLOGIN","GF Launcher didn't start.");
        }else{
            ply->gfLoginError("Waiting for Nostale. Start Nostale the normal way to use the Account in this Bot.You have to run this Bot as Admin for it to work!");
            ply->getLog()->addLog("GFLOGIN","GF Launcher did start.");
        }
    }
    firstrun = false;
    Memory memory;
    int id = memory.GetProcessId(L"NostaleClientX.exe");
    if(id==0){
        QTimer::singleShot(500,this,SLOT(getId()));
        return;
    }
    ply->gfLoginError("Nostale Client found!");
    ply->getLog()->addLog("GFLOGIN","Nostale CLient found id: "+QString::number(id));
    QTimer::singleShot(1000,this,SLOT(getUID()));
}

void gflogin::getUID()
{
    ply->getLog()->addLog("GFLOGIN","Trying to read Memory");
    Memory memory;
    int id = memory.GetProcessId(L"NostaleClientX.exe");
    if(id==0){ply->gfLoginError("Couldnt get Nostale PID.");deleteLater() ;return;}
    HANDLE nos = OpenProcess(PROCESS_ALL_ACCESS, false,id);
    if (!nos) {ply->gfLoginError("Could not Acess Memory. Are you Root?" );ply->getLog()->addLog("GFLOGIN","Could not read Memory"); deleteLater();return;}


    int base = memory.GetModuleBase(id,L"gf_wrapper.dll");
    QString uid = QString::fromStdString(memory.ReadText(nos,base+0x1B3A8));
    ply->getLog()->addLog("GFLOGIN","Recived UID "+ uid);


//    int base2 = memory.GetModuleBase(id,L"NostaleClientX.exe");
//    //int offsets[] = {0x0};
//    //int port = memory.ReadPointerInt(nos,base2+0x2D35DC,offsets,1);
//    int port = memory.ReadInt(nos,base2+0x2D35DC);
//    ply->getLog()->addLog("GFLOGIN","Recived Port "+ QString::number(port));


//    int base3 = memory.GetModuleBase(id,L"psw_tnt.dll");
//    //QString username = memory.ReadPointerText(nos,base3+0x000018D8,offsets,1);
//    QString username = "";
//    ply->getLog()->addLog("GFLOGIN","Recived Username "+ username);


    ply->gfLoginError(uid);
    ply->getLog()->addLog("GFLOGIN","Trying to log in");
    if(uid.length()<10){
        ply->gfLoginError("Failed to get UID! Try again or wait for an update  UID: "+uid);
    }else{
        ply->getNet()->gfLogin(uid,-1,0);
    }
    TerminateProcess(nos,0);
    ply->getLog()->addLog("GFLOGIN","Closing Nostale Client");
    CloseHandle(nos);
    line.removeAll(this);
    deleteLater();
}
