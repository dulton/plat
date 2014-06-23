#ifndef XMLMSGWRITER_H
#define XMLMSGWRITER_H

#include <QXmlStreamWriter>

class XmlMsgWriter : public QXmlStreamWriter
{
public:
    explicit XmlMsgWriter(QString *out);
    ~XmlMsgWriter();
    void write_SIP_Start(const QString &evt_type);
    void write_PtzItem(const QString &devcode, const QString &cmd,
                       const QString &p1, const QString &p2,
                       const QString &p3);
    void write_SIP_End();
private:
    QString *_str;
    void _addNewLine();
};

#endif // XMLMSGWRITER_H
