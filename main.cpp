#include "mainwindow.h"
#include <QApplication>
#include "scene.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    CScene scene;
    ei::CObject obj;
    obj.loadFromFile("c:\\Users\\konstantin.bezelians\\files\\temp\\hd.fig");
    scene.addObject(&obj);

    MainWindow w;
    w.show();

    return a.exec();
}
