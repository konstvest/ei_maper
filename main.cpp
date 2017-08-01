//#include <QtGui/QApplication>
#include <QtGui>
#include <QApplication>
#include "mainwindow.h"
#include "mainscene.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainScene scene;

    scene.resize(600, 600);
    scene.show();
    
    return a.exec();
}
