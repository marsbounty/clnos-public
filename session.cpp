#include "session.h"
#include "ui_session.h"
#include <QDesktopWidget>
#include <QSettings>

session::session(bool load,QStringList currProfiles, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::session),
    profiles(currProfiles)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    if(load){
        setWindowTitle("Load Session");
        ui->stackedWidget->setCurrentIndex(1);
        QSettings settings(qApp->applicationDirPath()+"/configs/sessions.ini",QSettings::IniFormat);
        QStringList keys = settings.allKeys();
        ui->loadname->addItems(keys);
    }else{
        setWindowTitle("Save Session");
        ui->stackedWidget->setCurrentIndex(0);
    }
    this->setModal(true);
}

session::~session()
{
    delete ui;
}

void session::on_save_clicked()
{
    QSettings settings(qApp->applicationDirPath()+"/configs/sessions.ini",QSettings::IniFormat);
    settings.setValue(ui->savename->text(),profiles);
    this->deleteLater();
}

void session::on_load_clicked()
{
    QSettings settings(qApp->applicationDirPath()+"/configs/sessions.ini",QSettings::IniFormat);
    QStringList keys = settings.allKeys();
    QString sessionN = ui->loadname->currentText();
    if(keys.contains(sessionN)){
        emit load(settings.value(sessionN).toStringList());
        this->deleteLater();
    }
}
