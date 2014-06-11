#include "sipevtthr.h"
#include <QDebug>

#if defined(Q_OS_WIN)
#include <winsock2.h>
#include "eXosip2/eXosip.h"
#elif defined(Q_OS_LINUX)
#include <netinet/in.h>
#include <eXosip2/eXosip.h>
#endif


SipEvtThr::SipEvtThr(const QMap<QString, QString> &inimap, QObject *parent) :
    QObject(parent) {
    if(inimap.count() > 0) {
        QMap<QString, QString>::const_iterator i;
        for(i = inimap.constBegin(); i != inimap.constEnd(); ++i) {
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
}

SipEvtThr::~SipEvtThr() {
    if(_localip != NULL) {
        delete []_localip;
    }
}

void SipEvtThr::evtloop() {
    //_exosipInit();
    while(true) {
        pevt = eXosip_event_wait(S, MS);
        if(pevt == NULL) {
            qDebug() << "noen";
            continue;
        }
        switch (pevt->type) {
        case EXOSIP_REGISTRATION_NEW:
            qDebug() << "new reg comming";
            break;
        default:
            qDebug() << "default";
            qDebug() << pevt->type;
            break;
        }
    }

}

//move to main loop
//win will crash here!
/*
int SipEvtThr::_exosipInit() {

    int ret = eXosip_init();
    if(ret != 0) {
        emit err("init err");
        return ret;
    }

    ret = eXosip_listen_addr(IPPROTO_UDP, _localip, _dftsip_port, AF_INET, 0);
    if(ret != 0) {
        eXosip_quit();
        emit err("listen bind err");
        return ret;
    }
    return 0;
}
*/
