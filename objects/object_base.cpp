#include "objects\object_base.h"
#include <QJsonArray>
#include <QString>
#include "mob\mob.h"
#include "landscape.h"
#include "view.h"
#include "log.h"
#include "resourcemanager.h"
#include "property.h"

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

void CObjectBase::draw(bool isActive, QOpenGLShaderProgram* program)
{
    Q_UNUSED(isActive);
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
        QMatrix4x4 rtMatrix;
        rtMatrix.setToIdentity();
        rtMatrix.rotate(m_rotation);
        matrix = matrix * rtMatrix;
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
        QMatrix4x4 rtMatrix;
        rtMatrix.setToIdentity();
        rtMatrix.rotate(m_rotation);
        matrix = matrix * rtMatrix;
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
    rot.append(QJsonValue::fromVariant(m_rotation.x()));
    rot.append(QJsonValue::fromVariant(m_rotation.y()));
    rot.append(QJsonValue::fromVariant(m_rotation.z()));
    rot.append(QJsonValue::fromVariant(m_rotation.scalar()));
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
    rot.append(QJsonValue::fromVariant(m_rotation.x()));
    rot.append(QJsonValue::fromVariant(m_rotation.y()));
    rot.append(QJsonValue::fromVariant(m_rotation.z()));
    rot.append(QJsonValue::fromVariant(m_rotation.scalar()));
    obj.insert("Rotation", rot);
    return obj;
}

CBox CObjectBase::getBBox()
{
    QVector3D min;
    QVector3D max;
    bool bInit = false;
    QMatrix4x4 rtMatrix;
    rtMatrix.setToIdentity();
    rtMatrix.rotate(m_rotation);
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

            rotatedPos = rtMatrix*arrVert[i].position; //get vector, rotated with matrix
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

void CObjectBase::collectParams(QMap<QSharedPointer<IPropertyBase>, bool>& aProp, ENodeType paramType)
{
    Q_UNUSED(paramType);
    propUint mapId(eObjParam_NID, m_mapID);
    util::addParam(aProp, &mapId);
    propStr comment(eObjParam_COMMENTS, m_comment);
    util::addParam(aProp, &comment);
    prop3D pos(eObjParam_POSITION, m_position);
    util::addParam(aProp, &pos);
}

void CObjectBase::collectlogicParams(QMap<QSharedPointer<IPropertyBase>, bool>& aProp, ENodeType paramType)
{
    Q_UNUSED(aProp);
    Q_UNUSED(paramType);
    Q_ASSERT(false && "todo: implement");
}

void CObjectBase::getParam(QSharedPointer<IPropertyBase>& prop, EObjParam propType)
{
    switch (propType){
    case eObjParam_NID:
    {
        prop.reset(new propUint(propType, m_mapID));
        break;
    }
    case eObjParam_NAME:
    {
        prop.reset(new propStr(propType, m_name));
        break;
    }
    case eObjParam_POSITION:
    {
        prop.reset(new prop3D(propType, m_position));
        break;
    }
    case eObjParam_COMMENTS:
    {
        prop.reset(new propStr(propType, m_comment));
        break;
    }
    default:
        Q_ASSERT(false);
    }
}

void CObjectBase::applyParam(const QSharedPointer<IPropertyBase>& prop)
{
    switch (prop->type()) {
    case eObjParam_NID:
    {
        m_mapID = dynamic_cast<const propUint*>(prop.get())->value();
        break;
    }
    case eObjParam_NAME:
    {
        m_name = dynamic_cast<const propStr*>(prop.get())->value();
        break;
    }
    case eObjParam_COMMENTS:
    {
        m_comment = dynamic_cast<const propStr*>(prop.get())->value();
        break;
    }
    case eObjParam_POSITION:
    {
        QVector3D pos = dynamic_cast<const prop3D*>(prop.get())->value();
        updatePos(pos);
        break;
    }
    default:
        //Q_ASSERT(false);
        break;
    }
}

void CObjectBase::getLogicParam(QSharedPointer<IPropertyBase>& prop, EObjParam propType)
{
    Q_UNUSED(prop);
    Q_UNUSED(propType);
    Q_ASSERT(false);
    return;
}

void CObjectBase::applyLogicParam(const QSharedPointer<IPropertyBase>& prop)
{
    Q_UNUSED(prop);
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
    QMatrix4x4 rtMatrix;
    rtMatrix.setToIdentity();
    rtMatrix.rotate(m_rotation);
    for(auto& part: m_aPart)
        for(auto& vert : part->vertData())
        {
            rotatedPos = rtMatrix*vert.position; //get vector, rotated with matrix
            if (rotatedPos.z() < min)
                min = rotatedPos.z();
        }

    m_minPoint = QVector3D(0.0f, 0.0f, min);
    CLandscape::getInstance()->projectPosition(this);
}


