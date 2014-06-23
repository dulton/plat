#include "ptzinfo.h"
#include <QDebug>
#include "xmlmsgwriter.h"

PtzInfo::PtzInfo(QString camcode, QObject *parent)
    :QObject(parent) {
    _camcode= camcode;
    _ptzcmd = PTZ_CMD_NONE;
    _sp1 = SPEED_NONE;
    _sp2 = SPEED_NONE;
}

PtzInfo::~PtzInfo() {

}

void PtzInfo::setPtzcmd(const PTZ_CMD &ptzcmd) {
    _ptzcmd = ptzcmd;
}

PTZ_CMD PtzInfo::Ptzcmd() const {
    return _ptzcmd;
}

void PtzInfo::setPara1(const SPEED &sp1) {
    _sp1 = sp1;
}

void PtzInfo::setPara2(const SPEED &sp2) {
    _sp2 = sp2;
}

SPEED PtzInfo::Para1() const {
    return _sp1;
}

SPEED PtzInfo::Para2() const {
    return _sp2;
}

void PtzInfo::setPrePara(const int &para) {
    _prepara = para;
}

int PtzInfo::PrePara() const {
    return _prepara;
}

void PtzInfo::setCamCode(const QString &devcode) {
    _camcode = devcode;
}

QString PtzInfo::CamCode() const {
    return _camcode;
}

QString PtzInfo::getXmlMsg() const {
    QString str;
    if(_camcode.isEmpty() || _ptzcmd == PTZ_CMD_NONE ||
       _sp1 == SPEED_NONE || _sp2 == SPEED_NONE) {
        return str;
    }
    XmlMsgWriter write(&str);
    write.write_SIP_Start("Control_Camera");
    write.write_PtzItem(_camcode,
                        QString::number(_ptzcmd, 16),
                        QString::number(_sp1, 16),
                        QString::number(_sp2, 16));
    write.write_SIP_End();
    return str;
}
