#include "testingwindow.h"
#include "ui_testingwindow.h"

testingwindow::testingwindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::testingwindow)
{
    ui->setupUi(this);
}

testingwindow::~testingwindow()
{
    delete ui;
}
