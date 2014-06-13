#ifndef SIPEVTTHR_H
#define SIPEVTTHR_H

#include <QObject>
#include <eXosip2/eXosip.h>
#include <QtNetwork/QHostAddress>
#include <QMap>
#include <QString>

class SipEvtThr : public QObject
{
    Q_OBJECT
public:
    explicit SipEvtThr(int sip_port, int rtp_port, char *local_ip, QObject *parent = 0);
    ~SipEvtThr();
signals:
    void finished();
    void err(QString err);
    void info(QString info);
    void warn(QString warn);
    void succ(QString succ);
public slots:
    void evtloop();
private:
    int _send_401Reg(eXosip_event_t *e, char *ipaddr, char *nonce, char *alg, char *auth_type);
    int _send_2xxAns(eXosip_event_t *e);
    int _addQuote(char *str, int len, char *out, int olen);
    QString _rmQuote(char *str);
    int _cmpRespMd5(char *resp, char *username, char *pass, char *relam, char *uri, char *method, char *nonce);
    int _chkRegInfo(char *resp, char *username, char *pass, char *relam, char *uri, char *method, char *nonce);
    void _prcsReg(eXosip_event_t *e);
    void _prcsNotify(eXosip_event_t *e);
private:
    eXosip_event *pevt;
    enum {
        MS = 200,
        S = 0
    };
    struct WData {
        const char *nonce;
        const char *alg;
        const char *auth_type;
        char *local_ip;
        int sip_port;
        int rtp_port;
    }_data;
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

#endif // SIPEVTTHR_H
