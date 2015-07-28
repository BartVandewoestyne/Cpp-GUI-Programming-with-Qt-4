#include <QApplication>

#include "mainwindow.h"

// SINGLE FILE VERSION
//int main(int argc, char *argv[])
//{
//    QApplication app(argc, argv);
//    MainWindow mainWin;
//    mainWin.show();
//    return app.exec();
//}

// MULTIPLE FILE VERSION
//int main(int argc, char *argv[])
//{
//    QApplication app(argc, argv);
//    MainWindow *mainWin = new MainWindow;
//    mainWin->show();
//    return app.exec();
//}

// SPLASH SCREEN VERSION
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QSplashScreen *splash = new QSplashScreen;
    splash->setPixmap(QPixmap(":/images/splash.png"));
    splash->show();

    Qt::Alignment topRight = Qt::AlignRight | Qt::AlignTop;
    splash->showMessage(QObject::tr("Setting up the main window..."),
                           topRight, Qt::white);
    MainWindow mainWin;

    splash->showMessage(QObject::tr("Loading modules..."),
                           topRight, Qt::white);
    loadModules();

    splash->showMessage(QObject::tr("Establishing connections..."),
                           topRight, Qt::white);
    establishConnections();

    mainWin.show();
    splash->finish(&mainWin);
    delete splash;

    return app.exec();
}
