#include "chatui.h"
#include "ui_chatui.h"
#include "chatmng.h"
#include <QCompleter>

chatUI::chatUI(chatmng *mng,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::chatUI),
    mng(mng)
{
    ui->setupUi(this);
    show();
    connect(mng,SIGNAL(showChat(QString,int)),this,SLOT(addChat(QString,int)));
    setWindowTitle("Chat");
    ui->whisperReciver->hide();
    QCompleter *completer = new QCompleter(mng->getWhisperNames(),this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    ui->whisperReciver->setCompleter(completer);
    init();
}

chatUI::~chatUI()
{
    delete ui;
}

void chatUI::init()
{
    QString whisperColor = "<font color=\"Coral\">";
    foreach (QString msg, mng->getHistory()) {
        ui->chatBrowser->append(whisperColor+msg);
    }
}

void chatUI::addChat(QString msg, int type)
{
    QString msgColor = "<font color=\"DeepPink\">";
    QString whisperColor = "<font color=\"Coral\">";
    QString globalColor = "<font color=\"Lime\">";
    QString famColor = "<font color=\"DarkBlue \">";




    switch (type) {
    case 0:
        msg.prepend(msgColor);
        break;
    case 3:
        msg.prepend(whisperColor);
        break;
    case 4:
        msg.prepend(famColor);
        break;
    default:
        msg.prepend(globalColor);
        break;
    }
    ui->chatBrowser->append(msg);
}

void chatUI::on_chatType_currentIndexChanged(int index)
{
    if(index == 1){
        ui->whisperReciver->show();
    }else{
        ui->whisperReciver->hide();
    }
}

void chatUI::on_send_clicked()
{
    QString msg = ui->message->text();
    QString reciver = ui->whisperReciver->text();
    int type = ui->chatType->currentIndex();
    if(!msg.isEmpty()){
        switch (type) {
        case 0:
            mng->sendNormal(msg);
            break;
        case 1:
            mng->sendWhisper(reciver,msg);
            break;
        case 2:

            break;
        default:
            break;
        }
    }
}
