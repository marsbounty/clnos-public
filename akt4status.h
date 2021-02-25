#ifndef AKT4STATUS_H
#define AKT4STATUS_H

#include <QDialog>
#include <QTime>

namespace Ui {
class akt4status;
}

struct akt4s{
    int demon;
    int angel;
    int timeleft;
    QTime time;
};

class Player;

class akt4status : public QDialog
{
    Q_OBJECT

public:
    explicit akt4status(QWidget *parent = 0);
    static void updateStat(QStringList packet,Player* ply);
    ~akt4status();

private slots:
    void update();

private:
    Ui::akt4status *ui;

    void setLastUpdated(QTime time);


    static QMap<QString,akt4s> stats;


};

#endif // AKT4STATUS_H
