#include "platmainw.h"
#include "ui_platmainw.h"
#include <QDebug>
#include <QResource>

#if defined(Q_OS_WIN)
#include <WinSock2.h>
#include "eXosip2/eXosip.h"
#elif defined(Q_OS_LINUX)
#include <netinet/in.h>
#include <eXosip2/eXosip.h>
#endif

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
    if(_localip != NULL) {
        delete []_localip;
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

    _dftrtp_port = 0;
    _dftsip_port = 0;
    _localip = NULL;

    /*simple vilidate*/
    if(_setmap.count() > 0) {
        QMap<QString, QString>::const_iterator i;
        for(i = _setmap.constBegin(); i != _setmap.constEnd(); ++i) {
            if(QString::compare("local_ip", i.key(), Qt::CaseInsensitive) == 0) {
                _localip = new char[i.value().trimmed().length()];
                if(_localip == NULL) {
                    exit(-1);
                }
                int _len = i.value().trimmed().length();
                for(int cnt = 0; cnt < _len; cnt++) {
                    _localip[cnt] = i.value().at(cnt).toLatin1();
                }
            } else if(QString::compare("sip_port", i.key(), Qt::CaseInsensitive) == 0) {
                bool cvtflg = false;
                _dftsip_port = i.value().toInt(&cvtflg);
                if(!cvtflg) {
                    _dftsip_port = 5060;
                }
            } else if(QString::compare("rtp_port", i.key(), Qt::CaseInsensitive) == 0) {
                bool cvtflg = false;
                _dftrtp_port = i.value().toInt(&cvtflg);
                if(!cvtflg) {
                    _dftrtp_port = 1576;
                }
            }
        }
    }
    return;
}

void PlatMainW::_initSipEvtListener() {

    if(_initExosip() != 0) {
        exit(-1);
    }
    _evtthr = new QThread();
    _evtworker = new SipEvtThr(_dftsip_port, _dftrtp_port, _localip);
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

int PlatMainW::_initExosip() {
    int ret = eXosip_init();
    if(ret != 0) {
        return -1;
    }
    if(_localip == NULL && _dftsip_port == 0) {
        ret = eXosip_listen_addr(IPPROTO_UDP, NULL, 15060, AF_INET, 0);
    } else if(_localip != NULL && _dftsip_port != 0) {
        ret = eXosip_listen_addr(IPPROTO_UDP, _localip, _dftsip_port, AF_INET, 0);
    } else if(_localip == NULL && _dftsip_port != 0) {
        ret = eXosip_listen_addr(IPPROTO_UDP, NULL, _dftsip_port, AF_INET, 0);
    } else if(_localip != NULL && _dftsip_port == 0) {
        ret = eXosip_listen_addr(IPPROTO_UDP, _localip, 15060, AF_INET, 0);
    }
    if(ret != 0) {
        eXosip_quit();
        return -2;
    }
    return ret;
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
