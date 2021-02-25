#ifndef GSETTINGSUI_H
#define GSETTINGSUI_H

#include <QDialog>

namespace Ui {
class gSettingsUI;
}

class gSettingsUI : public QDialog
{
    Q_OBJECT

public:
    explicit gSettingsUI(QWidget *parent = 0);
    ~gSettingsUI();

private slots:
    void on_close_clicked();
    void on_save_clicked();

    void on_enableLogging_toggled(bool checked);

    void on_useProxy_toggled(bool checked);

    void on_stupidTest_toggled(bool checked);

    void on_notificationsEnable_toggled(bool checked);

    void on_icUseFixedPos_toggled(bool checked);

    void on_resetTotalEarned_clicked();

    void on_autoreconnect_toggled(bool checked);

    void on_icmoving_toggled(bool checked);

    void on_nostaleNoUpdate_toggled(bool checked);

    void on_vendettaNoUpdate_toggled(bool checked);

    void on_useBuffsWithDelay_toggled(bool checked);

    void on_checkBox_toggled(bool checked);

    void on_setting_loadLastSession_toggled(bool checked);

    void on_cmb_login_version_currentIndexChanged(int index);


private:
    Ui::gSettingsUI *ui;
    void loadSettings();
    void saveSettings();
};

#endif // GSETTINGSUI_H
