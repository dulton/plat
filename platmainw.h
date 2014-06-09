#ifndef PLATMAINW_H
#define PLATMAINW_H

#include <QMainWindow>

namespace Ui {
class PlatMainW;
}

class PlatMainW : public QMainWindow
{
    Q_OBJECT

public:
    explicit PlatMainW(QWidget *parent = 0);
    ~PlatMainW();

private:
    Ui::PlatMainW *ui;
};

#endif // PLATMAINW_H
