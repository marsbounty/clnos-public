#ifndef TESTINGWINDOW_H
#define TESTINGWINDOW_H

#include <QDialog>

namespace Ui {
class testingwindow;
}

class testingwindow : public QDialog
{
    Q_OBJECT

public:
    explicit testingwindow(QWidget *parent = nullptr);
    ~testingwindow();

private:
    Ui::testingwindow *ui;
};

#endif // TESTINGWINDOW_H
