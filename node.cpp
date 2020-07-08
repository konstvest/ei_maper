#include <QtMath>
#include <QMatrix3x3>
#include "node.h"

int CNode::s_freeId;

// generate color by node ID (for selecting)
static SColor generateColor(int id)
{
    const uchar r = uchar(id/65025);
    const uchar g = uchar(id/255);
    const uchar b = uchar(id%255);
    return SColor(r,g,b);
}

void CNode::init()
{
    m_aChild.clear();
    ++s_freeId;
    m_id = s_freeId;
    m_pickingColor = generateColor(m_id);
    m_rotateMatrix.setToIdentity();
}

CNode::CNode():
    m_position(0.0, 0.0, 0.0)
    //,m_rotation(0.0, 0.0, 0.0)
    ,m_parent(nullptr)
{
    init();
}

CNode::CNode(CNode* parent):
    m_position(0.0, 0.0, 0.0)
    //,m_rotation(0.0, 0.0, 0.0)
{
    if (m_parent)
        m_parent = parent;
    init();
}

CNode::~CNode()
{
    for(auto& child : m_aChild)
        delete child;
}

void CNode::setRot(QVector4D& quat)
{
    m_rotateMatrix.setToIdentity();
    m_rotateMatrix.rotate(QQuaternion(quat));
}

void CNode::move(float x, float y, float z)
{
    m_position.setX(m_position.x()+x);
    m_position.setY(m_position.y()+y);
    m_position.setZ(m_position.z()+z);
}

void CNode::rotate(QQuaternion& quat)
{
// 3D Rotation Converter: https://www.andre-gaschler.com/rotationconverter/

    quat.normalize();
    m_rotateMatrix.rotate(quat);
}

void CNode::applyRotation()
{
    m_rotateMatrix.setToIdentity();
//    m_rotateMatrix.rotate(m_rotation.x(), 1.0, 0.0, 0.0);
//    m_rotateMatrix.rotate(m_rotation.y(), 0.0, 1.0, 0.0);
//    m_rotateMatrix.rotate(m_rotation.z(), 0.0, 0.0, 1.0);
}
