#include "sipevtthr.h"
#include <QDebug>
#include <QCryptographicHash>

#if defined(Q_OS_WIN)
#include <winsock2.h>
#include "eXosip2/eXosip.h"
#elif defined(Q_OS_LINUX)
#include <netinet/in.h>
#include <eXosip2/eXosip.h>
#endif


SipEvtThr::SipEvtThr(int sip_port, int rtp_port, char *local_ip, QObject *parent) :
    QObject(parent){
    _data.nonce = "9bd055";
    _data.auth_type = "Digest";
    _data.alg = "MD5";
    _data.local_ip = new char[strlen(local_ip)];
    if(_data.local_ip == NULL) {
        exit(-1);
    }
    strcpy(_data.local_ip, local_ip);
    _data.sip_port = sip_port;
    _data.rtp_port = rtp_port;
}

SipEvtThr::~SipEvtThr() {
    if(_data.local_ip != NULL) {
        delete []_data.local_ip;
        _data.local_ip = NULL;
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
        case EXOSIP_MESSAGE_NEW:
            if(MSG_IS_REGISTER(pevt->request)) {
                osip_authorization_t *auth;
                osip_message_get_authorization(pevt->request, 0, &auth);
                if(auth != NULL) {
                    int chk_ret = _chkRegInfo(auth->response, auth->username, "pass", auth->realm,
                                auth->uri, osip_message_get_method(pevt->request),
                                auth->nonce);
                    if(chk_ret != 0) {
                        qDebug() << "break on check";
                        break;
                    }
                    chk_ret = _cmpRespMd5(auth->response, auth->username, "pass", auth->realm,
                                auth->uri, osip_message_get_method(pevt->request),
                                auth->nonce);
                    if(chk_ret == 0) {
                        eXosip_lock();
                        _send_2xxReg(pevt);
                        eXosip_unlock();
                    } else {
                        eXosip_lock();
                        _send_401Reg(pevt,
                                     _data.local_ip,  (char *)_data.nonce,
                                     (char *)_data.alg, (char *)_data.auth_type);
                        eXosip_unlock();
                    }
                } else {
                    eXosip_lock();
                    _send_401Reg(pevt,
                       _data.local_ip,  (char *)_data.nonce, (char *)_data.alg, (char *)_data.auth_type);
                    eXosip_unlock();
                }

            }
            break;
        default:
            qDebug() << "default";
            qDebug() << pevt->type;
            break;
        }
    }

}

int SipEvtThr::_send_401Reg(eXosip_event_t *e,
                            char *ipaddr, char *nonce, char *alg, char *auth_type) {

    if(e == NULL) {
        return -1;
    }
    if(ipaddr == NULL || nonce == NULL || alg == NULL || auth_type == NULL) {
        return -2;
    }
    osip_www_authenticate_t *www_hdr;
    osip_www_authenticate_init(&www_hdr);
    osip_www_authenticate_set_auth_type(www_hdr, auth_type);
    osip_www_authenticate_set_algorithm(www_hdr, alg);

    char buf_nonce[128];
    memset(buf_nonce, 0, sizeof(buf_nonce)/sizeof(buf_nonce[0]));
    /*fix len for ipaddr and nonce*/
    _addQuote(nonce, strlen(nonce), buf_nonce, sizeof(buf_nonce)/sizeof(buf_nonce[0]));
    osip_www_authenticate_set_nonce(www_hdr, buf_nonce);

    char buf_ipaddr[128];
    memset(buf_ipaddr, 0, sizeof(buf_ipaddr)/sizeof(buf_ipaddr[0]));
    _addQuote(ipaddr, strlen(ipaddr), buf_ipaddr, sizeof(buf_ipaddr)/sizeof(buf_ipaddr[0]));
    osip_www_authenticate_set_realm(www_hdr, buf_ipaddr);

    osip_message_t *ans = NULL;
    char *hdr_str = NULL;
    eXosip_message_build_answer(e->tid, 401, &ans);
    osip_www_authenticate_to_str(www_hdr, &hdr_str);
    if(hdr_str != NULL) {
        qDebug() << "set auth val ret:" << osip_message_set_www_authenticate(ans, hdr_str);
        qDebug() << "send ret : "<<eXosip_message_send_answer(e->tid, 401, ans);
    }
    return 0;
}

int SipEvtThr::_send_2xxAns(eXosip_event_t *e) {
    if(e == NULL) {
        return -1;
    }
    osip_message_t *ans;
    eXosip_message_build_answer(e->tid, 200, &ans);
    eXosip_message_send_answer(e->tid, 200, ans);
    return 0;
}

int SipEvtThr::_addQuote(char *str, int len, char *out, int olen) {
    if(str == NULL || len <= 0 || out == NULL || olen < len + 2) {
        return -1;
    }
    QString tmp(str);
    tmp.prepend("\"");
    tmp.append("\"");
    if(olen >= tmp.length()) {
        for(int i = 0; i < tmp.length(); i++) {
            *(out + i) = tmp.at(i).toLatin1();
        }
    }
    return 0;
}

QString SipEvtThr::_rmQuote(char *str) {
    QString ret;
    if(str != NULL && strlen(str) > 0) {
        ret.append(str);
        if(ret.startsWith("\"")) {
            ret.remove(0, 1);
        }
        if(ret.endsWith("\"")) {
            ret.chop(1);
        }
    }
    return ret;
}


int SipEvtThr::_cmpRespMd5(char *resp, char *username, char *pass,
                           char *relam, char *uri, char *method, char *nonce) {
    QString qresp = _rmQuote(resp);
    QString qusrname = _rmQuote(username);
    QString qpass = _rmQuote(pass);
    QString qrelam = _rmQuote(relam);
    QString quri = _rmQuote(uri);
    QString qmethod = _rmQuote(method);
    QString qnonce = _rmQuote(nonce);

    QCryptographicHash ha1(QCryptographicHash::Md5);
    QCryptographicHash ha2(QCryptographicHash::Md5);
    QCryptographicHash ha3(QCryptographicHash::Md5);
    QString ha1_calc = qusrname + QString(":") + qrelam + QString(":") + qpass;
    QString ha2_calc = qmethod + QString(":") + quri;

    ha1.addData(ha1_calc.toStdString().c_str(),
                strlen(ha1_calc.toStdString().c_str()));
    ha2.addData(ha2_calc.toStdString().c_str(),
                strlen(ha2_calc.toStdString().c_str()));


    QString ha3_calc;
    ha3_calc.append(ha1.result().toHex().data());
    ha3_calc.append(":");
    ha3_calc.append(qnonce);
    ha3_calc.append(":");
    ha3_calc.append(ha2.result().toHex().data());
    ha3.addData(ha3_calc.toStdString().c_str(),
                strlen(ha3_calc.toStdString().c_str()));

    QString fin(ha3.result().toHex().data());
    if(fin.compare(qresp) == 0) {
        return 0;
    }
    return -1;
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
