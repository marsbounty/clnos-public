#ifndef NEWBOTDIALOG_H
#define NEWBOTDIALOG_H

#include <QDialog>

namespace Ui {
class NewBotDialog;
}

class NewBotDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewBotDialog(QStringList profiles, QWidget *parent = 0);
    ~NewBotDialog();

signals:
    void clicked(QString name,bool autologin, bool abort);

private slots:
    void on_load_profile_clicked();
    void on_new_profile_2_clicked();

private:
    Ui::NewBotDialog *ui;
};

#endif // NEWBOTDIALOG_H
