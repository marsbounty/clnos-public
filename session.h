#ifndef SESSION_H
#define SESSION_H

#include <QDialog>

namespace Ui {
class session;
}

class session : public QDialog
{
    Q_OBJECT

public:
    explicit session(bool load, QStringList currProfiles, QWidget *parent = nullptr);
    ~session();

private slots:
    void on_save_clicked();

    void on_load_clicked();

signals:
    void load(QStringList profiles);

private:
    Ui::session *ui;
    QStringList profiles;
};

#endif // SESSION_H
