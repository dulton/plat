#include "platmainw.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/icons/icons/app_icon.png"));
    PlatMainW w;
    w.show();

    return a.exec();
}
