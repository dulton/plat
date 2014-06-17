#include "settings.h"
#include <QFile>
#include <QTextCodec>
#include <QDebug>
#include <QStringList>

Settings::Settings(QString filename, QObject *parent) :
    QSettings(filename, QSettings::IniFormat, parent) {
}

QMap<QString, QString> Settings::readGrp(QString grp) {
    QMap<QString, QString> retmap;
    retmap.clear();
    if(grp.length() > 0) {
        QFile file;
        file.setFileName(this->fileName());
        qDebug() << Q_FUNC_INFO <<this->fileName();
        if(!file.exists()) {
            return retmap;
        }
        if(file.open(QIODevice::ReadOnly)) {
            setIniCodec(QTextCodec::codecForName("UTF-8"));
            beginGroup(grp);
            QString item;
            QStringList items = childKeys();
            foreach (item, items) {
                retmap.insert(item, value(item, "").toString().trimmed());
            }
            endGroup();
        }
        file.close();
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
    QFile file;
    file.setFileName(this->fileName());
    if(file.open(QIODevice::ReadWrite | QIODevice::Unbuffered)) {
        setIniCodec(QTextCodec::codecForName("UTF-8"));
        /*if grp exist then force add*/
        beginGroup(grp);
        setValue(key, val);
        endGroup();
    }
    if(file.isOpen()) {
        file.close();
    }
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
    QFile file;
    file.setFileName(this->fileName());
    if(file.open(QIODevice::ReadOnly)) {
        setIniCodec(QTextCodec::codecForName("UTF-8"));
        beginGroup(grp);
        if(contains(key)) {
            return value(key).toString();
        }
        endGroup();
    }
    if(file.isOpen()) {
        file.close();
    }
    return ret;
}
