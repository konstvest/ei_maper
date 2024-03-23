#include <QLineEdit>

#include "operationmanager.h"
#include "node.h"
#include "view.h"
#include "landscape.h"
#include "key_manager.h"
#include "settings.h"
#include "camera.h"
#include "layout_components/connectors_ui.h"
#include "log.h"
#include "scene.h"
#include "undo.h"
#include "round_mob_form.h"

void strToOperValue(QVector3D& vec, const EOperateAxis axis, const QString& value)
{
    vec = QVector3D(0.0f, 0.0f, 0.0f);
    if(value.isEmpty())
    {
        ei::log(eLogDebug, "Operation has invalid value");
        return;
    }
    switch (axis)
    {
    case EOperateAxisX:
        vec.setX(value.toFloat());
        break;
    case EOperateAxisY:
        vec.setY(value.toFloat());
        break;
    case EOperateAxisZ:
        vec.setZ(value.toFloat());
        break;
    case EOperateAxisXY:
    {
        vec.setX(value.toFloat());
        vec.setY(value.toFloat());
        break;
    }
    case EOperateAxisXZ:
    {
        vec.setX(value.toFloat());
        vec.setY(value.toFloat());
        break;
    }
    case EOperateAxisYZ:
    {
        vec.setY(value.toFloat());
        vec.setZ(value.toFloat());
        break;
    }
    }
}

void CSelect::keyPress(COperation *pOp, QKeyEvent *pEvent)
{
    switch (pEvent->nativeVirtualKey()) {
    case eKey_G:
    {
        pOp->setCurrent(new CMoveAxis(m_pView, EOperateAxisXY));
        delete this;
        break;
    }
    case eKey_R:
    {
        pOp->setCurrent(new CRotateAxis(m_pView, EOperateAxisZ));
        delete this;
        break;
    }
    case eKey_T:
    {
        if (pOp->keyManager()->isPressed(eKey_leftCtrl))
        {
            m_pView->iterateRoundMob();
        }
        else
        {
            pOp->setCurrent(new CScaleAxis(m_pView, EOperateAxisZ));
            delete this;
        }
        break;
    }
    case eKey_Delete:
    {
        m_pView->deleteSelectedNodes();
        //pOp->setCurrent(new CScaleAxis(m_pView, EOperateAxisZ));
        break;
    }
    case eKey_C:
    {
        if (pOp->keyManager()->isPressed(eKey_leftCtrl))
            m_pView->selectedObjectToClipboardBuffer();
        break;
    }
    case eKey_V:
    {
        if (pOp->keyManager()->isPressed(eKey_leftCtrl))
            m_pView->clipboradObjectsToScene();
        break;
    }
    case eKey_H:
    {
        if (pOp->keyManager()->isPressed(eKey_Alt))
            m_pView->unHideAll();
        else
            m_pView->hideSelectedNodes();

        break;
    }
    case eKey_P:
    {
        if(CScene::getInstance()->getMode() == eEditModeLogic)
            m_pView->addLogicPoint(false);
        break;
    }
    case eKey_L:
    {
        if(CScene::getInstance()->getMode() == eEditModeLogic)
            m_pView->addLogicPoint(true);
        break;
    }
    case  eKey_U:
    {
        m_pView->execUnloadCommand();
        break;
    }
    case eKey_X:
    {
        if (pOp->keyManager()->isPressed(eKey_leftCtrl))
        {
            m_pView->selectedObjectToClipboardBuffer();
            m_pView->deleteSelectedNodes();
        }
        break;
    }
    case eKey_NumDot:
    {
        m_pView->moveCamToSelectedObjects();
        break;
    }
    default:
        pOp->keyManager()->press(EKeyCode(pEvent->nativeVirtualKey()));
        break;
    }
}

void CSelect::keyRelease(COperation *pOp, QKeyEvent *pEvent)
{
    pOp->keyManager()->release(EKeyCode(pEvent->nativeVirtualKey()));
    switch (pEvent->nativeVirtualKey()) {
    case eKey_leftCtrl:
    { //todo: optimize logic for applying active mob selection
        m_pView->applyRoundMob();
        break;
    }
    default:
        break;
    }
}

void CSelect::mousePressEvent(COperation *pOp, QMouseEvent *pEvent)
{
    switch (pEvent->buttons()) {
    case Qt::LeftButton:
    {
        pOp->keyManager()->press(eKey_special1);
//        m_pView->pickObject(event->pos(), pOp->keyManager()->isPressed(eKey_Shift));
//        m_pView->viewParameters();
        break;
    }

    }
    m_lastPos = pEvent->pos();
}

void CSelect::mouseReleaseEvent(COperation *pOp, QMouseEvent *pEvent)
{
    switch (pEvent->button()) {
    case Qt::LeftButton:
    {
        pOp->keyManager()->release(eKey_special1);
        const QPoint topLeft(m_lastPos.x() < pEvent->pos().x() ? m_lastPos.x() : pEvent->pos().x(), m_lastPos.y() > pEvent->pos().y() ? m_lastPos.y() : pEvent->pos().y());
        const QPoint bottomRight(m_lastPos.x() > pEvent->pos().x() ? m_lastPos.x() : pEvent->pos().x(), m_lastPos.y() < pEvent->pos().y() ? m_lastPos.y() : pEvent->pos().y());
        QRect rect(topLeft, bottomRight);
        m_pView->pickObject(rect, pOp->keyManager()->isPressed(eKey_Shift));
        m_pView->viewParameters();

        break;
    }
    default: //TODO
    {
        int d(777);
        Q_UNUSED(d);
    }
        break;
    }
    m_lastPos = pEvent->pos();
}

void CSelect::mouseMoveEvent(COperation *pOp, QMouseEvent *pEvent)
{
    const int dx = pEvent->x() - m_lastPos.x();
    const int dy = pEvent->y() - m_lastPos.y();

    if (pEvent->buttons() & Qt::MiddleButton)
    {
        // rotate around pivot
        auto pOpt = dynamic_cast<COptInt*>(m_pView->settings()->opt(eOptSetGeneral, "mouseSenseY"));
        Q_ASSERT(pOpt);
        float coef = 12.0f-pOpt->value()/10.0f;
        float angle = float(dy)/coef;
        pOp->camera()->xRotate(angle);
        pOpt = dynamic_cast<COptInt*>(m_pView->settings()->opt(eOptSetGeneral, "mouseSenseX"));
        Q_ASSERT(pOpt);
        coef = 12.0f-pOpt->value()/10.0f;
        angle = float(dx)/coef;
        pOp->camera()->zRotate(angle);
        m_lastPos = pEvent->pos();
    }
    else if (pEvent->buttons() & Qt::LeftButton)
    {
        //todo. QRubberBand
        QPoint leftTop (m_lastPos.x() < pEvent->pos().x() ? m_lastPos.x() : pEvent->pos().x(), m_lastPos.y() > pEvent->pos().y() ? m_lastPos.y() : pEvent->pos().y());
        QPoint leftBottom(m_lastPos.x() < pEvent->pos().x() ? m_lastPos.x() : pEvent->pos().x(), m_lastPos.y() < pEvent->pos().y() ? m_lastPos.y() : pEvent->pos().y());
        QPoint rightButtom (m_lastPos.x() > pEvent->pos().x() ? m_lastPos.x() : pEvent->pos().x(), m_lastPos.y() < pEvent->pos().y() ? m_lastPos.y() : pEvent->pos().y());
        QPoint rightTop(m_lastPos.x() > pEvent->pos().x() ? m_lastPos.x() : pEvent->pos().x(), m_lastPos.y() > pEvent->pos().y() ? m_lastPos.y() : pEvent->pos().y());
        //QRect rect(leftTop, rightButtom);
        QRect rect(leftBottom, rightTop);
        m_pView->drawSelectFrame(rect);

    }
    QVector3D mouseCoord (m_pView->getLandPos(pEvent->x(), pEvent->y()));
    QString text = "not defined";
    if(mouseCoord.z() > -0.1f && mouseCoord.z() < 100.0f)
        text = util::makeString(mouseCoord, true);
    pOp->updateMouseCoords(text);
}

CMoveAxis::CMoveAxis(CView *pView, EOperateAxis ax)
    :CState(pView)
{
    qDebug()<< "CMoveAxis init with: " << ax;
    axis = ax;
    m_lastPos = m_pView->mapFromGlobal(QCursor::pos());
    lastLandPos = m_pView->getLandPos(m_lastPos.x(), m_lastPos.y());
    pView->operationSetBackup(EOperationAxisType::eMove);
    CStatusConnector::getInstance()->updateStatus("move.ico", "shift+Z(x,y) = exclude Z(x,y,) axis, X(y,z) - move X(y,z), Esc - Cancel, LMB - apply");
    CButtonConnector::getInstance()->pressButton(EButtonOpMove);
}

void CMoveAxis::keyPress(COperation *pOp, QKeyEvent *pEvent)
{
    switch (pEvent->nativeVirtualKey()) {
    case eKey_Esc:
    {
        qDebug() << "exit moveAxis operation";
        m_pView->operationRevert(EOperationAxisType::eMove);
        pOp->setCurrent(new CSelect(m_pView));
        delete this;
        break;
    }
    case eKey_X:
    {
        m_pView->operationRevert(EOperationAxisType::eMove);
        if(pOp->keyManager()->isPressed(eKey_Shift))
            pOp->setCurrent(new CMoveAxis(m_pView, EOperateAxisYZ));
        else
            pOp->setCurrent(new CMoveAxis(m_pView, EOperateAxisX));
        delete this;
        break;
    }
    case eKey_Y:
    {
        m_pView->operationRevert(EOperationAxisType::eMove);
        if(pOp->keyManager()->isPressed(eKey_Shift))
            pOp->setCurrent(new CMoveAxis(m_pView, EOperateAxisXZ));
        else
            pOp->setCurrent(new CMoveAxis(m_pView, EOperateAxisY));
        delete this;
        break;
    }
    case eKey_Z:
    {
        m_pView->operationRevert(EOperationAxisType::eMove);
        if(pOp->keyManager()->isPressed(eKey_Shift))
            pOp->setCurrent(new CMoveAxis(m_pView, EOperateAxisXY));
        else
            pOp->setCurrent(new CMoveAxis(m_pView, EOperateAxisZ));
        delete this;
        break;
    }
    case eKey_NumMinus:
    case eKey_NumDot:
    {
        value += pEvent->text();
        qDebug() << value << endl;
        break;
    }
    case eKey_0 ... eKey_9:
    {
        //todo: show value in ui
        value += pEvent->text();
        qDebug() << value << endl;
        m_pView->operationRevert(EOperationAxisType::eMove);
        QVector3D dir(0.0f, 0.0f, 0.0f);
        strToOperValue(dir, axis, value);
        m_pView->moveTo(dir);
        break;
    }
    case eKey_Backspace:
    {
        if(!value.isEmpty())
            value.remove(value.length()-1, 1);
        qDebug() << value << endl;
        if(value.isEmpty())
            break;

        m_pView->operationRevert(EOperationAxisType::eMove);
        QVector3D dir;
        strToOperValue(dir, axis, value);
        m_pView->moveTo(dir);
        break;
    }
    case eKey_Enter:
    {
        if(!value.isEmpty())
            m_pView->operationApply(EOperationAxisType::eMove);
        pOp->setCurrent(new CSelect(m_pView));
        delete this;
        break;
    }
    default:
    {
        pOp->keyManager()->press(EKeyCode(pEvent->nativeVirtualKey()));
        break;
    }
    }
}

void CMoveAxis::keyRelease(COperation *pOp, QKeyEvent *pEvent)
{
    switch (pEvent->nativeVirtualKey()) {
    default:
    {
        pOp->keyManager()->release(EKeyCode(pEvent->nativeVirtualKey()));
        break;
    }
    }
}

void CMoveAxis::axisOperate(COperation *pOp, CNode *pNode)
{
    qDebug() << "running operation move axis " + QString::number(axis) + " to " + QString::number(pNode->mapId()) + " with value: " + value;
    if(value.isEmpty() || !pNode)
    {
        qDebug() << "operation is invalid (empty)";
    }
    else
    {
        QVector3D moveDir(pNode->position());
        switch (axis) {
        case EOperateAxisX:
            moveDir.setX(moveDir.x() + value.toFloat()); break;
        case EOperateAxisY:
            moveDir.setY(moveDir.y() + value.toFloat()); break;
        case EOperateAxisZ:
            moveDir.setZ(moveDir.z() + value.toFloat()); break;
        case EOperateAxisXY:
        {
            moveDir.setX(moveDir.x() + value.toFloat());
            moveDir.setY(moveDir.y() + value.toFloat());
            break;
        }
        case EOperateAxisXZ:
        {
            moveDir.setX(moveDir.x() + value.toFloat());
            moveDir.setZ(moveDir.z() + value.toFloat());
            break;
        }
        case EOperateAxisYZ:
        {
            moveDir.setY(moveDir.y() + value.toFloat());
            moveDir.setZ(moveDir.z() + value.toFloat());
            break;
        }
        }

        pNode->updatePos(moveDir);
    }
    m_pView->operationApply(EOperationAxisType::eMove);
    pOp->setCurrent(new CSelect(m_pView));
    delete this;
}

void CMoveAxis::mousePressEvent(COperation *pOp, QMouseEvent *pEvent)
{
    switch (pEvent->button()) {
    case Qt::LeftButton:
    {
        Q_UNUSED(pOp);
        break;
    }
    default:
    {
        int d(777);
        Q_UNUSED(d);
        break;
    }
    }
}

void CMoveAxis::mouseReleaseEvent(COperation *pOp, QMouseEvent *pEvent)
{
    switch (pEvent->button()) {
    case Qt::LeftButton:
    {
        m_pView->operationApply(EOperationAxisType::eMove);
        pOp->setCurrent(new CSelect(m_pView));
        delete this;
        break;
    }
    default:
    {
        int d(777);
        Q_UNUSED(d);
        break;
    }
    }

}

void CMoveAxis::mouseMoveEvent(COperation *pOp, QMouseEvent *pEvent)
{
    Q_UNUSED(pOp);
    QVector3D newPos = m_pView->getLandPos(pEvent->x(), pEvent->y());
    QVector3D moveDir = newPos - lastLandPos;
    switch (axis) {
    case EOperateAxisX:
    {
        moveDir.setY(0.0f);
        moveDir.setZ(0.0f);
        break;
    }
    case EOperateAxisY:
    {
        moveDir.setX(0.0f);
        moveDir.setZ(0.0f);
        break;
    }
    case EOperateAxisZ:
    {
        //double trueLength = sqrt(pow(x(), 2) + pow(y(), 2));
        //QLineF line(QLine (event->pos(), m_lastPos));
        float dif = (m_lastPos.y() - pEvent->y())/50.0f;
        //moveDir = QVector3D(0.0f, 0.0f, float(line.length())/50.0f);
        moveDir = QVector3D(0.0f, 0.0f, dif);
        m_lastPos = pEvent->pos();
        break;
    }
    case EOperateAxisXY:
        moveDir.setZ(0.0f); break;
    case EOperateAxisXZ:
        moveDir.setY(0.0f); break;
    case EOperateAxisYZ:
        moveDir.setX(0.0f); break;
    }
    m_pView->moveTo(moveDir);
    //m_pView->viewParameters();
    lastLandPos = newPos;
}

CRotateAxis::CRotateAxis(CView *pView, EOperateAxis ax)
    :CState(pView)
{
    qDebug()<< "CRotateAxis init with: " << ax;
    axis = ax;
    m_lastPos = m_pView->mapFromGlobal(QCursor::pos());
    pView->operationSetBackup(EOperationAxisType::eRotate);
    CStatusConnector::getInstance()->updateStatus("rotate.ico", "Z(x,y) - rotate Z(x,y). Esc - Cancel, LMB - apply");
    CButtonConnector::getInstance()->pressButton(EButtonOpRotate);
}


void CRotateAxis::keyPress(COperation *pOp, QKeyEvent *pEvent)
{
    switch (pEvent->nativeVirtualKey()) {
    case eKey_Esc:
    {
        qDebug() << "exit CRotateAxis operation";
        m_pView->operationRevert(EOperationAxisType::eRotate);
        pOp->setCurrent(new CSelect(m_pView));
        delete this;
        break;
    }
    case eKey_NumMinus:
    case eKey_NumDot:
    {
        value += pEvent->text();
        qDebug() << value << endl;
        break;
    }
    case eKey_0 ... eKey_9:
    {
        value += pEvent->text();
        qDebug() << value << endl;
        m_pView->operationRevert(EOperationAxisType::eRotate);
        QVector3D rot(0.0f, 0.0f, 0.0f);
        strToOperValue(rot, axis, value);
        m_pView->rotateTo(rot);
        break;
    }
    case eKey_Backspace:
    {
        if(!value.isEmpty())
            value.remove(value.length()-1, 1);
        qDebug() << value << endl;
        if(value.isEmpty())
            break;

        m_pView->operationRevert(EOperationAxisType::eRotate);
        QVector3D rot(0.0f, 0.0f, 0.0f);
        strToOperValue(rot, axis, value);
        m_pView->rotateTo(rot);
        break;
    }
    case eKey_Enter:
    {
        if(!value.isEmpty())
            m_pView->operationApply(EOperationAxisType::eRotate);
        pOp->setCurrent(new CSelect(m_pView));
        delete this;
        break;
    }
    case eKey_X:
    {
        m_pView->operationRevert(EOperationAxisType::eRotate);
        pOp->setCurrent(new CRotateAxis(m_pView, EOperateAxisX));
        delete this;
        break;
    }
    case eKey_Y:
    {
        m_pView->operationRevert(EOperationAxisType::eRotate);
        pOp->setCurrent(new CRotateAxis(m_pView, EOperateAxisY));
        delete this;
        break;
    }
    case eKey_Z:
    {
        m_pView->operationRevert(EOperationAxisType::eRotate);
        pOp->setCurrent(new CRotateAxis(m_pView, EOperateAxisZ));
        delete this;
        break;
    }
    default:
    {
        pOp->keyManager()->press(EKeyCode(pEvent->nativeVirtualKey()));
        break;
    }
    }
}

void CRotateAxis::keyRelease(COperation *pOp, QKeyEvent *pEvent)
{
    switch (pEvent->nativeVirtualKey()) {
    default:
    {
        pOp->keyManager()->release(EKeyCode(pEvent->nativeVirtualKey()));
        break;
    }
    }
}

void CRotateAxis::mousePressEvent(COperation *pOp, QMouseEvent *pEvent)
{
    switch (pEvent->buttons()) {
    case Qt::LeftButton:
    {
        Q_UNUSED(pOp);
        break;
    }
    default:
    {
        int d(777);
        Q_UNUSED(d);
        break;
    }
    }
}

void CRotateAxis::mouseReleaseEvent(COperation *pOp, QMouseEvent *pEvent)
{
    switch (pEvent->button()) {
    case Qt::LeftButton:
    {
        m_pView->operationApply(EOperationAxisType::eRotate);
        pOp->setCurrent(new CSelect(m_pView));
        delete this;
        break;
    }
    default:
    {
        int d(777);
        Q_UNUSED(d);
        break;
    }
    }
}

void CRotateAxis::mouseMoveEvent(COperation *pOp, QMouseEvent *pEvent)
{
    Q_UNUSED(pOp);
    float dif = (pEvent->x() - m_lastPos.x())/10.0f;
    QVector3D rot(0.0f, 0.0f, 0.0f); //euler angles for add rotation;
    switch (axis) {
    case EOperateAxisX:
        rot.setX(dif); break;
    case EOperateAxisY:
        rot.setY(dif); break;
    case EOperateAxisZ:
        rot.setZ(dif); break;
    case EOperateAxisXY:
    case EOperateAxisXZ:
    case EOperateAxisYZ:
    {
        qDebug() << "not defined";
    }
    }
    m_pView->rotateTo(rot);
    //m_pView->viewParameters();
    m_lastPos = pEvent->pos();
}

void CRotateAxis::axisOperate(COperation *pOp, CNode *pNode)
{
    qDebug() << "running operation rotate axis " + QString::number(axis) + " to " + QString::number(pNode->mapId()) + " with value: " + value;
    if(value.isEmpty() || !pNode)
    {
        qDebug() << "operation is invalid (empty)";
    }
    else
    {
        const float gimbalAvoidStep = 0.001f;
        QVector3D rotation(pNode->getEulerRotation());
        QQuaternion quat;
        QVector3D eulerRot;
        const auto setIsNan = [&quat, &eulerRot](QVector3D& rot)
        {
            quat = QQuaternion::fromEulerAngles(rot);
            eulerRot = quat.toEulerAngles();
            if(eulerRot.x() == eulerRot.x() && eulerRot.y() == eulerRot.y() && eulerRot.z() == eulerRot.z())
                return false;
            return true;
        };

        switch (axis) {
        case EOperateAxisX:
        {
            rotation.setX(rotation.x() + value.toFloat());
            for(int i(0); i<100; ++i) //avoid gimbal lock
                if (setIsNan(rotation))
                    rotation.setX(rotation.x() + gimbalAvoidStep);
                else
                    break;
            break;
        }
        case EOperateAxisY:
        {
            rotation.setY(rotation.y() + value.toFloat());
            for(int i(0); i<100; ++i) //avoid gimbal lock
                if (setIsNan(rotation))
                    rotation.setY(rotation.y() + gimbalAvoidStep);
                else
                    break;
            break;
        }
        case EOperateAxisZ:
        {
            rotation.setZ(rotation.z() + value.toFloat());
            for(int i(0); i<100; ++i) //avoid gimbal lock
                if (setIsNan(rotation))
                    rotation.setZ(rotation.z() + gimbalAvoidStep);
                else
                    break;
            break;
        }
        case EOperateAxisXY:
        {
            rotation.setX(rotation.x() + value.toFloat());
            rotation.setY(rotation.y() + value.toFloat());
            for(int i(0); i<200; ++i) //avoid gimbal lock
            {
                if (!setIsNan(rotation))
                    break;

                if (i%2 == 0)
                    rotation.setX(rotation.x() + gimbalAvoidStep);
                else
                    rotation.setY(rotation.y() + gimbalAvoidStep);
            }
            break;
        }
        case EOperateAxisXZ:
        {
            rotation.setX(rotation.x() + value.toFloat());
            rotation.setZ(rotation.z() + value.toFloat());
            for(int i(0); i<200; ++i) //avoid gimbal lock
            {
                if (!setIsNan(rotation))
                    break;

                if (i%2 == 0)
                    rotation.setX(rotation.x() + gimbalAvoidStep);
                else
                    rotation.setZ(rotation.z() + gimbalAvoidStep);
            }
            break;
        }
        case EOperateAxisYZ:
        {
            rotation.setY(rotation.y() + value.toFloat());
            rotation.setZ(rotation.z() + value.toFloat());
            for(int i(0); i<200; ++i) //avoid gimbal lock
            {
                if (!setIsNan(rotation))
                    break;

                if (i%2 == 0)
                    rotation.setY(rotation.y() + gimbalAvoidStep);
                else
                    rotation.setZ(rotation.z() + gimbalAvoidStep);
            }
            break;
        }
        }
        pNode->setRot(quat);
        m_pView->viewParameters();
    }
    m_pView->operationApply(EOperationAxisType::eRotate);
    pOp->setCurrent(new CSelect(m_pView));
    delete this;
}

CScaleAxis::CScaleAxis(CView *pView, EOperateAxis ax)
    :CState(pView)
{
    qDebug()<< "CScaleAxis init with: " << ax;
    axis = ax;
    m_lastPos = m_pView->mapFromGlobal(QCursor::pos());
    pView->operationSetBackup(EOperationAxisType::eScale);
    CStatusConnector::getInstance()->updateStatus("scale.ico", "X(y,z) - scale X(y,z), shift+Z(x,y) = exclude Z(x,y,) axis. Esc - Cancel, LMB - apply");
    CButtonConnector::getInstance()->pressButton(EButtonOpScale);
}


void CScaleAxis::keyPress(COperation *pOp, QKeyEvent *pEvent)
{
    switch (pEvent->nativeVirtualKey()) {
    case eKey_Esc:
    {
        qDebug() << "exit CScaleAxis operation";
        m_pView->operationRevert(EOperationAxisType::eScale);
        pOp->setCurrent(new CSelect(m_pView));
        delete this;
        break;
    }
    case eKey_NumMinus:
    case eKey_NumDot:
    {
        value += pEvent->text();
        qDebug() << value << endl;
        break;
    }
    case eKey_0 ... eKey_9:
    {
        value += pEvent->text();
        qDebug() << value << endl;
        m_pView->operationRevert(EOperationAxisType::eScale);
        QVector3D scale(0.0f, 0.0f, 0.0f);
        strToOperValue(scale, axis, value);
        m_pView->scaleTo(scale);
        break;
    }
    case eKey_Backspace:
    {
        if(!value.isEmpty())
            value.remove(value.length()-1, 1);
        qDebug() << value << endl;
        if(value.isEmpty())
            break;

        m_pView->operationRevert(EOperationAxisType::eScale);
        QVector3D scale(0.0f, 0.0f, 0.0f);
        strToOperValue(scale, axis, value);
        m_pView->scaleTo(scale);
        break;
    }
    case eKey_Enter:
    {
        if(!value.isEmpty())
            m_pView->operationApply(EOperationAxisType::eScale);
        pOp->setCurrent(new CSelect(m_pView));
        delete this;
        break;
    }
    case eKey_X:
    {
        m_pView->operationRevert(EOperationAxisType::eScale);
        pOp->setCurrent(new CScaleAxis(m_pView, EOperateAxisX));
        delete this;
        break;
    }
    case eKey_Y:
    {
        m_pView->operationRevert(EOperationAxisType::eScale);
        pOp->setCurrent(new CScaleAxis(m_pView, EOperateAxisY));
        delete this;
        break;
    }
    case eKey_Z:
    {
        m_pView->operationRevert(EOperationAxisType::eScale);
        pOp->setCurrent(new CScaleAxis(m_pView, EOperateAxisZ));
        delete this;
        break;
    }
    default:
    {
        pOp->keyManager()->press(EKeyCode(pEvent->nativeVirtualKey()));
        break;
    }
    }
}

void CScaleAxis::keyRelease(COperation *pOp, QKeyEvent *pEvent)
{
    switch (pEvent->nativeVirtualKey()) {
    default:
    {
        pOp->keyManager()->release(EKeyCode(pEvent->nativeVirtualKey()));
        break;
    }
    }
}

void CScaleAxis::axisOperate(COperation *pOp, CNode *pNode)
{
    qDebug() << "running operation scale axis " + QString::number(axis) + " to " + QString::number(pNode->mapId()) + " with value: " + value;
    if(value.isEmpty() || !pNode)
    {
        qDebug() << "operation is invalid (empty)";
    }
    else
    {

        QVector3D scale(pNode->complection());
        switch (axis) {
        case EOperateAxisX:
            scale.setX(scale.x() + value.toFloat()); break;
        case EOperateAxisY:
            scale.setY(scale.y() + value.toFloat()); break;
        case EOperateAxisZ:
            scale.setZ(scale.z() + value.toFloat()); break;
        case EOperateAxisXY:
        {
            scale.setX(scale.x() + value.toFloat());
            scale.setY(scale.y() + value.toFloat());
            break;
        }
        case EOperateAxisXZ:
        {
            scale.setX(scale.x() + value.toFloat());
            scale.setZ(scale.z() + value.toFloat());
            break;
        }
        case EOperateAxisYZ:
        {
            scale.setY(scale.y() + value.toFloat());
            scale.setZ(scale.z() + value.toFloat());
            break;
        }
        }

        pNode->setConstitution(scale);
        m_pView->viewParameters();
    }
    m_pView->operationApply(EOperationAxisType::eScale);
    pOp->setCurrent(new CSelect(m_pView));
    delete this;
}

void CScaleAxis::mousePressEvent(COperation *pOp, QMouseEvent *pEvent)
{
    switch (pEvent->buttons()) {
    case Qt::LeftButton:
    {
        Q_UNUSED(pOp);
        break;
    }
    default:
        break;
    }

}

void CScaleAxis::mouseReleaseEvent(COperation *pOp, QMouseEvent *pEvent)
{
    switch (pEvent->button()) {
    case Qt::LeftButton:
    {
        m_pView->operationApply(EOperationAxisType::eScale);
        pOp->setCurrent(new CSelect(m_pView));
        delete this;
        break;
    }
    default:
        break;
    }
}

void CScaleAxis::mouseMoveEvent(COperation *pOp, QMouseEvent *pEvent)
{
    Q_UNUSED(pOp);
    float dif = (pEvent->x() - m_lastPos.x())/200.0f;
    QVector3D scale(0.0f, 0.0f, 0.0f); //euler angles for add rotation;
    switch (axis) {
    case EOperateAxisX:
        scale.setX(dif); break;
    case EOperateAxisY:
        scale.setY(dif); break;
    case EOperateAxisZ:
        scale.setZ(dif); break;
    case EOperateAxisXY:
    case EOperateAxisXZ:
    case EOperateAxisYZ:
    {
        qDebug() << "not defined";
    }
    }
    m_pView->scaleTo(scale);
    m_lastPos = pEvent->pos();
}

COperation::COperation(CState *s)
    : current(s)
{
    //create key manager
    m_keyManager.reset(new CKeyManager());
}

void COperation::changeState(CState *state)
{
    if(nullptr!= current)
        delete current;
    current = state;
}

void COperation::mousePressEvent(QMouseEvent *pEvent)
{
    current->mousePressEvent(this, pEvent);
}

void COperation::mouseReleaseEvent(QMouseEvent *pEvent)
{
    current->mouseReleaseEvent(this, pEvent);
}

void COperation::mouseMoveEvent(QMouseEvent *pEvent)
{
    current->mouseMoveEvent(this, pEvent);
}

void COperation::attachCam(CCamera *pCam)
{
    m_pCam = pCam;
}

void COperation::updateMouseCoords(const QString& pos)
{
    m_pMouseCoord->clear();
    m_pMouseCoord->setText(pos);
}

CSelect::CSelect(CView *pView):
    CState(pView)
{
    qDebug()<< "CSelect init";
    if(CScene::getInstance()->getMode()==eEditModeLogic)
        CStatusConnector::getInstance()->updateStatus("select.ico", "LMB-Select object, Shift+LMB-Add to select, MMB-camera rotation, G-Move, P-Add Patrol(trap zone), L-Add look(cast point), CTLR+Tab-change mode");
    else
        CStatusConnector::getInstance()->updateStatus("select.ico", "LMB-Select object, Shift+LMB-Add to select, MMB-camera rotation, G-Move, T-Scale, R-Rotate, CTLR+Tab-change mode, CTRL+T-switch active Mob");
    CButtonConnector::getInstance()->pressButton(EButtonOpSelect);
}
