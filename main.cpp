#include "platmainw.h"
#include <QApplication>


#if 0
#ifdef DBG_MODE
void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
    case QtDebugMsg:
        fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtWarningMsg:
        fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        abort();
    }
}
#endif

#endif

int main(int argc, char *argv[])
{
#if 0
#ifdef DBG_MODE
    qInstallMessageHandler(myMessageOutput);
#endif
#endif
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/icons/icons/app_icon.png"));
    PlatMainW w;
    w.show();

    return a.exec();
}
