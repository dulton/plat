#include "platmainw.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PlatMainW w;
    w.show();

    return a.exec();
}
