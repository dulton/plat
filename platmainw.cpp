#include "platmainw.h"
#include "ui_platmainw.h"
#include <QDebug>
#include <QResource>

#include <eXosip2/eXosip.h>
#include <netinet/in.h>


PlatMainW::PlatMainW(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PlatMainW)
{
    ui->setupUi(this);
    _initCfg();
    _initSipEvtListener();
    _extDataSetUp();
    _extUISetUp();
}

PlatMainW::~PlatMainW()
{
    delete ui;
    if(_sdpfile != NULL) {
        if(_sdpfile->isOpen()) {
            _sdpfile->close();
        }
        delete _sdpfile;
    }
}

void PlatMainW::_extUISetUp() {
    _videoview = new VideoView(ui->videow);
    QSize vsize(ui->videow->size());
    _videoview->setSize(vsize);
}

void PlatMainW::_extDataSetUp() {
    QResource res(":/sdp/cat.sdp");
    QFile res_sdp(res.absoluteFilePath());
    if(!res_sdp.open(QIODevice::ReadOnly)) {
        return;
    }
    _sdpfile = new QFile("./cat.sdp");
    if(_sdpfile->exists()) {
        if(!QFile::remove(_sdpfile->fileName())) {
            return;
        }
    }
    if(_sdpfile->open(QIODevice::ReadWrite)) {
        QTextStream out(_sdpfile);
        out << res_sdp.readAll();
    }
    res_sdp.close();
    _sdpfile->close();

    return;
}

void PlatMainW::_initCfg() {
    _settings = new Settings("./plat.ini");
    _setmap = _settings->readGrp("APP_CFG");
    /*default values*/
}

void PlatMainW::_initSipEvtListener() {

    /*
    int ret = eXosip_init();
    if(ret != 0) {
        return;
    }
    ret = eXosip_listen_addr(IPPROTO_UDP, NULL, 5060, AF_INET, 0);
    if(ret != 0) {
        eXosip_quit();
        return;
    }
    */

    _evtthr = new QThread();
    _evtworker = new SipEvtThr(_setmap);
    _evtworker->moveToThread(_evtthr);
    connect(_evtworker, SIGNAL(err(QString)),
            this, SLOT(evtLoopErr(QString)));
    connect(_evtthr, SIGNAL(started()),
            _evtworker, SLOT(evtloop()));
    connect(_evtworker, SIGNAL(finished()),
            _evtthr, SLOT(quit()));
    connect(_evtworker, SIGNAL(finished()),
            _evtworker, SLOT(deleteLater()));
    connect(_evtworker, SIGNAL(finished()),
            _evtthr, SLOT(deleteLater()));
    _evtthr->start();
}

void PlatMainW::on_btn_invate_clicked() {
#if 1
    _videoview->setLocalsdp(_sdpfile->fileName());
    _videoview->start();
#endif
}

void PlatMainW::on_btn_stop_clicked() {

}

void PlatMainW::evtLoopErr(QString err) {
    qDebug() << err;
}
