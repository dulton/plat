#include "settings.h"
#include <QFile>
#include <QTextCodec>
#include <QDebug>
#include <QStringList>

Settings::Settings(QString filename, QObject *parent) :
    QSettings(filename, QSettings::IniFormat, parent) {
    setIniCodec(QTextCodec::codecForName("UTF-8"));
}

QMap<QString, QString> Settings::readGrp(QString grp) {
    QMap<QString, QString> retmap;
    retmap.clear();
    if(grp.length() > 0) {
        beginGroup(grp);
        QString item;
        QStringList items = childKeys();
        foreach (item, items) {
            retmap.insert(item, value(item, "").toString().trimmed());
        }
        endGroup();
    }
    return retmap;
}

int Settings::writeGrp(QString grp, QString key, QString val) {
    if(grp.isEmpty() || key.isEmpty() || val.isEmpty()) {
        return -1;
    }
    if(this->fileName().isEmpty()) {
        return -2;
    }

    beginGroup(grp);
    setValue(key, val);
    endGroup();
    return 0;
}

QString Settings::readGKV(QString grp, QString key) {
    QString ret;
    ret.clear();
    if(this->fileName().isEmpty()) {
        return ret;
    }
    if(grp.isEmpty() || key.isEmpty()) {
        return ret;
    }

    beginGroup(grp);
    if(contains(key)) {
        qDebug() << Q_FUNC_INFO << value(key).toString();
        ret = value(key).toString();
        endGroup();
        return ret;
    }
    endGroup();
    return ret;
}
