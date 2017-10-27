#include "mainwindow.h"
#include <QApplication>
#include "scene.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //CScene scene(mapName = nullptr, mobName = nullptr);

    MainWindow w;

    w.show();

    return a.exec();
}
