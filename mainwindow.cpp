#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "logger.h"
#include "playercontroller.h"
#include "player.h"
#include "playerui.h"
#include <QDebug>
#include <QLayout>
#include "packetlogger.h"
#include "akt4status.h"
#include "settings.h"
#include <QMessageBox>
#include "nosstrings.h"
#include <QCloseEvent>
#include <QLabel>
#include "gsettingsui.h"
#include <QDesktopWidget>
#include <QScreen>
#include <gflogin.h>
#include "battlemanager.h"
#include "complimentui.h"
#include "botcontroller.h"
#include "nosobjectmanager.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_QuitOnClose);
    int max = Settings::getGlobalSettings()->getSetting("max_botcount").toInt();
    botCount = new QLabel("Bots: 0/"+QString::number(max),this);
    totalGold = new QLabel("Total Gold: 0",this);
    totalEarned = new QLabel("Total Earned: 0",this);
    statusBar()->addWidget(botCount);
    statusBar()->addWidget(totalGold);
    statusBar()->addWidget(totalEarned);
    pCon = new PlayerController(this);
    PlayerController::setPC(pCon);
    connect(pCon,SIGNAL(addTab(QString,PlayerUI*)),this,SLOT(addTab(QString,PlayerUI*)));
    connect(pCon,SIGNAL(botCount(int,int)),this,SLOT(updateBotCount(int,int)));
    connect(pCon,SIGNAL(moneyStats(int,int)),this,SLOT(updateGold(int,int)));
    dashLayout = new FlowLayout();
    ui->scrollArea->widget()->setLayout(dashLayout);
    ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setWindowTitle("ClNos "+qApp->applicationVersion());
    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());
    setWindowIcon(QIcon(QPixmap(":/icons/ressources/Gold.png")));


    new nosObjectManager(this);
    BattleManager::initSkills();
    NosStrings::initialize(Settings::getGlobalSettings()->getSetting("language").toString());
    //BattleManager::printSkills();


    //QRect desktopSize = qApp->desktop()->availableGeometry();
    QRect desktopSize = QGuiApplication::primaryScreen()->availableGeometry();
    float hScale = qMax(1.0f,desktopSize.height() / float(1040));
    float wScale = qMax(1.0f,desktopSize.width() / float(1920));
    //qDebug()<<desktopSize.height()<<":"<<hScale<<" "<<desktopSize.width()<<":"<<wScale;
    setMinimumSize(width(),height());
    resize(width()*wScale,height()*hScale);

    ui->actionAuto_Buff_Master->setChecked(Settings::getGlobalSettings()->getSetting("autoBuffMaster").toBool());

    //gfloginTest* test = new gfloginTest(this);
    //test->show();

    //pCon->createDefault();

    if(Settings::getPsa().isEmpty()){
        ui->psa->hide();
        ui->closepsa->hide();
    }else{
        ui->psa->setText(Settings::getPsa());
    }

    if(Settings::getGlobalSettings()->getSetting("loadLastSession").toBool()){
        pCon->loadLastSession();
    }
}


MainWindow::~MainWindow()
{
    //pCon->backupCurrentSession();

    Settings::getGlobalSettings()->saveSettings("clnos");
    qApp->closeAllWindows();
    //delete(pCon);
    Logger::getGlobalLogger()->addLog("main","Closing Application");
    delete Logger::getGlobalLogger();
    delete ui;

}

void MainWindow::updateBotCount(int curr, int max)
{
    botCount->setText("Bots: "+QString::number(curr)+"/"+QString::number(max));
}

void MainWindow::updateGold(int total, int earned)
{
    totalGold->setText("Total Gold: "+QLocale().toString(total));
    totalEarned->setText("Total Earned: "+QLocale().toString(earned));
}

void MainWindow::addTab(QString name, PlayerUI *plyui)
{
    Q_UNUSED(name);
    dashLayout->addWidget(plyui);
}

void MainWindow::on_actionNew_triggered()
{
    pCon->newBot(false);
}

void MainWindow::on_actionLoad_from_Profile_triggered()
{
    pCon->newBot(true);
}

void MainWindow::on_actionPacket_Logger_triggered()
{
    if(!plog){
        plog = new PacketLogger(pCon);
    }else{
        plog->activateWindow();
    }
}

void MainWindow::closeEvent(QCloseEvent *ev)
{
    ev->ignore();
    if (QMessageBox::Yes == QMessageBox::question(this, "Close Confirmation", "Exit?", QMessageBox::Yes | QMessageBox::No))
    {
        pCon->deleteLater();
        qApp->closeAllWindows();
        //qApp->quit();
        ev->accept();
    }
}

void MainWindow::on_actionAkt_4_Status_triggered()
{
    new akt4status();
}

void MainWindow::on_actionSave_current_Session_triggered()
{
    pCon->saveCurrentSession();
}

void MainWindow::on_actionLoad_last_Session_triggered()
{
    pCon->loadLastSession();
}

void MainWindow::on_actionSettings_triggered()
{
    new gSettingsUI();
}

void MainWindow::on_actionQuit_triggered()
{
    this->close();
}

void MainWindow::on_closepsa_clicked()
{
    ui->psa->hide();
    ui->closepsa->hide();
}

void MainWindow::on_actionLoad_Session_triggered()
{
    pCon->loadSession();
}

void MainWindow::on_actionArena_of_Talents_triggered()
{
    pCon->showAotUI();
}

void MainWindow::on_actionAuto_Buff_Master_triggered()
{
    Settings::getGlobalSettings()->changeSetting("autoBuffMaster",!Settings::getGlobalSettings()->getSetting("autoBuffMaster").toBool());
    ui->actionAuto_Buff_Master->setChecked(Settings::getGlobalSettings()->getSetting("autoBuffMaster").toBool());
}

void MainWindow::on_actionUse_all_available_Buffs_triggered()
{
    pCon->useAllBuffs();
}

void MainWindow::on_actionCompliment_Player_triggered()
{
    ComplimentUI *compui = new ComplimentUI(pCon,this);
    compui->show();
}

void MainWindow::on_actionControls_triggered()
{
    if(!pConUI){
        pConUI = new BotController(pCon,this);
    }
    else{
        pConUI->activateWindow();
    }
}
