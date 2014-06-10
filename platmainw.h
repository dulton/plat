#ifndef PLATMAINW_H
#define PLATMAINW_H

#include <QMainWindow>
#include "videoview.h"

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
    void extUISetUp();
private:
    Ui::PlatMainW *ui;
    VideoView *_videoview;
};

#endif // PLATMAINW_H
