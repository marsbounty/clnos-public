#include "gsettingsui.h"
#include "ui_gsettingsui.h"
#include "settings.h"
#include "stattracker.h"
#include "playercontroller.h"
#include "nosstrings.h"

gSettingsUI::gSettingsUI(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::gSettingsUI)
{
    ui->setupUi(this);
    show();
    setWindowTitle("Settings");
    loadSettings();
    ui->tabWidget->setCurrentIndex(0);
}

gSettingsUI::~gSettingsUI()
{
    delete ui;
}

void gSettingsUI::on_close_clicked()
{
    deleteLater();
}

void gSettingsUI::on_save_clicked()
{
    saveSettings();
    Settings::getGlobalSettings()->saveSettings("clnos");
}

void gSettingsUI::loadSettings()
{
    ui->proxyIP->setEnabled(false);
    ui->proxyPort->setEnabled(false);
    ui->proxyUsername->setEnabled(false);
    ui->proxyPassword->setEnabled(false);
    ui->enablePacketLogging->setEnabled(false);
    ui->icXPos->setEnabled(false);
    ui->icYPos->setEnabled(false);

    ui->enableLogging->setChecked(Settings::getGlobalSettings()->getSetting("logging/enabled").toBool());
    ui->enablePacketLogging->setChecked(Settings::getGlobalSettings()->getSetting("logging/packets").toBool());
    ui->nostaleHash->setText(  Settings::getGlobalSettings()->getSetting("loginvars/dat").toString());
    ui->nostaleXhash->setText(Settings::getGlobalSettings()->getSetting("loginvars/xdat").toString());
    ui->nostaleVersion->setText(Settings::getGlobalSettings()->getSetting("loginvars/version").toString());
    ui->proxyIP->setText(Settings::getGlobalSettings()->getSetting("proxy/ip").toString());
    ui->proxyPort->setText(Settings::getGlobalSettings()->getSetting("proxy/port").toString());
    ui->proxyUsername->setText(Settings::getGlobalSettings()->getSetting("proxy/username").toString());
    ui->proxyPassword->setText(Settings::getGlobalSettings()->getSetting("proxy/password").toString());
    ui->useProxy->setChecked(Settings::getGlobalSettings()->getSetting("proxy/enabled").toBool());
    ui->notificationsEnable->setChecked(Settings::getGlobalSettings()->getSetting("notifications/enabled").toBool());
    ui->notificationAkt4Angel->setChecked(Settings::getGlobalSettings()->getSetting("notifications/akt4angel").toBool());
    ui->notificationAkt4Demon->setChecked(Settings::getGlobalSettings()->getSetting("notifications/akt4demon").toBool());
    ui->notificationWhisper->setChecked(Settings::getGlobalSettings()->getSetting("notifications/whisper").toBool());
    ui->notificationMapchange->setChecked(Settings::getGlobalSettings()->getSetting("notifications/mapchange").toBool());
    ui->icUseFixedPos->setChecked(Settings::getGlobalSettings()->getSetting("ic/staticPos").toInt()==1);
    QPoint icPos = Settings::getGlobalSettings()->getSetting("ic/Pos").toPoint();
    ui->icXPos->setValue(icPos.rx());
    ui->icYPos->setValue(icPos.ry());
    ui->autoPosDelay->setValue(Settings::getGlobalSettings()->getSetting("ic/autoDelay").toInt());
    ui->autoreconnect->setChecked(!Settings::getGlobalSettings()->getSetting("login/autoreconnect").toBool());

    ui->vendettaHash->setText(Settings::getGlobalSettings()->getSetting("loginvars/vendettahash").toString());
    ui->vendettaVersion->setText(Settings::getGlobalSettings()->getSetting("loginvars/vendettaversion").toString());

    ui->vendettaNoUpdate->setChecked(Settings::getGlobalSettings()->getSetting("update/autoUpdateVendetta").toBool());
    ui->nostaleNoUpdate->setChecked(Settings::getGlobalSettings()->getSetting("update/autoUpdateNostale").toBool());
    ui->useBuffsWithDelay->setChecked(Settings::getGlobalSettings()->getSetting("autoBuffDelay").toBool());

    int i = stattracker::getTracker()->getIcJoined();
    int y = stattracker::getTracker()->getIcWon();
    ui->icJoined->setText(QString::number(i));
    ui->icWon->setText(QString::number(y));
    if(y==0){
        ui->icWinp->setText("0 %");
    }else{
        ui->icWinp->setText(QString::number(y*100/i)+" %");
    }

    ui->goldTotal->setText(QLocale().toString(PlayerController::getPC()->getCurrentGold()));
    ui->goldSessionEarned->setText(QLocale().toString(PlayerController::getPC()->getEarnedGold()));
    ui->goldTotalEarned->setText(QLocale().toString(PlayerController::getPC()->getTotalEarnedGold()));

    ui->language->addItems(NosStrings::getLanguages());
    ui->language->setCurrentText(Settings::getGlobalSettings()->getSetting("language").toString().toLower());

    ui->characterLoadDelay->setValue(Settings::getGlobalSettings()->getSetting("profiles/loadDelay").toInt());
    ui->icmoving->setChecked(Settings::getGlobalSettings()->getSetting("ic/move").toBool());

    ui->customServerIP->setText(Settings::getGlobalSettings()->getSetting("customServer/ip").toString());
    ui->customServerPort->setValue(Settings::getGlobalSettings()->getSetting("customServer/port").toInt());

    //ui->gfLoginRegion->setCurrentIndex(Settings::getGlobalSettings()->getSetting("gfLoginRegion").toInt());
    ui->checkBox->setChecked(Settings::getGlobalSettings()->getSetting("experimentalLogin").toBool());
    ui->setting_loadLastSession->setChecked(Settings::getGlobalSettings()->getSetting("loadLastSession").toBool());

    ui->delay_login->setValue(Settings::getGlobalSettings()->getSetting("delay/login").toInt());
    ui->delay_login_var->setValue(Settings::getGlobalSettings()->getSetting("delay/loginvar").toInt());
    ui->delay_buff->setValue(Settings::getGlobalSettings()->getSetting("delay/buff").toInt());
    ui->delay_buff_var->setValue(Settings::getGlobalSettings()->getSetting("delay/buffvar").toInt());
    ui->delay_buy->setValue(Settings::getGlobalSettings()->getSetting("delay/buy").toInt());
    ui->delay_buy_var->setValue(Settings::getGlobalSettings()->getSetting("delay/buyvar").toInt());
    ui->delay_sell->setValue(Settings::getGlobalSettings()->getSetting("delay/sell").toInt());
    ui->delay_sell_var->setValue(Settings::getGlobalSettings()->getSetting("delay/sellvar").toInt());
    ui->delay_checksold->setValue(Settings::getGlobalSettings()->getSetting("delay/checksold").toInt());
    ui->delay_checksold_var->setValue(Settings::getGlobalSettings()->getSetting("delay/checksoldvar").toInt());
    ui->delay_ic->setValue(Settings::getGlobalSettings()->getSetting("delay/ic").toInt());
    ui->delay_ic_var->setValue(Settings::getGlobalSettings()->getSetting("delay/icvar").toInt());
    ui->delay_follow->setValue(Settings::getGlobalSettings()->getSetting("delay/follow").toInt());
    ui->delay_follow_var->setValue(Settings::getGlobalSettings()->getSetting("delay/followvar").toInt());
    ui->delay_mimic->setValue(Settings::getGlobalSettings()->getSetting("delay/mimic").toInt());
    ui->delay_mimic_var->setValue(Settings::getGlobalSettings()->getSetting("delay/mimicvar").toInt());
    ui->delay_other->setValue(Settings::getGlobalSettings()->getSetting("delay/other").toInt());
    ui->delay_other_var->setValue(Settings::getGlobalSettings()->getSetting("delay/othervar").toInt());

    ui->invMiniPacket->setText(Settings::getGlobalSettings()->getSetting("packets/minilandInv").toString());

}

void gSettingsUI::saveSettings()
{
    Settings::getGlobalSettings()->changeSetting("packets/minilandInv",ui->invMiniPacket->text().trimmed());

    Settings::getGlobalSettings()->changeSetting("delay/login",ui->delay_login->value());
    Settings::getGlobalSettings()->changeSetting("delay/loginvar",ui->delay_login_var->value());
    Settings::getGlobalSettings()->changeSetting("delay/buff",ui->delay_buff->value());
    Settings::getGlobalSettings()->changeSetting("delay/buffvar",ui->delay_buff_var->value());
    Settings::getGlobalSettings()->changeSetting("delay/buy",ui->delay_buy->value());
    Settings::getGlobalSettings()->changeSetting("delay/buyvar",ui->delay_buy_var->value());
    Settings::getGlobalSettings()->changeSetting("delay/sell",ui->delay_sell->value());
    Settings::getGlobalSettings()->changeSetting("delay/sellvar",ui->delay_sell_var->value());
    Settings::getGlobalSettings()->changeSetting("delay/checksold",ui->delay_checksold->value());
    Settings::getGlobalSettings()->changeSetting("delay/checksoldvar",ui->delay_checksold_var->value());
    Settings::getGlobalSettings()->changeSetting("delay/ic",ui->delay_ic->value());
    Settings::getGlobalSettings()->changeSetting("delay/icvar",ui->delay_ic_var->value());
    Settings::getGlobalSettings()->changeSetting("delay/follow",ui->delay_follow->value());
    Settings::getGlobalSettings()->changeSetting("delay/followvar",ui->delay_follow_var->value());
    Settings::getGlobalSettings()->changeSetting("delay/mimic",ui->delay_mimic->value());
    Settings::getGlobalSettings()->changeSetting("delay/mimicvar",ui->delay_mimic_var->value());
    Settings::getGlobalSettings()->changeSetting("delay/other",ui->delay_other->value());
    Settings::getGlobalSettings()->changeSetting("delay/othervar",ui->delay_other_var->value());


    Settings::getGlobalSettings()->changeSetting("logging/enabled",ui->enableLogging->isChecked());
    Settings::getGlobalSettings()->changeSetting("logging/packets",ui->enablePacketLogging->isChecked());
    Settings::getGlobalSettings()->changeSetting("loginvars/dat",ui->nostaleHash->text());
    Settings::getGlobalSettings()->changeSetting("loginvars/xdat",ui->nostaleXhash->text());
    Settings::getGlobalSettings()->changeSetting("loginvars/version",ui->nostaleVersion->text());
    Settings::getGlobalSettings()->changeSetting("loginvars/vendettaversion",ui->vendettaVersion->text());
    Settings::getGlobalSettings()->changeSetting("loginvars/vendettahash",ui->vendettaHash->text());
    Settings::getGlobalSettings()->changeSetting("proxy/ip",ui->proxyIP->text());
    Settings::getGlobalSettings()->changeSetting("proxy/port",ui->proxyPort->text());
    Settings::getGlobalSettings()->changeSetting("proxy/username",ui->proxyUsername->text());
    Settings::getGlobalSettings()->changeSetting("proxy/password",ui->proxyUsername->text());
    Settings::getGlobalSettings()->changeSetting("proxy/enabled",ui->useProxy->isChecked());
    Settings::getGlobalSettings()->changeSetting("notifications/enabled",ui->notificationsEnable->isChecked());
    Settings::getGlobalSettings()->changeSetting("notifications/akt4angel",ui->notificationAkt4Angel->isChecked());
    Settings::getGlobalSettings()->changeSetting("notifications/akt4demon",ui->notificationAkt4Demon->isChecked());
    Settings::getGlobalSettings()->changeSetting("notifications/whisper",ui->notificationWhisper->isChecked());
    Settings::getGlobalSettings()->changeSetting("notifications/mapchange",ui->notificationMapchange->isChecked());
    Settings::getGlobalSettings()->changeSetting("ic/staticPos",ui->icUseFixedPos->isChecked()?1:0);
    Settings::getGlobalSettings()->changeSetting("ic/Pos",QPoint(ui->icXPos->value(),ui->icYPos->value()));
    Settings::getGlobalSettings()->changeSetting("ic/autoDelay",ui->autoPosDelay->value());
    Settings::getGlobalSettings()->changeSetting("profiles/loadDelay",ui->characterLoadDelay->value());
    Settings::getGlobalSettings()->changeSetting("customServer/ip",ui->customServerIP->text());
    Settings::getGlobalSettings()->changeSetting("customServer/port",ui->customServerPort->value());
    Settings::getGlobalSettings()->changeSetting("language",ui->language->currentText());

    //Settings::getGlobalSettings()->changeSetting("gfLoginRegion",ui->gfLoginRegion->currentIndex());

    Settings::getGlobalSettings()->updateProxy();
    NosStrings::initialize(ui->language->currentText());
}

void gSettingsUI::on_enableLogging_toggled(bool checked)
{
    ui->enablePacketLogging->setEnabled(checked);
}

void gSettingsUI::on_useProxy_toggled(bool checked)
{
    ui->proxyIP->setEnabled(checked);
    ui->proxyPort->setEnabled(checked);
    ui->proxyUsername->setEnabled(checked);
    ui->proxyPassword->setEnabled(checked);
}

void gSettingsUI::on_stupidTest_toggled(bool checked)
{
    ui->nostaleHash->setEnabled(checked);
    ui->nostaleVersion->setEnabled(checked);
    ui->nostaleXhash->setEnabled(checked);
    ui->vendettaHash->setEnabled(checked);
    ui->vendettaVersion->setEnabled(checked);
}

void gSettingsUI::on_notificationsEnable_toggled(bool checked)
{
    ui->notificationAkt4Angel->setEnabled(checked);
    ui->notificationAkt4Demon->setEnabled(checked);
    ui->notificationWhisper->setEnabled(checked);
    ui->notificationMapchange->setEnabled(checked);
    ui->vendettaNoUpdate->setEnabled(checked);
    ui->nostaleNoUpdate->setEnabled(checked);
}

void gSettingsUI::on_icUseFixedPos_toggled(bool checked)
{
    ui->icXPos->setEnabled(checked);
    ui->icYPos->setEnabled(checked);
}

void gSettingsUI::on_resetTotalEarned_clicked()
{
    Settings::getGlobalSettings()->changeSetting("stats/totalEarned",0);
}

void gSettingsUI::on_autoreconnect_toggled(bool checked)
{
    Settings::getGlobalSettings()->changeSetting("login/autoreconnect",!checked);
}

void gSettingsUI::on_icmoving_toggled(bool checked)
{
    Settings::getGlobalSettings()->changeSetting("ic/move",checked);
}

void gSettingsUI::on_nostaleNoUpdate_toggled(bool checked)
{
    Settings::getGlobalSettings()->changeSetting("update/autoUpdateNostale",checked);
}

void gSettingsUI::on_vendettaNoUpdate_toggled(bool checked)
{
    Settings::getGlobalSettings()->changeSetting("update/autoUpdateVendetta",checked);
}

void gSettingsUI::on_useBuffsWithDelay_toggled(bool checked)
{
    Settings::getGlobalSettings()->changeSetting("autoBuffDelay",checked);
}

void gSettingsUI::on_checkBox_toggled(bool checked)
{
    Settings::getGlobalSettings()->changeSetting("experimentalLogin",checked);
}

void gSettingsUI::on_setting_loadLastSession_toggled(bool checked)
{
    Settings::getGlobalSettings()->changeSetting("loadLastSession",checked);
}

void gSettingsUI::on_cmb_login_version_currentIndexChanged(int index)
{
    Settings::getGlobalSettings()->changeSetting("gfLoginRegion",index);
}

