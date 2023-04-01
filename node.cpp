#include <QtMath>
#include <QMatrix3x3>
#include "node.h"

uint CNode::s_freeId = 0;

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
    //m_rotation ?
}

CNode::CNode():
    m_position(0.0, 0.0, 0.0)
    ,m_mapID(0)
    ,m_name("")
    ,m_comment("")
    ,m_parent(nullptr)
    ,m_state(eDraw)
{
    init();
}

CNode::CNode(CNode* parent):
    m_position(0.0, 0.0, 0.0)
{
    if (m_parent)
        m_parent = parent;
    init();
}

CNode::CNode(const CNode &node):
    QObject()
  ,m_position(node.m_position)
  ,m_drawPosition(node.m_drawPosition)
  ,m_mapID(node.m_mapID)
  ,m_name(node.m_name)
  ,m_comment(node.m_comment)
  ,m_rotation(node.m_rotation)
  ,m_parent(node.m_parent)
  ,m_state(node.m_state)
{
    init();
    //children?
}

CNode::~CNode()
{
    for(auto& child : m_aChild)
        delete child;
}

// 3D Rotation Converter: https://www.andre-gaschler.com/rotationconverter/
QVector3D CNode::getEulerRotation()
{
    QMatrix4x4 rtMatrix;
    rtMatrix.setToIdentity();
    rtMatrix.rotate(m_rotation);
    QMatrix3x3 mtrx3 = rtMatrix.normalMatrix();
    QQuaternion quat;
    quat = QQuaternion::fromRotationMatrix(mtrx3);
    return quat.toEulerAngles();
}

void CNode::move(float x, float y, float z)
{
    m_position.setX(m_position.x()+x);
    m_position.setY(m_position.y()+y);
    m_position.setZ(m_position.z()+z);
}
