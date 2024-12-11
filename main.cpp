#include "mainwindow.h"
#include <QApplication>
#include <QSplashScreen>

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")// 该指令仅支持VS环境
#endif

int main(int argc, char *argv[]){
    QApplication a(argc, argv);

    QSplashScreen *splash = new QSplashScreen;
    splash->setPixmap(QPixmap("Icon/recognition.png"));
    splash->show();

    Qt::Alignment topRight = Qt::AlignRight | Qt::AlignTop;
    splash->showMessage(QObject::tr("Setting up the main window..."), topRight, Qt::white);

    splash->showMessage(QObject::tr("Loading data..."), topRight, Qt::white);
    //MSystemInfo::Instance()->init();

    splash->showMessage(QObject::tr("Connecting device..."), topRight, Qt::white);
    //MSystemInfo::Instance()->open();

    MainWindow w;
    w.show();

    splash->finish(&w);
    delete splash;

    return a.exec();
}
