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
    m_pOp->keyPress(EKeyCode(event->nativeVirtualKey()));
}

void CView::keyReleaseEvent(QKeyEvent* event)
{
    m_pOp->keyRelease(EKeyCode(event->nativeVirtualKey()));
}
