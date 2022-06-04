#include <QGLWidget>
#include <QtMath>
#include "camera.h"
#include "key_manager.h"
#include "settings.h"

CCamera::CCamera()
    :m_keyManager(nullptr)
{
    m_pos = QVector3D(0.0, 1.0, 10.0);
    m_pivot = QVector3D(0.0, 0.0, 0.0);
    m_xRot = -90.0f;
    m_zRot = 0.0f;
    m_step = 0.5f;
}

void CCamera::reset()
{
    COptBool* pOpt = dynamic_cast<COptBool*>(m_pSettings->opt("freeCamera"));
    Q_ASSERT(pOpt);
    if (pOpt->value())
    { // free camera
        m_pos = QVector3D(0.0, 1.0, 10.0);
        m_pivot = QVector3D(0.0, 0.0, 0.0);
        m_xRot = -90.0f;
        m_zRot = 0.0f;
        m_step = 0.5f;
    }
    else
    { // horizontal camera
        m_pos = QVector3D(0.0, 1.0, 30.0);
        m_pivot = QVector3D(0.0, 0.0, 20.0);
        m_xRot = -70.0f;
        m_zRot = 45.0f;
        m_step = 0.5f;
    }
}

void CCamera::xRotate(float& angle)
{
    m_xRot += angle;
    util::normalizeAngle(m_xRot);
}

void CCamera::zRotate(float& angle)
{
    m_zRot += angle;
    util::normalizeAngle(m_zRot);
}

QMatrix4x4 CCamera::update()
{
    move();
    QMatrix4x4 matrix;
    matrix.setToIdentity();
    matrix.translate(m_pivot.x(), m_pivot.y(), m_pivot.z());
    matrix.translate(-m_pos.x(), -m_pos.y(), -m_pos.z());
    matrix.rotate(m_xRot, 1.0, 0.0, 0.0);
    matrix.rotate(m_zRot, 0.0, 0.0, 1.0);
    matrix.translate(-m_pivot.x(), -m_pivot.y(), -m_pivot.z());
    return matrix;
}

QMatrix4x4 CCamera::viewMatrix()
{
    QMatrix4x4 matrix;
    matrix.setToIdentity();
    matrix.translate(m_pivot.x(), m_pivot.y(), m_pivot.z());
    matrix.translate(-m_pos.x(), -m_pos.y(), -m_pos.z());
    matrix.rotate(m_xRot, 1.0, 0.0, 0.0);
    matrix.rotate(m_zRot, 0.0, 0.0, 1.0);
    matrix.translate(-m_pivot.x(), -m_pivot.y(), -m_pivot.z());
    return matrix;
}

void CCamera::strafeRight(bool bShift)
{
    QVector3D dir(1.0, 0.0, 0.0);
    move(dir, bShift);
}

void CCamera::strafeLeft(bool bShift)
{
    QVector3D dir(-1.0, 0.0, 0.0);
    move(dir, bShift);
}

void CCamera::strafeForward(bool bShift)
{
    QVector3D dir(m_pivot-m_pos);
    move(dir, bShift);
}

void CCamera::shiftForward(bool bShift)
{
    QVector3D dir(m_pivot-m_pos);
    shift(dir, bShift);
}

void CCamera::strafeBack(bool bShift)
{
    QVector3D dir(m_pos-m_pivot);
    move(dir, bShift);
}

void CCamera::shiftBack(bool bShift)
{
    QVector3D dir(m_pos-m_pivot);
    shift(dir, bShift);
}

void CCamera::strafeUp(bool bShift)
{
    QVector3D dir(0.0f, 1.0f, 0.0f);
    move(dir, bShift);
}

void CCamera::strafeUpward(bool bShift)
{
    QVector3D dir(0.0f, 0.0f, 1.0f);
    m_pos += dir*(bShift ? m_step*2.0f : m_step);
    m_pivot += dir*(bShift ? m_step*2.0f : m_step);
}

void CCamera::strafeDown(bool bShift)
{
    QVector3D dir(0.0f, -1.0f, 0.0f);
    move(dir, bShift);
}

void CCamera::strafeDownward(bool bShift)
{
    QVector3D dir(0.0f, 0.0f, -1.0f);
    m_pos += dir*(bShift ? m_step*2.0f : m_step);
    m_pivot += dir*(bShift ? m_step*2.0f : m_step);
}

void CCamera::move(QVector3D& orDir, bool bShift)
{
    QMatrix4x4 rot;
    rot.setToIdentity();
    rot.rotate(m_xRot, 1.0, 0.0, 0.0);
    rot.rotate(m_zRot, 0.0, 0.0, 1.0);
    orDir=rot.transposed()*orDir;
    orDir.normalize();

    m_pos += orDir*(bShift ? m_step*2.0f : m_step);
    m_pivot += orDir*(bShift ? m_step*2.0f : m_step);
}

void CCamera::shift(QVector3D& orDir, bool bShift)
{
    QMatrix4x4 rot;
    rot.setToIdentity();
    rot.rotate(m_xRot, 1.0, 0.0, 0.0);
    rot.rotate(m_zRot, 0.0, 0.0, 1.0);
    orDir=rot.transposed()*orDir;
    orDir.setZ(0.0f);
    orDir.normalize();

    const QVector3D step = orDir*(bShift ? m_step*2.0f : m_step);
    m_pos += step;
    m_pivot += step;
}

void CCamera::enlarge(const bool bZoom)
{
    COptBool* pOpt = dynamic_cast<COptBool*>(m_pSettings->opt("freeCamera"));
    Q_ASSERT(pOpt);
    if (pOpt->value())
    {
        QVector3D dir = bZoom ? m_pivot-m_pos : m_pos-m_pivot;
        dir.normalize();
        m_pos += dir*m_step;
    }
    else
    {
        if(bZoom)
            strafeDownward(m_keyManager->isPressed(Qt::Key_Shift));
        else
            strafeUpward(m_keyManager->isPressed(Qt::Key_Shift));
    }
}

void CCamera::move()
{
    COptBool* pOpt = dynamic_cast<COptBool*>(m_pSettings->opt("freeCamera"));
    Q_ASSERT(pOpt);
    if (pOpt->value())
    { // free camera
        if(m_keyManager->isPressed(Qt::Key_W) || m_keyManager->isPressed(Qt::Key_Up))
            strafeForward(m_keyManager->isPressed(Qt::Key_Shift));
        if(m_keyManager->isPressed(Qt::Key_S) || m_keyManager->isPressed(Qt::Key_Down))
            strafeBack(m_keyManager->isPressed(Qt::Key_Shift));
    }
    else
    {
        if(m_keyManager->isPressed(Qt::Key_W) || m_keyManager->isPressed(Qt::Key_Up))
            shiftForward(m_keyManager->isPressed(Qt::Key_Shift));
        if(m_keyManager->isPressed(Qt::Key_S) || m_keyManager->isPressed(Qt::Key_Down))
            shiftBack(m_keyManager->isPressed(Qt::Key_Shift));
    }
    if(m_keyManager->isPressed(Qt::Key_D) || m_keyManager->isPressed(Qt::Key_Right))
        strafeRight(m_keyManager->isPressed(Qt::Key_Shift));
    if(m_keyManager->isPressed(Qt::Key_A) || m_keyManager->isPressed(Qt::Key_Left))
        strafeLeft(m_keyManager->isPressed(Qt::Key_Shift));
    if(m_keyManager->isPressed(Qt::Key_E))
        strafeUpward(m_keyManager->isPressed(Qt::Key_Shift));
    if(m_keyManager->isPressed(Qt::Key_Q))
        strafeDownward(m_keyManager->isPressed(Qt::Key_Shift));
}

void CCamera::attachSettings(CSettings *pSetting)
{
    m_pSettings = pSetting;
    if (m_pSettings)
    {
        COptBool* pOpt = dynamic_cast<COptBool*>(m_pSettings->opt("freeCamera"));
        Q_ASSERT(pOpt);
        if (pOpt->value())
        { // free camera
            m_pos = QVector3D(0.0, 1.0, 30.0);
            m_pivot = QVector3D(0.0, 0.0, 10.0);
            m_xRot = -70.0f;
            m_zRot = 45.0f;
        }
    }
}
