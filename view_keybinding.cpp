#include <QApplication>
#include <QMessageBox>
#include <QKeyEvent>
#include <QDir>

#include <QClipboard>
#include <QGuiApplication>
#include <QJsonDocument>
#include <QJsonArray>

#include "view.h"
#include "key_manager.h"
#include "mob\mob.h"
#include "camera.h"
#include "landscape.h"
#include "operationmanager.h"
#include "scene.h"

void CView::keyPressEvent(QKeyEvent* event)
{
    QApplication* application = static_cast<QApplication *>(QApplication::instance());
    application->inputMethod()->reset();
    QLocale a = application->inputMethod()->locale();
    QString lang = a.languageToString(a.language());
    //a.setDefault(QLocale::English);

//    if (a.language() != QLocale::English)
//        QMessageBox::warning(this, "Warning", "Please set locale to english");

    switch (event->nativeVirtualKey()) {
    case eKey_M:
//        unloadLand();
        break;
//    case eKey_U:
//        unloadMob("");
//        break;
    case eKey_I:
    {
        break;
    }
    default:
    {
        m_pOp->keyPress(event);
        break;
    }
    }
    qDebug() << QString(event->nativeVirtualKey()) << ":" << event->nativeVirtualKey();
}

void CView::keyReleaseEvent(QKeyEvent* event)
{
    m_pOp->keyRelease(event);
}
