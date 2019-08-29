#include <QGLWidget>
#include <QtMath>
#include "camera.h"
#include "key_manager.h"

CCamera::CCamera()
{
    reset();
}

void CCamera::reset()
{
    m_pos = QVector3D(0.0, 1.0, 10.0);
    m_pivot = QVector3D(0.0, 0.0, 0.0);
    m_xRot = -90.0f;
    m_zRot = 0.0f;
    m_step = 0.3f;
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

void CCamera::strafeBack(bool bShift)
{
    QVector3D dir(m_pos-m_pivot);
    move(dir, bShift);
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

void CCamera::enlarge(const bool bZoom)
{
    QVector3D dir = bZoom ? m_pivot-m_pos : m_pos-m_pivot;
    dir.normalize();
    m_pos += dir*m_step;
}

void CCamera::move()
{
    if(m_keyManager->isPressed(Qt::Key_W) || m_keyManager->isPressed(Qt::Key_Up))
        strafeForward(m_keyManager->isPressed(Qt::Key_Shift));
    if(m_keyManager->isPressed(Qt::Key_S) || m_keyManager->isPressed(Qt::Key_Down))
        strafeBack(m_keyManager->isPressed(Qt::Key_Shift));
    if(m_keyManager->isPressed(Qt::Key_D) || m_keyManager->isPressed(Qt::Key_Right))
        strafeRight(m_keyManager->isPressed(Qt::Key_Shift));
    if(m_keyManager->isPressed(Qt::Key_A) || m_keyManager->isPressed(Qt::Key_Left))
        strafeLeft(m_keyManager->isPressed(Qt::Key_Shift));
    if(m_keyManager->isPressed(Qt::Key_E))
        strafeUpward(m_keyManager->isPressed(Qt::Key_Shift));
    if(m_keyManager->isPressed(Qt::Key_Q))
        strafeDownward(m_keyManager->isPressed(Qt::Key_Shift));
}
