#ifndef SIPEVTTHR_H
#define SIPEVTTHR_H

#include <QObject>
#include <eXosip2/eXosip.h>
#include <QtNetwork/QHostAddress>
#include <QMap>
#include <QString>
#include "settings.h"
#include "ptzinfo.h"
#include <QMutex>

class SipEvtThr : public QObject
{
    Q_OBJECT
public:
    explicit SipEvtThr(int sip_port, int rtp_port, char *local_ip, char *user_code, QObject *parent = 0);
    ~SipEvtThr();
signals:
    void finished();
    void err(QString err);
    void info(QString info);
    void warn(QString warn);
    void succ(QString succ);
    void rtp_start();
    void update_ResDisp(QString s);
public slots:
    void evtloop();
    void send_INVITE();
    void send_BYE();
    void send_PTZ_DI_CTL(const PtzInfo &info);
private:
    int _send_401Reg(eXosip_event_t *e, char *ipaddr, char *nonce, char *alg, char *auth_type);
    int _send_AnsStatus(eXosip_event_t *e, int status);
    int _addQuote(char *str, int len, char *out, int olen);
    QString _rmQuote(char *str);
    int _cmpRespMd5(char *resp, char *username, char *pass, char *relam, char *uri, char *method, char *nonce);
    int _chkRegInfo(char *resp, char *username, char *pass, char *relam, char *uri, char *method, char *nonce);
    void _prcsReg(eXosip_event_t *e);
    void _prcsNotify(eXosip_event_t *e);
    void _prcsINVITE(eXosip_event_t *e);
    void _recContractVia(osip_contact_t *c, osip_via_t *v);
    int _chkSipContentType(osip_content_type_t *t, int type);
    QString _bdSDPMsg(char *oip, char *cip, int lport, int payload);
    QString _bdFTC(char *code, char *ip, int port);
    QString _readXmlNOTIFY(char *msg);
    QString _fmtMsg(QString msg);
    void _readUset();
    int _chkUset();
private:
    eXosip_event *pevt;
    enum {
        MS = 200,
        S = 0
    };
    enum {
        E_SDP = 0,
        E_XML
    };
    struct WData {
        const char *nonce;
        const char *alg;
        const char *auth_type;
        const char *dft_pass;
        const char *dft_sdp_type;
        const char *dft_xml_type;
        char *user_code;
        char *local_ip;
        int sip_port;
        int rtp_port;
        int rtp_playload;
    }_data;
    struct CallInfo {
        int cid;
        int did;
    }_callinfo;
    struct UsetInfo {
        QString devcode;
        QString remoteip;
        QString pass;
        int remoteport;
    }_usetinfo;
    Settings *_uset;
    QMutex mt;
};

inline int SipEvtThr::_chkRegInfo(char *resp, char *username,
                                  char *pass, char *relam, char *uri, char *method, char *nonce) {
    if(resp != NULL && username != NULL && pass != NULL &&
       relam != NULL && uri != NULL && method != NULL && nonce != NULL) {
        if(strlen(resp) > 0 && strlen(username) > 0 && strlen(pass) > 0 &&
           strlen(relam) > 0 && strlen(uri) > 0 && strlen(method) > 0 &&
           strlen(nonce) > 0) {
            return 0;
        }
    }
    return -1;
}

inline int SipEvtThr::_chkUset() {
    if(_usetinfo.devcode.isEmpty() || _usetinfo.pass.isEmpty() ||
            _usetinfo.remoteip.isEmpty() || _usetinfo.remoteport == 0) {
#if 1
        qDebug() << Q_FUNC_INFO << _usetinfo.devcode;
        qDebug() << Q_FUNC_INFO << _usetinfo.pass;
        qDebug() << Q_FUNC_INFO << _usetinfo.remoteip;
        qDebug() << Q_FUNC_INFO << _usetinfo.remoteport;
#endif
        return -1;
    }
    return 0;
}

#endif // SIPEVTTHR_H
