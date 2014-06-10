#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QMap>

class Settings : public QSettings
{
    Q_OBJECT
public:
    explicit Settings(QString filename, QObject *parent = 0);
    QMap <QString, QString> readGrp(QString grp);

signals:

public slots:

private:
};

#endif // SETTINGS_H
