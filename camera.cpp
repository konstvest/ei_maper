#include <QGLWidget>
#include <QtMath>
#include "camera.h"
#include "key_manager.h"
#include "settings.h"
#include "log.h"

CCamera::CCamera()
    :m_keyManager(nullptr)
    ,m_pSettings(nullptr)
{
    m_pos = QVector3D(0.0, 1.0, 10.0);
    m_pivot = QVector3D(0.0, 0.0, 0.0);
    m_xRot = -90.0f;
    m_zRot = 0.0f;
    m_step = 0.5f;
}

CCamera::CCamera(QVector3D pos, QVector3D pivot, float xRot, float zRot)
    :m_pos(pos)
    ,m_pivot(pivot)
    ,m_xRot(xRot)
    ,m_zRot(zRot)
    ,m_step(0.5f)
    ,m_keyManager(nullptr)
    ,m_pSettings(nullptr)
{

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
        m_step = 1.0f;
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
            strafeDownward(nullptr == m_keyManager ? false : m_keyManager->isPressed(eKey_Shift));
        else
            strafeUpward(nullptr == m_keyManager ? false : m_keyManager->isPressed(eKey_Shift));
    }
}

void CCamera::move()
{
    if(nullptr ==  m_keyManager)
    {
        return;
    }

    COptBool* pOpt = dynamic_cast<COptBool*>(m_pSettings->opt("freeCamera"));
    Q_ASSERT(pOpt);
    if (pOpt->value())
    { // free camera
        if(m_keyManager->isPressed(eKey_W) || m_keyManager->isPressed(eKey_UpArrow))
            strafeForward(m_keyManager->isPressed(eKey_Shift));
        if(m_keyManager->isPressed(eKey_S) || m_keyManager->isPressed(eKey_DownArrow))
            strafeBack(m_keyManager->isPressed(eKey_Shift));
    }
    else
    {
        if(m_keyManager->isPressed(eKey_W) || m_keyManager->isPressed(eKey_UpArrow))
            shiftForward(m_keyManager->isPressed(eKey_Shift));
        if(m_keyManager->isPressed(eKey_S) || m_keyManager->isPressed(eKey_DownArrow))
            shiftBack(m_keyManager->isPressed(eKey_Shift));
    }
    if(m_keyManager->isPressed(eKey_D) || m_keyManager->isPressed(eKey_RightArrow))
        strafeRight(m_keyManager->isPressed(eKey_Shift));
    if(m_keyManager->isPressed(eKey_A) || m_keyManager->isPressed(eKey_LeftArrow))
        strafeLeft(m_keyManager->isPressed(eKey_Shift));
    if(m_keyManager->isPressed(eKey_E))
        strafeUpward(m_keyManager->isPressed(eKey_Shift));
    if(m_keyManager->isPressed(eKey_Q))
        strafeDownward(m_keyManager->isPressed(eKey_Shift));
}

// move camera to 'posTarget' point
void CCamera::moveTo(QVector3D posTarget)
{
    QVector3D dif = m_pos - m_pivot;
    m_pivot = posTarget;
    m_pos = m_pivot + dif;
}

//move camera away on 'distance' point from pivot
void CCamera::moveAwayOn(float distance)
{
    if(distance==0)
    {
        ei::log(eLogWarning, "show distance is null. object empty");
        return;
    }
    QVector3D dir(m_pos-m_pivot);
    dir.normalize();
    m_pos = m_pivot + dir*distance;
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
