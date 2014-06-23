#include "xmlmsgwriter.h"
#include <QDebug>
#include <QTextCodec>

XmlMsgWriter::XmlMsgWriter(QString *out)
    :QXmlStreamWriter(out){
    _str = out;
}

XmlMsgWriter::~XmlMsgWriter() {

}

void XmlMsgWriter::write_SIP_Start(const QString &evt_type) {
    /*this will not add attr encoding utf-8*/
    //setCodec(QTextCodec::codecForName("UTF-8"));
    writeStartDocument("1.0");
    _fixencodingstr(_str);
    writeStartElement("SIP_XML");
    if(evt_type.length() > 0) {
        writeAttribute("EventType", evt_type);
    } else {
        writeAttribute("EventType", "none");
    }
}

void XmlMsgWriter::write_PtzItem(const QString &devcode,
                                 const QString &cmd, const QString &p1,
                                 const QString &p2, const QString &p3) {
    if(devcode.isEmpty() || cmd.isEmpty() ||
            p1.isEmpty() || p2.isEmpty() || p3.isEmpty()) {
        return;
    }
    writeStartElement("Item");
    writeAttribute("Code", devcode);
    writeAttribute("Command", cmd);
    writeAttribute("CommandPara1", p1);
    writeAttribute("CommandPara2", p2);
    writeAttribute("CommandPara3", p3);
    writeEndElement();
}

void XmlMsgWriter::write_SIP_End() {
    writeEndElement();
    writeEndDocument();
    _addNewLine();
}

void XmlMsgWriter::_addNewLine() {
    QChar var;
    QList <int> posl;
    int pos = 0;
    foreach (var, *_str) {
        if(var == '>') {
            posl.push_back(pos);
        }
        pos++;
    }
    int _ioff = 0;
    foreach (pos, posl) {
        _str->insert(pos + 1 + _ioff, "\r\n");
        _ioff += strlen("\r\n");
    }
}

/*fix set encoding tag*/
void XmlMsgWriter::_fixencodingstr(QString *str) {
    QString fix_en = " encoding=\"UTF-8\"";
    int pos = str->indexOf("?>");
    if(pos == -1) {
        return;
    }
    _str->insert(pos, fix_en);
    return;
}
