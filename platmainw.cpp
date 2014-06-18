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
    if(_usercode != NULL) {
        delete []_usercode;
    }
}

void PlatMainW::_extUISetUp() {
    _videoview = new VideoView(ui->videow);
    QSize vsize(ui->videow->size());
    _videoview->setSize(vsize);

#if 1
    /*for debug*/
    _videoview->setHidden(true);
#endif

    /*set icon*/
    ui->b_left->setIcon(QIcon(":/icons/icons/left.png"));
    ui->b_right->setIcon(QIcon(":/icons/icons/right.png"));
    ui->b_up->setIcon(QIcon(":/icons/icons/up.png"));
    ui->b_down->setIcon(QIcon(":/icons/icons/down.png"));
    ui->b_left_up->setIcon(QIcon(":/icons/icons/up_left.png"));
    ui->b_left_down->setIcon(QIcon(":/icons/icons/down_left.png"));
    ui->b_right_up->setIcon(QIcon(":/icons/icons/up_right.png"));
    ui->b_right_down->setIcon(QIcon(":/icons/icons/down_right.png"));

    ui->btn_invate->setEnabled(false);
    ui->btn_stop->setEnabled(false);
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
    _usercode = NULL;

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
#if defined(Q_OS_WIN)
                _ipaddlen = _len;
#endif
                memset(_localip, 0, sizeof(char) * _len);
                strcpy(_localip, i.value().toStdString().c_str());
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
            } else if(QString::compare("user_code", i.key(), Qt::CaseInsensitive) == 0) {
                _usercode = new char[i.value().trimmed().length()];
                if(_usercode == NULL) {
                    exit(-1);
                }
                int _len = i.value().trimmed().length();
#if defined(Q_OS_WIN)
                _usercodelen = _len;
#endif
                memset(_usercode, 0, sizeof(char) * _len);
                strcpy(_usercode, i.value().toStdString().c_str());
            }
        }
    }
    qDebug() << _usercode;
    return;
}

void PlatMainW::_initSipEvtListener() {

    if(_initExosip() != 0) {
        exit(-1);
    }
    _evtthr = new QThread();
    _evtworker = new SipEvtThr(_dftsip_port, _dftrtp_port, _localip, _usercode);
    _evtworker->moveToThread(_evtthr);
    connect(_evtworker, SIGNAL(err(QString)), this, SLOT(evtLoopErr(QString)));
    connect(_evtworker, SIGNAL(info(QString)), this, SLOT(evtLoopInfo(QString)));
    connect(_evtworker, SIGNAL(succ(QString)), this, SLOT(evtLoopSucc(QString)));
    connect(_evtworker, SIGNAL(warn(QString)), this, SLOT(evtLoopWarn(QString)));
    connect(_evtworker, SIGNAL(rtp_start()), this, SLOT(startRecvRtp()));
    connect(_evtworker, SIGNAL(update_ResDisp(QString)), this, SLOT(updateResDisp(QString)));
    connect(_evtthr, SIGNAL(started()), _evtworker, SLOT(evtloop()));
    connect(_evtworker, SIGNAL(finished()), _evtthr, SLOT(quit()));
    connect(_evtworker, SIGNAL(finished()), _evtworker, SLOT(deleteLater()));
    connect(_evtworker, SIGNAL(finished()), _evtthr, SLOT(deleteLater()));
    _evtthr->start();
}

int PlatMainW::_initExosip() {

    /*for debug*/
    osip_trace_level_t t = TRACE_LEVEL7;
    TRACE_INITIALIZE(t, stdout);

    int ret = eXosip_init();
    if(ret != 0) {
        return -1;
    }
#if defined(Q_OS_WIN)
    /*on windows the string len not right
     uglily fix*/
    if(_ipaddlen > 0 && (int)strlen(_localip) > _ipaddlen) {
        *(_localip + _ipaddlen) = '\0';
    }
    if(_usercodelen > 0 && (int)strlen(_usercode) > _usercodelen) {
        *(_usercode) + _usercodelen = '\0';
    }
#endif
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
    _evtworker->send_INVITE();
#if 0
    _videoview->setLocalsdp(_sdpfile->fileName());
    _videoview->start();
#endif
}

void PlatMainW::on_btn_stop_clicked() {
    _evtworker->send_BYE();
}

void PlatMainW::evtLoopErr(QString err) {
    ui->txt_debug->setTextColor(Qt::darkRed);
    ui->txt_debug->append(err);
}

void PlatMainW::evtLoopInfo(QString info) {
    ui->txt_debug->setTextColor(Qt::black);
    ui->txt_debug->append(info);
}

void PlatMainW::evtLoopWarn(QString warn) {
    ui->txt_debug->setTextColor(Qt::darkYellow);
    ui->txt_debug->append(warn);
}

void PlatMainW::evtLoopSucc(QString succ) {
    ui->txt_debug->setTextColor(Qt::darkGreen);
    ui->txt_debug->append(succ);

}

void PlatMainW::startRecvRtp() {
    _videoview->setLocalsdp(_sdpfile->fileName());
    _videoview->start();
}

void PlatMainW::updateResDisp(QString s) {
    if(s.isEmpty()) {
        return;
    }
    if(ui->btn_invate->isEnabled() == false) {
        ui->btn_invate->setEnabled(true);
    }
    if(ui->btn_stop->isEnabled() == false) {
        ui->btn_stop->setEnabled(true);
    }
    ui->txtDev->clear();
    ui->txtDev->setTextColor(Qt::darkBlue);
    ui->txtDev->append(s);
}
