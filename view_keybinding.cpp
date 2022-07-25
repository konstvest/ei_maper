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
#include "mob.h"
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

//    if (a.language() != QLocale::English)
//        QMessageBox::warning(this, "Warning", "Please set locale to english");

    switch (event->key()) {
    case Qt::Key_M:
        unloadLand();
        break;
    case Qt::Key_U:
        unloadMob("");
        break;
    case Qt::Key_I:
    {
        updateTreeLogic();
        break;
    }
    default:
    {
        m_pOp->keyPress(event);
        break;
    }
    }
}

void CView::keyReleaseEvent(QKeyEvent* event)
{
    m_pOp->keyRelease(event);
}
