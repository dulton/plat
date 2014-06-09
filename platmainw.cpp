#include "platmainw.h"
#include "ui_platmainw.h"

PlatMainW::PlatMainW(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PlatMainW)
{
    ui->setupUi(this);
}

PlatMainW::~PlatMainW()
{
    delete ui;
}
