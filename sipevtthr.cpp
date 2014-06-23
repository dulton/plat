#include "sipevtthr.h"
#include <QDebug>
#include <QCryptographicHash>
#include <QXmlStreamReader>
#include <QXmlStreamAttribute>
#include <QXmlStreamAttributes>
#include <QTime>
#include <QStringList>

#if defined(Q_OS_WIN)
#include <winsock2.h>
#include "eXosip2/eXosip.h"
#define strncasecmp strnicmp
#elif defined(Q_OS_LINUX)
#include <netinet/in.h>
#include <eXosip2/eXosip.h>
#endif


SipEvtThr::SipEvtThr(int sip_port, int rtp_port, char *local_ip, char *user_code, QObject *parent) :
    QObject(parent){
    _data.nonce = "9bd055";
    _data.auth_type = "Digest";
    _data.alg = "MD5";
    _data.dft_pass = "pass";
    _data.dft_sc_type = "application/sdp";
    _data.rtp_playload = 100;
    _data.local_ip = new char[strlen(local_ip)];
    _callinfo.cid = -1;
    _callinfo.did = -1;
    if(_data.local_ip == NULL) {
        exit(-1);
    }
    strcpy(_data.local_ip, local_ip);

    _data.user_code = new char[strlen(user_code)];
    if(_data.user_code == NULL) {
        exit(-1);
    }
    strcpy(_data.user_code, user_code);

    _data.sip_port = sip_port;
    _data.rtp_port = rtp_port;
    _uset = new Settings("./uset.ini");
}

SipEvtThr::~SipEvtThr() {
    if(_data.local_ip != NULL) {
        delete []_data.local_ip;
        _data.local_ip = NULL;
    }
}
/**
  @brief sip evt main loop
*/
void SipEvtThr::evtloop() {
    QString msg2c;
    while(true) {
        pevt = eXosip_event_wait(S, MS);
        if(pevt == NULL) {
            qDebug() << "none event";
            continue;
        }
        switch (pevt->type) {
        case EXOSIP_REGISTRATION_NEW:
            qDebug() << "new reg comming";
            break;
        case EXOSIP_MESSAGE_NEW:
            if(MSG_IS_REGISTER(pevt->request)) {
                _prcsReg(pevt);
            } else if(MSG_IS_NOTIFY(pevt->request)) {
                _prcsNotify(pevt);
            }
            break;
        case EXOSIP_CALL_ANSWERED:
            if(MSG_IS_INVITE(pevt->request)) {
                _prcsINVITE(pevt);
            }
            break;
        case EXOSIP_CALL_PROCEEDING:
            {
                msg2c.clear();
                msg2c = _fmtMsg("remote proceeding video call");
                emit succ(msg2c);
            }
            break;
        case EXOSIP_CALL_RINGING:
            {
                msg2c.clear();
                msg2c = _fmtMsg("recive call ringing msg");
                emit succ(msg2c);
            }
            break;
        case EXOSIP_CALL_MESSAGE_ANSWERED:
            {
                msg2c.clear();
                msg2c = _fmtMsg("revive client 200 ok msg");
                emit succ(msg2c);
            }
            break;
        case EXOSIP_CALL_RELEASED:
            {
                msg2c.clear();
                msg2c = _fmtMsg("call release success!");
                emit succ(msg2c);
            }
            break;
        default:
            {
                msg2c.clear();
                msg2c.append("recive none process msg: Type ");
                msg2c.append(QString::number(pevt->type));
                emit warn(_fmtMsg(msg2c));
            }
            break;
        }
    }

}

/**
  @brief read the cfg file get info then send INVITE to remote
         for single client invate video test
*/
void SipEvtThr::send_INVITE() {

    QStringList clist = _uset->childGroups();
    if(clist.count() >= 1) {
        QString inv_to;
        QString inv_from;
        QString remote_ip;
        QString dev_code;
        int remote_port;
        /*just pick the first one*/
        dev_code = clist.at(0);
        if(dev_code.isEmpty()) {
            qDebug() << "grp is empty pls chk the record function";
            return;
        }
        remote_ip = _uset->readGKV(dev_code, "ip_addr");
        bool f;
        remote_port = _uset->readGKV(dev_code, "port").toInt(&f);
        if(!f) {
            remote_port = 5061;
        }
        inv_to = _bdFTC((char *)dev_code.toStdString().c_str(),
                        (char *)remote_ip.toStdString().c_str(), remote_port);
        inv_from = _bdFTC(_data.user_code, _data.local_ip, _data.sip_port);

#if 0
        qDebug() << inv_to;
        qDebug() << inv_from;
#endif
        osip_message_t *invate;
        /*this sdp msg may not right but
         *plat will not use this sdp so dosen't matter*/
        QString sdp_msg = _bdSDPMsg(_data.local_ip, _data.local_ip, _data.sip_port, _data.rtp_playload);
        int b_ret = eXosip_call_build_initial_invite(&invate, inv_to.toStdString().c_str(),
                                                                     inv_from.toStdString().c_str(),
                                                                     NULL,
                                                                     "THIS");
        qDebug() << "build ret for init invate" << b_ret;
        osip_message_set_content_type(invate, _data.dft_sc_type);
        osip_message_set_body(invate, sdp_msg.toStdString().c_str(),
                              strlen(sdp_msg.toStdString().c_str()));
        eXosip_call_send_initial_invite(invate);
    }
}

/**
   @brief send buy to terminate a video call
*/
void SipEvtThr::send_BYE() {
    if(_callinfo.cid != -1 && _callinfo.did != -1) {
        if(eXosip_call_terminate(_callinfo.cid, _callinfo.did) != 0) {
            QString errs = _fmtMsg("terminate call failed!");
            emit err(errs);
        } else {
            QString succs = _fmtMsg("terminate call succcess!");
            emit succ(succs);
        }
    }
}

/**
   @brief send ptz direction ctl msg
*/
void SipEvtThr::send_PTZ_DI_CTL(const PtzInfo &info) {
    qDebug() << info.getXmlMsg();
    osip_message_t *ptzmsg;
    //eXosip_message_build_request(&ptzmsg, "MESSAGE", )
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

/**
   @brief send ans for any status
   @return
*/
int SipEvtThr::_send_AnsStatus(eXosip_event_t *e, int status) {
    if(e == NULL && (status <= 100 || status > 699)) {
        return -1;
    }
    int b_ret = -1;
    osip_message_t *ans;
    b_ret = eXosip_message_build_answer(e->tid, status, &ans);
    if(b_ret != 0) {
        /*some times build return fail but send will be success
         *so in this case should not return*/
        qDebug() << "build ans 2xx failed " << b_ret;
    }
    /*if the req method is NOTIFY exosip 3.3 will not send 200 ok
     *use exosip 3.6 will slove this prob!!*/
    //osip_message_set_status_code(ans, 200);
    b_ret = eXosip_message_send_answer(e->tid, status, ans);
    if(b_ret != 0) {
        /*the same as above(build)
         **/
        qDebug() << "send ans 2xx failed " << b_ret;
    }

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

void SipEvtThr::_prcsReg(eXosip_event_t *e) {
    /*e is not null here!*/
    osip_authorization_t *auth;
    osip_message_get_authorization(e->request, 0, &auth);
    if(auth != NULL) {
        /*chk and cmp md5 use default pass word*/
        int chk_ret = _chkRegInfo(auth->response, auth->username, (char *)_data.dft_pass, auth->realm,
                                  auth->uri, osip_message_get_method(e->request),
                                  auth->nonce);
#if 0
        qDebug() << auth->response;
        qDebug() << auth->username;
        qDebug() << auth->realm;
        qDebug() << auth->uri;
        qDebug() << osip_message_get_method(e->request);
        qDebug() << auth->nonce;
#endif
        if(chk_ret != 0) {
            QString warns_s = _fmtMsg("recv reg msg REG INFO not complete!");
            emit warn(warns_s);
            return;
        }
        chk_ret = _cmpRespMd5(auth->response, auth->username, (char *)_data.dft_pass, auth->realm,
                              auth->uri, osip_message_get_method(e->request),
                              auth->nonce);
        if(chk_ret == 0) {
            QString succs = _fmtMsg("MD5 chk success!");
            emit succ(succs);

            eXosip_lock();
            _send_AnsStatus(e, 200);
            eXosip_unlock();

            succs.clear();
            succs = QString ("%1 %2 %3").arg("client:").arg(auth->username).arg(" reg success!");
            emit succ(_fmtMsg(succs));

            osip_contact_t *ctinfo = NULL;
            osip_message_get_contact(e->request, 0, &ctinfo);
            osip_via_t *viainfo = NULL;
            osip_message_get_via(e->request, 0, &viainfo);

            _recContractVia(ctinfo, viainfo);
            return;
        } else {
            eXosip_lock();
            _send_401Reg(e,
                         _data.local_ip,  (char *)_data.nonce,
                         (char *)_data.alg, (char *)_data.auth_type);
            eXosip_unlock();
            QString infos = _fmtMsg("send 401 Unauth to client");
            emit info(infos);
            return;
        }
    } else {
        eXosip_lock();
        _send_401Reg(e,
                     _data.local_ip,  (char *)_data.nonce, (char *)_data.alg, (char *)_data.auth_type);
        eXosip_unlock();
        QString infos = _fmtMsg("send 401 Unauth to client");
        emit info(infos);
        return;
    }
    return;
}

void SipEvtThr::_prcsNotify(eXosip_event_t *e) {
    /* !FIXME:
     * need some verification here
     */
    osip_content_length_t *clen = NULL;
    clen = osip_message_get_content_length(e->request);
    if(clen != NULL) {
        QString infos;
        infos.append("Notify recived length is: ");
        infos.append(clen->value);
        emit info(_fmtMsg(infos));
    }
    osip_body_t *bd = NULL;
    osip_body_init(&bd);
    osip_message_get_body(e->request, 0, &bd);
    if(bd != NULL) {
        QString infos;
        infos.append("Nofity recived msg is: ");
        if(bd->body != NULL) {
            infos.append(bd->body);
            QString dev_s = _readXmlNOTIFY(bd->body);
            emit update_ResDisp(dev_s);
        } else {
            infos.append("null");
        }
        emit info(_fmtMsg(infos));
    }
    eXosip_lock();
    _send_AnsStatus(e, 200);
    eXosip_unlock();
    return;

}

void SipEvtThr::_prcsINVITE(eXosip_event_t *e) {
    osip_content_type_t *ct = NULL;
    ct = osip_message_get_content_type(e->request);
    if(_chkSipContentType(ct, E_SDP) == 0) {
        eXosip_lock();
        /*send ack*/
        osip_message_t *inv_ack;
        int b_ret = -1;
        b_ret = eXosip_call_build_ack(e->did, &inv_ack);
        qDebug() << "ack build ret :" << b_ret;
        b_ret = eXosip_call_send_ack(e->did, inv_ack);
        qDebug() << "ack send ret :" << b_ret;

        _callinfo.cid = e->cid;
        _callinfo.did = e->did;

        eXosip_unlock();
        /*start waiting rtp stream here!*/
        emit rtp_start();
        QString succs = _fmtMsg("send ack to client, wait for video!");
        emit succ(succs);
    }
    qDebug() << "chk content failed" << ct->type << "/" <<ct->subtype;

    //QString succs("recived 180 Ring.. wait for 200 OK!");
    //emit succ(succs);
    return;
}

void SipEvtThr::_recContractVia(osip_contact_t *c, osip_via_t *v) {
    if(c != NULL && c->url != NULL &&
       c->url->username != NULL && c->url->host != NULL &&
       v != NULL && v->port != NULL) {
        _uset->writeGrp(c->url->username, "pass", _data.dft_pass);
        _uset->writeGrp(c->url->username, "ip_addr", c->url->host);
        _uset->writeGrp(c->url->username, "port", v->port);
    }
    return;
}

int SipEvtThr::_chkSipContentType(osip_content_type_t *t, int type) {
    if(t == NULL || t->subtype == NULL || t->type == NULL) {
        return -1;
    }
    if(strncasecmp(t->type, "application", strlen("application")) == 0) {
        if(type == E_SDP) {
            if(strncasecmp(t->subtype, "sdp", strlen("sdp")) == 0) {
                return 0;
            }
        } else if(type == E_XML) {
            if(strncasecmp(t->subtype, "xml", strlen("xml")) == 0) {
                return 0;
            }
        }
    }
    return 1;
}

QString SipEvtThr::_bdSDPMsg(char *oip, char *cip, int lport, int payload) {
    QString retStr;
    if(oip != NULL && oip[0] != '\0' &&
       cip != NULL && cip[0] != '\0' &&
       lport > 0 && payload > 0 && payload <= 100) {
        retStr.append("v=0\r\n");
        retStr.append(QString("o=- 0 0 IN IP4 %1\r\n").arg(oip));
        retStr.append("s=-\r\n");
        retStr.append(QString("c=IN IP4 %1\r\n").arg(cip));
        retStr.append(QString("m=video %1 RTP/AVP %2\r\n").arg(lport).arg(payload));
        retStr.append(QString("a=rtpmap:%1 H.264/90000\r\n").arg(payload));
        retStr.append(QString("a=fmtp:%1 CIF=1\r\n").arg(payload));
        retStr.append("a=sendrecv\r\n");
    }
    return retStr;
}

QString SipEvtThr::_bdFTC(char *code, char *ip, int port) {
    //"sip:100010000004020001@192.168.1.168"
    QString hdr = "sip:";
    if(code != NULL && code[0] != '\0' &&
       ip != NULL && ip[0] != '\0' && port > 0) {
        hdr.append(code);
        hdr.append("@");
        hdr.append(ip);
        hdr.append(":");
        hdr.append(QString::number(port));
        return hdr;
    }
    hdr.clear();
    return hdr;
}

QString SipEvtThr::_readXmlNOTIFY(char *msg) {
    QString ret;
    if(msg == NULL || msg[0] == '\0') {
        return ret;
    }
    QXmlStreamReader reader(msg);

    while(!reader.atEnd() && !reader.hasError()) {
        reader.readNext();
        if(reader.isStartElement()) {
            if(reader.name() == "Code") {
                ret.append("Code: " + reader.readElementText());
                ret.append("\n");
                continue;
            }
            QXmlStreamAttributes attrs;
            QXmlStreamAttribute a;
            attrs = reader.attributes();
            foreach (a, attrs) {
                ret.append(a.name().toString() + ": " + a.value().toString());
                ret.append("\n");
            }
            ret.append("\n");
        }
    }

    if(reader.hasError()) {
        qDebug() << reader.errorString();
    }
    return ret;

}

QString SipEvtThr::_fmtMsg(QString msg) {
    QString ret;
    ret.append("[TIME: ");
    ret.append(QTime::currentTime().toString("hh:mm:ss]"));
    ret.append("   ");
    ret.append(msg);
    return ret;
}
