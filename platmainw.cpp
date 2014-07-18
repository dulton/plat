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
#include "ptzinfo.h"
#include <QTimer>
#include <QEventLoop>

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
    eXosip_quit();
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
    if(_camcode != NULL) {
        delete []_camcode;
    }
}

void PlatMainW::_extUISetUp() {
    _videoview = new VideoView(ui->videow);
    QSize vsize(ui->videow->size());
    _videoview->setSize(vsize);

#if 0
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

    ptzbtns[0] = ui->b_up;
    ptzbtns[1] = ui->b_down;
    ptzbtns[2] = ui->b_left;
    ptzbtns[3] = ui->b_right;
    ptzbtns[4] = ui->b_right_up;
    ptzbtns[5] = ui->b_right_down;
    ptzbtns[6] = ui->b_left_up;
    ptzbtns[7] = ui->b_left_down;

    unsigned int btnlen = sizeof(ptzbtns) / sizeof(ptzbtns[0]);
    for(unsigned int i = 0; i < btnlen; i++) {
        ptzbtns[i]->setEnabled(false);
    }

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
    _camcode = NULL;
    _ptz_timeout = 2000;

    /*simple vilidate*/
    if(_setmap.count() > 0) {
        QMap<QString, QString>::const_iterator i;
        for(i = _setmap.constBegin(); i != _setmap.constEnd(); ++i) {
            if(QString::compare("local_ip", i.key(), Qt::CaseInsensitive) == 0) {
                try {
                    _localip = new char[i.value().trimmed().length() + 1];
                } catch (...) {
                    qDebug() << "new failed";
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
                try {
                    _usercode = new char[i.value().trimmed().length() + 1];
                } catch(...) {
                    qDebug() << "new failed";
                    exit(-1);
                }
                int _len = i.value().trimmed().length();
#if defined(Q_OS_WIN)
                _usercodelen = _len;
#endif
                memset(_usercode, 0, sizeof(char) * _len);
                strcpy(_usercode, i.value().toStdString().c_str());

            } else if(QString::compare("cam_code", i.key(), Qt::CaseInsensitive) == 0) {
                try {
                    _camcode = new char[i.value().trimmed().length() + 1];
                } catch(...) {
                    qDebug() << "new failed";
                    exit(-1);
                }
                int _len = i.value().trimmed().length();
#if defined(Q_OS_WIN)
                _usercodelen = _len;
#endif
                memset(_camcode, 0, sizeof(char) * _len);
                strcpy(_camcode, i.value().toStdString().c_str());
            }
        }
    }
    qDebug() << _usercode;
    qDebug() << _camcode;
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
        *(_usercode + _usercodelen) = '\0';
    }
    if(_camcodelen > 0 && (int)strlen(_camcode) > _camcodelen) {
        *(_camcode + _camcodelen) = '\0';
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

void PlatMainW::_ptz_send_cb(PTZ_CMD cmd) {
    if(cmd != PTZ_CMD_NONE) {
        if(_chk_ptz_cmd(cmd) == CMD_DI) {
            PtzInfo ctl_info(_camcode);
            ctl_info.setPtzcmd(cmd);
            ctl_info.setPara1(SPEED5);
            ctl_info.setPara2(SPEED5);
            _evtworker->send_PTZ_DI_CTL(ctl_info);
        }
    }
    return;
}

void PlatMainW::on_btn_invate_clicked() {
    if(!_videoview->isStart()) {
        _evtworker->send_INVITE();
    }

#if 0
    _videoview->setLocalsdp(_sdpfile->fileName());
    _videoview->start();
#endif
}

void PlatMainW::on_btn_stop_clicked() {
    if(_videoview->isStart()) {
        _evtworker->send_BYE();
        _videoview->stop();
    }
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
    unsigned int btnlen = sizeof(ptzbtns) / sizeof(ptzbtns[0]);
    for(unsigned int i = 0; i < btnlen; i++) {
        if(ptzbtns[i]->isEnabled() == false) {
            ptzbtns[i]->setEnabled(true);
        }

    }
    ui->txtDev->clear();
    ui->txtDev->setTextColor(Qt::darkBlue);
    ui->txtDev->append(s);
}

/*for ptzs*/
void PlatMainW::on_b_left_up_pressed() {
    /*para = start same as stop*/
    if(_chk_can_ctlptz()) {
        _ptz_send_cb(PTZ_UP_LEFT_START);
    }

}
void PlatMainW::on_b_left_down_pressed() {
    if(_chk_can_ctlptz()) {
        _ptz_send_cb(PTZ_DOWN_LEFT_START);
    }
}

void PlatMainW::on_b_right_up_pressed() {
    if(_chk_can_ctlptz()) {
        _ptz_send_cb(PTZ_UP_RIGHT_START);
    }
}

void PlatMainW::on_b_right_down_pressed() {
    if(_chk_can_ctlptz()) {
        _ptz_send_cb(PTZ_DOWN_RIGHT_START);
    }
}

void PlatMainW::on_b_up_pressed() {
    if(_chk_can_ctlptz()) {
        _ptz_send_cb(PTZ_UP_START);
    }
}

void PlatMainW::on_b_down_pressed() {
    if(_chk_can_ctlptz()) {
        _ptz_send_cb(PTZ_DOWN_START);
    }
}
void PlatMainW::on_b_left_pressed() {
    if(_chk_can_ctlptz()) {
        _ptz_send_cb(PTZ_LEFT_START);
    }
}
void PlatMainW::on_b_right_pressed() {
    if(_chk_can_ctlptz()) {
        _ptz_send_cb(PTZ_RIGHT_START);
    }
}


void PlatMainW::on_b_right_up_released() {
    if(_chk_can_ctlptz()) {
        _ptz_send_cb(PTZ_UP_RIGHT_STOP);
    }
}

void PlatMainW::on_b_right_released() {
    if(_chk_can_ctlptz()) {
        _ptz_send_cb(PTZ_RIGHT_STOP);
    }
}

void PlatMainW::on_b_right_down_released() {
    if(_chk_can_ctlptz()) {
        _ptz_send_cb(PTZ_DOWN_RIGHT_STOP);
    }
}

void PlatMainW::on_b_down_released() {
    if(_chk_can_ctlptz()) {
        _ptz_send_cb(PTZ_DOWN_STOP);
    }
}

void PlatMainW::on_b_left_down_released() {
    if(_chk_can_ctlptz()) {
        _ptz_send_cb(PTZ_DOWN_LEFT_STOP);
    }
}

void PlatMainW::on_b_left_released() {
    if(_chk_can_ctlptz()) {
        _ptz_send_cb(PTZ_LEFT_STOP);
    }

}

void PlatMainW::on_b_left_up_released() {
    if(_chk_can_ctlptz()) {
        _ptz_send_cb(PTZ_UP_LEFT_STOP);
    }
}

void PlatMainW::on_b_up_released() {
    if(_chk_can_ctlptz()) {
        _ptz_send_cb(PTZ_UP_STOP);
    }
}
