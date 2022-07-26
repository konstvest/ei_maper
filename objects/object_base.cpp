#include "objects\object_base.h"
#include <QJsonArray>
#include <QString>
#include "mob.h"
#include "landscape.h"
#include "view.h"
#include "log.h"
#include "resourcemanager.h"

CObjectBase::CObjectBase(): 
  m_modelName("")
  ,m_complection(1.0f, 1.0f, 1.0f)
  ,m_minPoint(0.0f, 0.0f, 0.0f)
  ,m_texture(nullptr)
  ,m_pFigure(nullptr)
  ,m_bDeleted(false)
{

}

CObjectBase::CObjectBase(const CObjectBase &base):
    CNode(base)
{
    m_complection = base.m_complection;
    m_texture = base.m_texture;
    m_pFigure = base.m_pFigure;
    m_minPoint = base.m_minPoint;
    m_bodyParts = base.m_bodyParts;

    m_aPart.clear();
    for(auto& part:base.m_aPart)
    {
        m_aPart.append(new CPart(*part));
    }
}

CObjectBase::CObjectBase(QJsonObject data):
    m_texture(nullptr)
    ,m_pFigure(nullptr)
  ,m_bDeleted(false)
{

    //m_mapID = data["Id"].toVariant().toUInt(); //TODO: generate mapID
    //m_mapID = 333777;

    m_name = data["Map name"].toString();
    m_comment = data["Comments"].toString();

    QJsonArray arrComplection = data["Complection"].toArray();
    if (arrComplection.size() == 3)
        m_complection = QVector3D(arrComplection[0].toVariant().toFloat(), arrComplection[1].toVariant().toFloat(), arrComplection[2].toVariant().toFloat());

    //TODO: generate position (use mouse diff if object mmore than one
    QJsonArray arrPos = data["Position"].toArray();
    if (arrPos.size() == 3)
        m_position = QVector3D(arrPos[0].toVariant().toFloat(), arrPos[1].toVariant().toFloat(), arrPos[2].toVariant().toFloat());

    QJsonArray arrQuot = data["Rotation"].toArray();
    if (arrQuot.size() == 4)
    {
        QVector4D rot(arrQuot[0].toVariant().toFloat(), arrQuot[1].toVariant().toFloat(), arrQuot[2].toVariant().toFloat(), arrQuot[3].toVariant().toFloat());
        CNode::setRot(rot);
    }

}

CObjectBase::CObjectBase(CNode* node):
    CNode(node)
    ,m_texture(nullptr)
    ,m_pFigure(nullptr)
{
}

CObjectBase::~CObjectBase()
{
    for(auto& part: m_aPart)
        delete part;
}

void CObjectBase::updateFigure(ei::CFigure* fig)
{
    Q_ASSERT(fig);
    m_pFigure = fig;
    recalcFigure();
}

void CObjectBase::loadFigure()
{
    updateFigure(CObjectList::getInstance()->getFigure(m_modelName));
}

void CObjectBase::loadTexture()
{
//    QString texName("default0");
//    setTexture(CTextureList::getInstance()->texture(texName));
}

void CObjectBase::setTexture(QOpenGLTexture* texture)
{
    Q_ASSERT(texture);
    m_texture = texture;
}

void CObjectBase::draw(QOpenGLShaderProgram* program)
{
    if(isMarkDeleted())
        return;

    if (m_state == ENodeState::eHidden) //dont draw hidden objects
        return;

    //Q_ASSERT(m_texture);
    if(m_texture == nullptr)
    {
        loadTexture();
        ei::log(eLogWarning, "empty texture, set default");
        return;
    }

    m_texture->bind(0);
    //if (!m_parent)
    {
        QMatrix4x4 matrix;
        matrix.setToIdentity();
        matrix.translate(m_drawPosition);
        matrix = matrix * m_rotateMatrix;
        program->setUniformValue("u_modelMmatrix", matrix);
    }

    if (m_state == ENodeState::eSelect)
        program->setUniformValue("u_highlight", true);

    program->setUniformValue("qt_Texture0", 0);
    for(auto& part: m_aPart)
        part->draw(program);

    program->setUniformValue("u_highlight", false);
}

void CObjectBase::drawSelect(QOpenGLShaderProgram* program)
{
    if(isMarkDeleted())
        return;

    if (m_state == ENodeState::eHidden) //skip hidden object for select
        return;

    Q_ASSERT(m_texture);
    m_texture->bind(0);
    if(m_texture == nullptr)
        return;

    //if (!m_parent)
    {
        QMatrix4x4 matrix;
        matrix.setToIdentity();
        matrix.translate(m_drawPosition);
        matrix = matrix * m_rotateMatrix;
        program->setUniformValue("u_modelMmatrix", matrix);
        program->setUniformValue("u_color", m_pickingColor.toVec4());
    }

    for(auto& part: m_aPart)
        part->drawSelect(program);
}

void CObjectBase::serializeJson(QJsonObject& obj)
{
    obj.insert("Model name", m_modelName);
    obj.insert("Id", QJsonValue::fromVariant(m_mapID));
    QJsonArray aComplection;
    aComplection.append(QJsonValue::fromVariant(m_complection.x()));
    aComplection.append(QJsonValue::fromVariant(m_complection.y()));
    aComplection.append(QJsonValue::fromVariant(m_complection.z()));
    obj.insert("Complection", aComplection);

    QJsonArray pos;
    pos.append(QJsonValue::fromVariant(m_position.x()));
    pos.append(QJsonValue::fromVariant(m_position.y()));
    pos.append(QJsonValue::fromVariant(m_position.z()));
    obj.insert("Position", pos);

    QJsonArray rot;
    QMatrix3x3 mtrx3 = m_rotateMatrix.normalMatrix();
    QQuaternion quat;
    quat = QQuaternion::fromRotationMatrix(mtrx3);
    rot.append(QJsonValue::fromVariant(quat.x()));
    rot.append(QJsonValue::fromVariant(quat.y()));
    rot.append(QJsonValue::fromVariant(quat.z()));
    rot.append(QJsonValue::fromVariant(quat.scalar()));
    obj.insert("Rotation", rot);

    return;
}

uint CObjectBase::serialize(util::CMobParser &parser)
{
    Q_UNUSED(parser);
    Q_ASSERT("What are you doing here?" && false);
    return 0;
}



bool CObjectBase::updatePos(QVector3D& pos)
{
    m_position = pos;
    CLandscape::getInstance()->projectPosition(this);
    return true;
}

void CObjectBase::setConstitution(QVector3D &vec)
{
    m_complection = vec;
    recalcFigure();
    CLandscape::getInstance()->projectPosition(this);
}

QJsonObject CObjectBase::toJson()
{
    QJsonObject obj;
    obj.insert("Id", QJsonValue::fromVariant(m_mapID));
    obj.insert("Node type", nodeType());

    obj.insert("Map name", m_name);
    obj.insert("Comments", m_comment);

    QJsonArray aComplection;
    aComplection.append(QJsonValue::fromVariant(m_complection.x()));
    aComplection.append(QJsonValue::fromVariant(m_complection.y()));
    aComplection.append(QJsonValue::fromVariant(m_complection.z()));
    obj.insert("Complection", aComplection);

    QJsonArray pos;
    pos.append(QJsonValue::fromVariant(m_position.x()));
    pos.append(QJsonValue::fromVariant(m_position.y()));
    pos.append(QJsonValue::fromVariant(m_position.z()));
    obj.insert("Position", pos);

    QJsonArray rot;
    QMatrix3x3 mtrx3 = m_rotateMatrix.normalMatrix();
    QQuaternion quat;
    quat = QQuaternion::fromRotationMatrix(mtrx3);
    rot.append(QJsonValue::fromVariant(quat.x()));
    rot.append(QJsonValue::fromVariant(quat.y()));
    rot.append(QJsonValue::fromVariant(quat.z()));
    rot.append(QJsonValue::fromVariant(quat.scalar()));
    obj.insert("Rotation", rot);
    return obj;
}

CBox CObjectBase::getBBox()
{
    QVector3D min;
    QVector3D max;
    bool bInit = false;
    const auto fillPoint = [&min, &max](QVector3D& vec)
    {
        //x
        if(vec.x() < min.x())
            min.setX(vec.x());
        else if(vec.x() > max.x())
            max.setX(vec.x());
        //y
        if(vec.y() < min.y())
            min.setY(vec.y());
        else if(vec.y() > max.y())
            max.setY(vec.y());
        //z
        if(vec.z() < min.z())
            min.setZ(vec.z());
        else if(vec.z() > max.z())
            max.setZ(vec.z());
    };

    QVector3D rotatedPos;
    for(auto& part: m_aPart)
    {
        auto& arrVert = part->vertData();
        for(int i(0); i < arrVert.size(); ++i)
        {
            if(!bInit)
            {
                min = arrVert[i].position;
                max = arrVert[i].position;
                bInit = true;
                continue;
            }

            rotatedPos = m_rotateMatrix*arrVert[i].position; //get vector, rotated with matrix
            fillPoint(rotatedPos);
        }
    }

   CBox bbox(min, max);
   return bbox;
}

void CObjectBase::setRot(const QQuaternion& quat)
{
    CNode::setRot(quat);
    recalcMinPos();
}

void CObjectBase::collectParams(QMap<EObjParam, QString> &aParam, ENodeType paramType)
{
    Q_UNUSED(paramType);
    util::addParam(aParam, eObjParam_NID, QString::number(m_mapID));

    util::addParam(aParam, eObjParam_COMMENTS, m_comment);
    util::addParam(aParam, eObjParam_POSITION, util::makeString(m_position));
}

void CObjectBase::collectlogicParams(QMap<EObjParam, QString> &aParam, ENodeType paramType)
{
    Q_UNUSED(aParam);
    Q_UNUSED(paramType);
    Q_ASSERT(false);
}

void CObjectBase::applyParam(EObjParam param, const QString &value)
{

    switch (param){
    case eObjParam_NID:
    {
        m_mapID = value.toUInt();
        break;
    }
    case eObjParam_NAME:
    {
        m_name = value;
        break;
    }
    case eObjParam_COMMENTS:
    {
        m_comment = value;
        break;
    }
    case eObjParam_POSITION:
    {
        QVector3D pos = util::vec3FromString(value);
        updatePos(pos);
        break;
    }
    default:
        Q_ASSERT(false);
    }

}

QString CObjectBase::getParam(EObjParam param)
{
    QString value;
    switch (param){
    case eObjParam_NID:
    {
        value = QString::number(m_mapID);
        break;
    }
    case eObjParam_NAME:
    {
        value = m_name;
        break;
    }
    case eObjParam_POSITION:
    {
        value = util::makeString(m_position);
        break;
    }
    case eObjParam_COMMENTS:
    {
        value = m_comment;
        break;
    }
    default:
        Q_ASSERT(false);
    }
    return value;
}

QString CObjectBase::getLogicParam(EObjParam param)
{
    Q_UNUSED(param);
    Q_ASSERT(false);
    return "";
}

void CObjectBase::applyLogicParam(EObjParam param, const QString &value)
{
    Q_UNUSED(param);
    Q_UNUSED(value);
    Q_ASSERT(false);
}

void CObjectBase::recalcFigure()
{
    for(auto& part: m_aPart)
        delete part;
    m_aPart.clear();
    m_pFigure->getVertexData(m_aPart, m_complection, m_bodyParts);
    recalcMinPos();
}

void CObjectBase::recalcMinPos()
{
    float min(1000.0f); // 0.0f?
    QVector3D rotatedPos;
    for(auto& part: m_aPart)
        for(auto& vert : part->vertData())
        {
            rotatedPos = m_rotateMatrix*vert.position; //get vector, rotated with matrix
            if (rotatedPos.z() < min)
                min = rotatedPos.z();
        }

    m_minPoint = QVector3D(0.0f, 0.0f, min);
    CLandscape::getInstance()->projectPosition(this);
}


