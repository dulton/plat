#ifndef PLATMAINW_H
#define PLATMAINW_H

#include "videoview.h"
#include "settings.h"
#include "sipevtthr.h"
#include <QMainWindow>
#include <QFile>
#include <QThread>
#include <QPushButton>

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
    /*for ptz*/

    void on_b_right_up_pressed();
    void on_b_right_pressed();
    void on_b_right_down_pressed();
    void on_b_down_pressed();
    void on_b_left_down_pressed();
    void on_b_left_pressed();
    void on_b_left_up_pressed();
    void on_b_up_pressed();

    void on_b_right_up_released();
    void on_b_right_released();
    void on_b_right_down_released();
    void on_b_down_released();
    void on_b_left_down_released();
    void on_b_left_released();
    void on_b_left_up_released();
    void on_b_up_released();


private:
    void _extUISetUp();
    void _extDataSetUp();
    void _initCfg();
    void _initSipEvtListener();
    int _initExosip();
    void _ptz_send_cb(PTZ_CMD cmd);
    int _chk_ptz_cmd(const PTZ_CMD &cmd);
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
    char *_camcode;
    int _ptz_timeout;
    QPushButton *ptzbtns[8];

#if defined(Q_OS_WIN)
    int _ipaddlen;
    int _usercodelen;
    int _camcodelen;
#endif
};

inline int PlatMainW::_chk_ptz_cmd(const PTZ_CMD &cmd) {
    if(cmd == PTZ_UP_START || cmd == PTZ_UP_STOP ||
       cmd == PTZ_DOWN_START || cmd == PTZ_DOWN_STOP ||
       cmd == PTZ_LEFT_START || cmd == PTZ_LEFT_STOP ||
       cmd == PTZ_RIGHT_START || cmd == PTZ_RIGHT_STOP ||
       cmd == PTZ_UP_LEFT_START || cmd == PTZ_UP_LEFT_STOP ||
       cmd == PTZ_DOWN_LEFT_START || cmd == PTZ_DOWN_LEFT_STOP ||
       cmd == PTZ_UP_RIGHT_START || cmd == PTZ_UP_RIGHT_STOP ||
       cmd == PTZ_DOWN_RIGHT_START || cmd == PTZ_DOWN_RIGHT_STOP) {
        return CMD_DI;
    } else if (cmd == PTZ_LIGHT_ON || cmd == PTZ_LIGHT_OFF ||
               cmd == PTZ_WARM_ON  || cmd == PTZ_WARM_OFF  ||
               cmd == PTZ_INFRARED_ON || cmd == PTZ_INFRARED_OFF ||
               cmd == PTZ_W_WIPER_ON || cmd == PTZ_W_WIPER_OFF ||
               cmd == PTZ_LOCK || cmd == PTZ_UNLOCK) {
        return CMD_CTL;
    }
    return CMD_NONE;
    /*other will be add later*/
}

#endif // PLATMAINW_H
