#include "platmainw.h"
#include "ui_platmainw.h"
#include <QDebug>

PlatMainW::PlatMainW(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PlatMainW)
{
    ui->setupUi(this);
    extUISetUp();
}

PlatMainW::~PlatMainW()
{
    delete ui;
}

void PlatMainW::extUISetUp() {
    _videoview = new VideoView(ui->videow);
    QSize vsize(ui->videow->size());
    _videoview->setSize(vsize);
}
