#include <QKeyEvent>
#include "view.h"
#include "key_manager.h"
#include "mob.h"

void CView::keyPressEvent(QKeyEvent* event)
{
    switch (event->key()) {
    case Qt::Key_Shift:
    case Qt::Key_W:
    case Qt::Key_Up:
    case Qt::Key_S:
    case Qt::Key_Down:
    case Qt::Key_D:
    case Qt::Key_Right:
    case Qt::Key_A:
    case Qt::Key_Left:
    case Qt::Key_E:
    case Qt::Key_Q:
        m_keyManager->press(Qt::Key(event->key()));
        break;
    }

    switch (event->key()) {
    case Qt::Key_Delete:
        delNodes();
        break;
    case Qt::Key_M:
        unloadLand();
        break;
    case Qt::Key_U:
        unloadMob();
        break;
    case Qt::Key_C:
        for(auto& mob: m_aMob)
            projectToLandscape(mob->nodeSelected());
        break;
    default:
        break;
    }
}

void CView::keyReleaseEvent(QKeyEvent* event)
{
    switch (event->key()) {
    case Qt::Key_Shift:
    case Qt::Key_W:
    case Qt::Key_Up:
    case Qt::Key_S:
    case Qt::Key_Down:
    case Qt::Key_D:
    case Qt::Key_Right:
    case Qt::Key_A:
    case Qt::Key_Left:
    case Qt::Key_E:
    case Qt::Key_Q:
        m_keyManager->release(Qt::Key(event->key()));
        break;
    }
}
