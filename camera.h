#ifndef CAMERA_H
#define CAMERA_H

#include <QMatrix4x4>
#include "utils.h"

class CKeyManager;

class CCamera
{
public:
    CCamera();
    ~CCamera(){}

    QMatrix4x4 update();
    void xRotate(float& angle);
    void zRotate(float& angle);
    void strafeLeft(bool bShift = false);
    void strafeRight(bool bShift = false);
    void strafeForward(bool bShift = false);
    void strafeBack(bool bShift = false);
    void strafeUp(bool bShift = false);
    void strafeUpward(bool bShift = false);
    void strafeDown(bool bShift = false);
    void strafeDownward(bool bShift = false);
    void enlarge(const bool bZoom = true);
    void reset();
    float step() const {return m_step;}
    QVector3D& pos() {return m_pos;}
    void move();
    void attachKeyManage(CKeyManager* km) {m_keyManager = km;}

private:
    void move(QVector3D& orDir, bool bShift = false);

private:
    QVector3D m_pos;
    QVector3D m_pivot;
    float m_xRot;
    float m_zRot;
    float m_step;
    CKeyManager* m_keyManager;
};

#endif // CAMERA_H
