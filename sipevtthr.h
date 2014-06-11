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
public slots:
    void evtloop();
private:
    //int _exosipInit();
    int _send_401Reg(eXosip_event_t *e, char *ipaddr, char *nonce, char *alg, char *auth_type);
    int _addQuote(char *str, int len, char *out, int olen);
    int _cmpRespMd5(const char *org);
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

#endif // SIPEVTTHR_H
