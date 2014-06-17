#ifndef PLATMAINW_H
#define PLATMAINW_H

#include "videoview.h"
#include "settings.h"
#include "sipevtthr.h"
#include <QMainWindow>
#include <QFile>
#include <QThread>

namespace Ui {
class PlatMainW;
}

class PlatMainW : public QMainWindow
{
    Q_OBJECT

public:
    explicit PlatMainW(QWidget *parent = 0);
    ~PlatMainW();
private slots:
    void on_btn_invate_clicked();

    void on_btn_stop_clicked();

    void evtLoopErr(QString err);
    void evtLoopInfo(QString info);
    void evtLoopWarn(QString warn);
    void evtLoopSucc(QString succ);
    void startRecvRtp();
    void updateResDisp(QString s);
private:
    void _extUISetUp();
    void _extDataSetUp();
    void _initCfg();
    void _initSipEvtListener();
    int _initExosip();
private:
    Ui::PlatMainW *ui;
    VideoView *_videoview;
    QFile *_sdpfile;
    Settings *_settings;
    QMap<QString, QString> _setmap;
    QThread *_evtthr;
    SipEvtThr *_evtworker;
    int _dftsip_port;
    int _dftrtp_port;
    char *_localip;
    char *_usercode;

#if defined(Q_OS_WIN)
    int _ipaddlen;
    int _usercodelen;
#endif
};

#endif // PLATMAINW_H
