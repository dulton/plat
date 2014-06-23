#include "xmlmsgwriter.h"
#include <QDebug>

XmlMsgWriter::XmlMsgWriter(QString *out)
    :QXmlStreamWriter(out){
    _str = out;
}

XmlMsgWriter::~XmlMsgWriter() {

}

void XmlMsgWriter::write_SIP_Start(const QString &evt_type) {
    writeStartDocument("1.0");
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
}

void XmlMsgWriter::_addNewLine() {

}
