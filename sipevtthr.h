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
    explicit SipEvtThr(const QMap<QString, QString> &inimap, QObject *parent = 0);
    ~SipEvtThr();
signals:
    void finished();
    void err(QString err);
public slots:
    void evtloop();
private:
    int _exosipInit();
private:
    eXosip_event *pevt;
    int _dftsip_port;
    int _dftrtp_port;
    char *_localip;
    enum {
        MS = 200,
        S = 0
    };
};

#endif // SIPEVTTHR_H
