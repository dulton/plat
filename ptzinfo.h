#ifndef PTZINFO_H
#define PTZINFO_H

#include <QObject>
#include "ptzdefines.h"

class PtzInfo : public QObject
{
    Q_OBJECT
public:
    explicit PtzInfo(QString devcode, QObject *parent = 0);
    ~PtzInfo();
    void setPtzcmd(const PTZ_CMD &ptzcmd);
    PTZ_CMD Ptzcmd() const;
    void setPara1(const SPEED &sp1);
    void setPara2(const SPEED &sp2);
    SPEED Para1() const;
    SPEED Para2() const;
    void setPrePara(const int &para);
    int PrePara() const;
    void setDevCode(const QString &devcode);
    QString DevCode() const;
    QString getXmlMsg() const;
signals:

public slots:

private:
    PTZ_CMD _ptzcmd;
    SPEED _sp1;
    SPEED _sp2;
    int _prepara;
    QString _devcode;
};

#endif // PTZINFO_H
