#include "objects\object_base.h"
#include <QJsonArray>
#include <QString>
#include "mob.h"
#include "landscape.h"
#include "view.h"
#include "log.h"
#include "objectlist.h"
#include "texturelist.h"

CObjectBase::CObjectBase():
    m_pMob(nullptr)
    ,m_texture(nullptr)
    ,m_pFigure(nullptr)
{
    m_bodyParts.clear();
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
    if(!m_pMob)
    {
        ei::log(eLogError, "Updating figure failed, Mob file empty");
        return;
    }
    updateFigure(m_pMob->view()->objList()->getFigure(m_modelName));
}

void CObjectBase::loadTexture()
{
    if(!m_pMob)
    {
        ei::log(eLogError, "Updating texture failed, Mob file empty");
        return;
    }
    QString texName("default0");
    auto a = m_pMob->view()->texList()->texture(texName);
    if(nullptr == a)
        int d(777);
    setTexture(m_pMob->view()->texList()->texture(texName));
}

void CObjectBase::setTexture(QOpenGLTexture* texture)
{
    Q_ASSERT(texture);
    m_texture = texture;
}

void CObjectBase::draw(QOpenGLShaderProgram* program)
{
    if (m_state == ENodeState::eHidden) //dont draw hidden objects
        return;

    Q_ASSERT(m_texture);
    if(m_texture == nullptr)
        return;

    m_texture->bind(0);
    if (!m_parent)
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
    Q_ASSERT(m_texture);
    m_texture->bind(0);
    if(m_texture == nullptr)
        return;

    if (!m_parent)
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

void CObjectBase::addParam(QMap<EObjParam, QString>& aParam, EObjParam param, QString str)
{
    if (aParam.contains(param))
    {
        if (aParam[param] != str)
            aParam.insert(param, "");
    }
    else
        aParam.insert(param, str);
}

bool CObjectBase::updatePos(QVector3D& pos)
{
    m_position = pos;
    m_pMob->view()->land()->projectPosition(this);
    return true;
}

void CObjectBase::attachMob(CMob *mob)
{
    m_pMob = mob;
}

void CObjectBase::setConstitution(QVector3D &vec)
{
    m_complection = vec;
    recalcFigure();
    m_pMob->view()->land()->projectPosition(this);
}

void CObjectBase::setRot(const QQuaternion& quat)
{
    CNode::setRot(quat);
    recalcMinPos();
}

void CObjectBase::collectParams(QMap<EObjParam, QString> &aParam, ENodeType paramType)
{
    Q_UNUSED(paramType);
    addParam(aParam, eObjParam_NID, QString::number(m_mapID));
    addParam(aParam, eObjParam_TEMPLATE, m_modelName);
    addParam(aParam, eObjParam_COMMENTS, m_comment);
    addParam(aParam, eObjParam_POSITION, util::makeString(m_position));
}

void CObjectBase::applyParam(EObjParam param, const QString &value)
{

    switch (param){
    case eObjParam_NID:
    {
        m_mapID = value.toInt();
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
    if(m_pMob)
        m_pMob->view()->land()->projectPosition(this);
}


