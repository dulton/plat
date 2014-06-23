#include "ptzinfo.h"
#include <QXmlStreamWriter>
#include <QXmlStreamAttribute>
#include <QXmlStreamAttributes>
#include <QBuffer>
#include <QTextCodec>

PtzInfo::PtzInfo(QString devcode, QObject *parent)
    :QObject(parent) {
    _devcode = devcode;
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

void PtzInfo::setDevCode(const QString &devcode) {
    _devcode = devcode;
}

QString PtzInfo::DevCode() const {
    return _devcode;
}

QString PtzInfo::getXmlMsg() {
    QXmlStreamWriter writer;
    QBuffer buf;
    writer.setDevice(&buf);
    writer.setCodec(QTextCodec::codecForName("UTF-8"));
    writer.writeStartDocument("1.0");
    writer.writeEndDocument();
}
