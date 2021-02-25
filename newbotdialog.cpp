#include "newbotdialog.h"
#include "ui_newbotdialog.h"
#include <QDesktopWidget>


NewBotDialog::NewBotDialog(QStringList profiles,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewBotDialog)
{
    ui->setupUi(this);
    ui->load_profile_name->addItems(profiles);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle("New Bot");
    this->setModal(true);
}

NewBotDialog::~NewBotDialog()
{
    delete ui;
}

void NewBotDialog::on_load_profile_clicked()
{
    emit clicked(ui->load_profile_name->currentText(),ui->auto_login->isChecked(),false);
    this->deleteLater();
}

void NewBotDialog::on_new_profile_2_clicked()
{
    emit clicked(ui->new_profile->text(),false,false);
    this->deleteLater();
}
