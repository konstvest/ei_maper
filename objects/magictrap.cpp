#include <QJsonArray>
#include "magictrap.h"
#include "resourcemanager.h"
#include "landscape.h"
#include "settings.h"
#include "scene.h"
#include "property.h"

CMagicTrap::CMagicTrap():
    m_indexBuf(QOpenGLBuffer::IndexBuffer)
  ,m_diplomacy(-1)
    ,m_castInterval(0)
    ,m_bCastOnce(false)

{
    m_type = 59;
    updateFigure(CObjectList::getInstance()->getFigure("trap"));
    setTexture(CTextureList::getInstance()->texture("trap"));
}

CMagicTrap::CMagicTrap(const CMagicTrap &trap):
    CWorldObj(trap)
  ,m_indexBuf(QOpenGLBuffer::IndexBuffer)
{
    m_type = 59;
    m_diplomacy = trap.m_diplomacy;
    m_spell = trap.m_spell;
    for(auto& area : trap.m_aActZone)
    {
        if(area->isMarkDeleted())
            continue;
        CActivationZone* pZone = new CActivationZone(this, *area);
        QObject::connect(pZone, SIGNAL(changeActZone()), this, SLOT(update()));
        m_aActZone.append(pZone);
    }
    for(const auto& pPoint : trap.m_aCastPoint)
    {
        if(pPoint->isMarkDeleted())
            continue;
        CTrapCastPoint* pCast = new CTrapCastPoint(this, *pPoint);
        QObject::connect(pCast, SIGNAL(changeCastPoint), this, SLOT(update()));
        m_aCastPoint.append(pCast);
    }
    m_castInterval = trap.m_castInterval;
    m_bCastOnce = trap.m_bCastOnce;
    updateFigure(CObjectList::getInstance()->getFigure("trap"));
    setTexture(CTextureList::getInstance()->texture("trap"));
    update();
}

CMagicTrap::CMagicTrap(QJsonObject data):
    CWorldObj(data["World object"].toObject())
  ,m_indexBuf(QOpenGLBuffer::IndexBuffer)
{
    m_type = 59;
    m_diplomacy = data["Diplomacy group"].toVariant().toUInt();
    m_spell = data["Spell"].toString();
    QJsonArray aArea = data["Area act."].toArray();
    for(auto it=aArea.begin(); it<aArea.end(); ++it)
    {
        CActivationZone* pZone = new CActivationZone(this);
        pZone->deSerializeJson(it->toObject());
        QObject::connect(pZone, SIGNAL(changeActZone()), this, SLOT(update()));
        m_aActZone.append(pZone);
    }


    QJsonArray aTarget = data["Cast Points"].toArray();
    for (auto it=aTarget.begin(); it<aTarget.end(); ++it)
    {
        CTrapCastPoint* pCast = new CTrapCastPoint(this);
        pCast->deSerializeJsonArray(it->toArray());
        QObject::connect(pCast, SIGNAL(changeCastPoint), this, SLOT(update()));
        m_aCastPoint.append(pCast);
    }

    m_castInterval = data["Cast interval"].toVariant().toUInt();
    m_bCastOnce = data["Is cast once?"].toBool();
    updateFigure(CObjectList::getInstance()->getFigure("trap"));
    setTexture(CTextureList::getInstance()->texture("trap"));
    update();
}

CMagicTrap::~CMagicTrap()
{
    for(auto& pZone : m_aActZone)
        delete pZone;

    m_aActZone.clear();
}

void CMagicTrap::draw(bool isActive, QOpenGLShaderProgram* program)
{
    CObjectBase::draw(isActive, program);
    for(auto& pCast : m_aCastPoint)
        pCast->draw(isActive, program);

    if(m_aDrawingLine.size() == 0) // do not draw looking point if absent
        return;

    COptBool* pOpt = dynamic_cast<COptBool*>(CObjectList::getInstance()->settings()->opt("drawLogic"));
    if (nullptr == pOpt)
        return;

    if (!pOpt->value() && nodeState() != ENodeState::eSelect && CScene::getInstance()->getMode() != eEditModeLogic)
        return;

    glDisable(GL_DEPTH_TEST);
    QMatrix4x4 matrix;
    matrix.setToIdentity();
    program->setUniformValue("u_modelMmatrix", matrix);

    program->setUniformValue("customColor", QVector4D(0.2f, 1.0f, 0.2f, 1.0f));
    int offset(0);
    // Tell OpenGL which VBOs to use

    m_vertexBuf.bind();
    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, int(sizeof(QVector3D)));

    // Draw cube geometry using indices from VBO 1
    m_indexBuf.bind();
    glDrawElements(GL_LINES, (m_aDrawingLine.count()-1)*2, GL_UNSIGNED_SHORT, nullptr);
    program->setUniformValue("customColor", QVector4D(0.0, 0.0, 0.0, 0.0));

    for(auto& actZone : m_aActZone)
    {
        actZone->draw(isActive, program);
    }
    glEnable(GL_DEPTH_TEST);
}

uint CMagicTrap::deserialize(util::CMobParser& parser)
{
    uint readByte(0);
    while(true)
    {
        if(parser.isNextTag("MT_DIPLOMACY"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readDword(m_diplomacy);
        }
        else if(parser.isNextTag("MT_SPELL"))
        {
            readByte += parser.readHeader();
            readByte += parser.readString(m_spell, parser.nodeLen());
        }
        else if(parser.isNextTag("MT_AREAS"))
        {
            readByte += parser.readHeader();
            uint num;
            readByte += parser.readDword(num); // numbers of area
            for(uint i(0); i<num; ++i)
            {
                CActivationZone* pZone = new CActivationZone(this);
                readByte += pZone->deserialize(parser);
                QObject::connect(pZone, SIGNAL(changeActZone()), this, SLOT(update()));
                m_aActZone.append(pZone);
            }
            //todo: check len
        }
        else if(parser.isNextTag("MT_TARGETS"))
        {
            readByte += parser.readHeader();
            uint num;
            readByte += parser.readDword(num);
            for(uint i(0); i<num; ++i)
            {
                CTrapCastPoint* pCast = new CTrapCastPoint(this);
                readByte += pCast->deserialize(parser);
                QObject::connect(pCast, SIGNAL(changeCastPoint()), this, SLOT(update()));
                m_aCastPoint.append(pCast);
            }
            //todo: check len
        }
        else if(parser.isNextTag("MT_CAST_INTERVAL"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readDword(m_castInterval);
        }
        else if(parser.isNextTag("LEVER_CAST_ONCE"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readBool(m_bCastOnce);
        }
        else
        {
            uint baseByte = CWorldObj::deserialize(parser);
            if(baseByte > 0)
                readByte += baseByte;
            else
                break;
        }
    }
    Q_ASSERT(m_type==59);
    update();
    return readByte;
}

void CMagicTrap::serializeJson(QJsonObject& obj)
{
    CWorldObj::serializeJson(obj);
    obj.insert("Diplomacy group", QJsonValue::fromVariant(m_diplomacy));
    obj.insert("Spell", m_spell);
    QJsonArray aArea;
    for(auto& actZone : m_aActZone)
    {
        actZone->serializeJsonArray(aArea);
    }
    obj.insert("Area act.", aArea);

    QJsonArray aTarget;
    for(auto& pCast : m_aCastPoint)
    {
        QJsonArray pos;
        pCast->serializeJsonArray(pos);
        aTarget.append(pos);
    }
    obj.insert("Cast Points", aTarget);
    obj.insert("Cast interval", QJsonValue::fromVariant(m_castInterval));
    obj.insert("Is cast once?", m_bCastOnce);
}

uint CMagicTrap::serialize(util::CMobParser &parser)
{
    uint writeByte(0);
    writeByte += parser.startSection("MAGIC_TRAP");

    writeByte += parser.startSection("MT_DIPLOMACY");
    writeByte += parser.writeDword(m_diplomacy);
    parser.endSection(); //MT_DIPLOMACY

    writeByte += parser.startSection("MT_SPELL");
    writeByte += parser.writeString(m_spell);
    parser.endSection(); //MT_SPELL

    writeByte += parser.startSection("MT_CAST_INTERVAL");
    writeByte += parser.writeDword(m_castInterval);
    parser.endSection(); //MT_CAST_INTERVAL

    writeByte += parser.startSection("LEVER_CAST_ONCE");
    writeByte += parser.writeBool(m_bCastOnce);
    parser.endSection(); //LEVER_CAST_ONCE

    int nZone = 0;
    for(auto& pZone : m_aActZone)
    {
        if(!pZone->isMarkDeleted())
            ++nZone;
    }
    if(nZone)
    {
        writeByte += parser.startSection("MT_AREAS");
        writeByte += parser.writeDword(nZone);
        for(auto& pZone : m_aActZone)
        {
            //writeByte += parser.writeAreaArray(arrArea);
            if(pZone->isMarkDeleted())
                continue;
            writeByte += pZone->serialize(parser);
        }
        parser.endSection(); //MT_AREAS
    }

    nZone = 0;
    for(auto& pCast : m_aCastPoint)
    {
        if(!pCast->isMarkDeleted())
            ++nZone;
    }
    if(nZone)
    {
        writeByte += parser.startSection("MT_TARGETS");
        writeByte += parser.writeDword(nZone);
        for(auto& pCast : m_aCastPoint)
        {
            if(pCast->isMarkDeleted())
                continue;
            writeByte += pCast->serialize(parser);
        }
        parser.endSection(); //MT_TARGETS
    }

    writeByte += CWorldObj::serialize(parser);


    parser.endSection(); //MAGIC_TRAP
    return writeByte;
}

void CMagicTrap::collectlogicParams(QList<QSharedPointer<IPropertyBase>>& aProp, ENodeType paramType)
{
    auto comm = paramType & eMagicTrap;
    if (comm != eMagicTrap)
        return;

    propInt diplomacy(eObjParam_TRAP_DIPLOMACY, m_diplomacy);
    util::addParam(aProp, &diplomacy);
    propStr spell(eObjParam_TRAP_SPELL, m_spell);
    util::addParam(aProp, &spell);
    propUint castInterval(eObjParam_TRAP_CAST_INTERVAL, m_castInterval);
    util::addParam(aProp, &castInterval);
    propBool bCastOnce(eObjParam_TRAP_CAST_ONCE, m_bCastOnce);
    util::addParam(aProp, &bCastOnce);
}

void CMagicTrap::applyLogicParam(const QSharedPointer<IPropertyBase>& prop)
{
    switch (prop->type()){
    case eObjParam_TRAP_DIPLOMACY:
    {
        m_diplomacy = dynamic_cast<const propChar*>(prop.get())->value();
        break;
    }
    case eObjParam_TRAP_SPELL:
    {
        m_spell = dynamic_cast<const propStr*>(prop.get())->value();
        break;
    }
    case eObjParam_TRAP_CAST_INTERVAL:
    {
        m_castInterval = dynamic_cast<const propUint*>(prop.get())->value();
        break;
    }
    case eObjParam_TRAP_CAST_ONCE:
    {
        m_bCastOnce = dynamic_cast<const propBool*>(prop.get())->value();
        break;
    }
    default:
        Q_ASSERT(false);
    }
}

void CMagicTrap::getLogicParam(QSharedPointer<IPropertyBase>& prop, EObjParam propType)
{
    switch (propType) {
    case eObjParam_TRAP_DIPLOMACY:
    {
        prop.reset(new propChar(propType, m_diplomacy));
        break;
    }
    case eObjParam_TRAP_SPELL:
    {
        prop.reset(new propStr(propType, m_spell));
        break;
    }
    case eObjParam_TRAP_CAST_INTERVAL:
    {
        prop.reset(new propUint(propType, m_castInterval));
        break;
    }
    case eObjParam_TRAP_CAST_ONCE:
    {
        prop.reset(new propBool(propType, m_bCastOnce));
        break;
    }
    default:
    {
        Q_ASSERT(false);
        break;
    }
    }
    return;

}

void CMagicTrap::collectParams(QList<QSharedPointer<IPropertyBase>>& aProp, ENodeType paramType)
{
    CWorldObj::collectParams(aProp, paramType);

    auto comm = paramType & eMagicTrap;
    if (comm != eMagicTrap)
        return;

    //util::addParam(aParam, eObjParam_TRAP_DIPLOMACY, QString::number(m_diplomacy));
    //util::addParam(aParam, eObjParam_TRAP_SPELL, m_spell);
    //TODO: move to logic parameters
    //util::addParam(aParam, eObjParam_TRAP_AREAS, util::makeString(arrArea));
    //util::addParam(aParam, eObjParam_TRAP_TARGETS, util::makeString(m_aTarget));
    //util::addParam(aParam, eObjParam_TRAP_CAST_INTERVAL, QString::number(m_castInterval));
    //util::addParam(aParam, eObjParam_TRAP_CAST_ONCE, util::makeString(m_bCastOnce));

    util::removeProp(aProp, eObjParam_PRIM_TXTR);
    util::removeProp(aProp, eObjParam_TEMPLATE);
}

void CMagicTrap::getParam(QSharedPointer<IPropertyBase>& prop, EObjParam propType)
{
    switch (propType) {
    case eObjParam_TRAP_DIPLOMACY:
    {
        prop.reset(new propChar(propType, m_diplomacy));
        break;
    }
    case eObjParam_TRAP_SPELL:
    {
        prop.reset(new propStr(propType, m_spell));
        break;
    }
    case eObjParam_TRAP_CAST_INTERVAL:
    {
        prop.reset(new propUint(propType, m_castInterval));
        break;
    }
    case eObjParam_TRAP_CAST_ONCE:
    {
        prop.reset(new propBool(propType, m_bCastOnce));
        break;
    }
    default:
    {
        CWorldObj::getParam(prop, propType);
        break;
    }
    }
}

void CMagicTrap::applyParam(const QSharedPointer<IPropertyBase>& prop)
{
    switch (prop->type()) {
    case eObjParam_TRAP_DIPLOMACY:
    {
        m_diplomacy = dynamic_cast<propChar*>(prop.get())->value();
        break;
    }
    case eObjParam_TRAP_SPELL:
    {
        m_spell = dynamic_cast<propStr*>(prop.get())->value();
        break;
    }
    case eObjParam_TRAP_CAST_INTERVAL:
    {
        m_castInterval = dynamic_cast<propUint*>(prop.get())->value();
        break;
    }
    case eObjParam_TRAP_CAST_ONCE:
    {
        m_bCastOnce = dynamic_cast<propBool*>(prop.get())->value();
        break;
    }
    default:
    {
        CWorldObj::applyParam(prop);
        break;
    }
    }
}

QJsonObject CMagicTrap::toJson()
{
    QJsonObject obj;
    QJsonObject world_obj = CWorldObj::toJson();
    obj.insert("World object", world_obj);
    obj.insert("Diplomacy group", QJsonValue::fromVariant(m_diplomacy));
    obj.insert("Spell", m_spell);
    QJsonArray aArea;
    for(auto& actZone : m_aActZone)
    {
        actZone->serializeJsonArray(aArea);
    }
    obj.insert("Area act.", aArea);

    QJsonArray aTarget;
    for(auto& pCast : m_aCastPoint)
    {
        QJsonArray pos;
        pCast->serializeJsonArray(pos);
        aTarget.append(pos);
    }
    obj.insert("Cast Points", aTarget);
    obj.insert("Cast interval", QJsonValue::fromVariant(m_castInterval));
    obj.insert("Is cast once?", m_bCastOnce);
    return obj;
}

void CMagicTrap::loadTexture()
{
    //do nothing. trap has hardcoded texture
}

void CMagicTrap::collectLogicNodes(QList<CNode *> &arrNode)
{
    for(auto& pZone : m_aActZone)
        arrNode.append(pZone);

    for(auto& pCast : m_aCastPoint)
        arrNode.append(pCast);
}

void CMagicTrap::clearLogicSelect()
{
    for(auto& pZone: m_aActZone)
        pZone->setState(ENodeState::eDraw);

    for(auto& pCast : m_aCastPoint)
        pCast->setState(ENodeState::eDraw);
}

bool CMagicTrap::updatePos(QVector3D &pos)
{
    bool bRes(false);
    if(CScene::getInstance()->getMode() == eEditModeLogic)
    {
        bRes = CObjectBase::updatePos(pos);
        return bRes;
    }
    else
    {
        QVector3D offset = pos - m_position;
        QVector3D posN;
        for(auto& castPoint : m_aCastPoint)
        {
            posN = castPoint->position() + offset;
            castPoint->updatePos(posN);
        }
        for(auto& pActZone : m_aActZone)
        {
            posN = pActZone->position() + offset;
            pActZone->updatePos(posN);
        }
        bRes = CObjectBase::updatePos(pos);
    }
    update();
    return bRes;
}

void CMagicTrap::setDrawPosition(QVector3D pos)
{
    m_drawPosition = pos;
    update();
}

int CMagicTrap::getZoneId(CActivationZone *pZone)
{
    return m_aActZone.indexOf(pZone);
}

CActivationZone *CMagicTrap::actZoneById(int zoneId)
{
    return m_aActZone[zoneId];
}

int CMagicTrap::getCastPointId(CTrapCastPoint *pCast)
{
    return m_aCastPoint.indexOf(pCast);
}

CTrapCastPoint *CMagicTrap::castPointById(int pointId)
{
    return m_aCastPoint[pointId];
}

CActivationZone *CMagicTrap::createActZone()
{
    CActivationZone* pZone = new CActivationZone(this);
    m_aActZone.append(pZone);
    QVector3D pos(m_position.x(), m_position.y(), 0.0f);
    pZone->updatePos(pos);
    QObject::connect(pZone, SIGNAL(changeActZone()), this, SLOT(update()));
    update();
    return pZone;
}

void CMagicTrap::deleteLastActZone()
{
    auto pZone = m_aActZone.back();
    delete pZone;
    m_aActZone.pop_back();
    update();
}

CTrapCastPoint *CMagicTrap::createCastPoint()
{
    CTrapCastPoint* pCast = new CTrapCastPoint(this);
    m_aCastPoint.append(pCast);
    QVector3D pos(m_position.x(), m_position.y(), 0.0f);
    pCast->updatePos(pos);
    QObject::connect(pCast, SIGNAL(changeCastPoint()), this, SLOT(update()));
    update();
    return pCast;
}

void CMagicTrap::deleteLastCastPoint()
{
    auto pCast = m_aCastPoint.back();
    delete pCast;
    m_aCastPoint.pop_back();
    update();
}

bool CMagicTrap::isOperationAxisAllow(EOperationAxisType type)
{
    return type == EOperationAxisType::eMove;
}

void CMagicTrap::update()
{
    for(auto& pZone : m_aActZone)
        pZone->update();

    //recalc points beetwen trap and activation zones
    m_aDrawingLine.clear();
    if(m_aActZone.empty())
        return;

    m_aDrawingLine.append(m_drawPosition); // self position
    //m_aDrawingLine.append(m_position); //todo: use m_drawPosition if update correctly when reading mob file
    for(auto& pZone: m_aActZone)
    {
        if(pZone->isMarkDeleted()) continue;
        m_aDrawingLine.append(pZone->drawPosition());
    }
    for(auto& pCast: m_aCastPoint)
    {
        if(pCast->isMarkDeleted()) continue;
        m_aDrawingLine.append(pCast->drawPosition());
    }

    // Generate VBOs and transfer data
    m_vertexBuf.create();
    m_vertexBuf.bind();
    m_vertexBuf.allocate(m_aDrawingLine.data(), m_aDrawingLine.count() * int(sizeof(QVector3D)));
    m_vertexBuf.release();

    QVector<ushort> aInd;
    for(int i(0); i<(m_aActZone.size() + m_aCastPoint.size()); ++i)
    {
        aInd.append(0);
        aInd.append(ushort(i+1));
    }

    m_indexBuf.create();
    m_indexBuf.bind();
    m_indexBuf.allocate(aInd.data(), aInd.count() * int(sizeof(ushort)));
    m_indexBuf.release();
}

CActivationZone::CActivationZone(CMagicTrap* pTrap):
    m_indexBuf(QOpenGLBuffer::IndexBuffer)
  ,m_radius(2.0f)
  ,m_pParent(pTrap)
{
    updateFigure(CObjectList::getInstance()->getFigure("trapZone"));
    setTexture(CTextureList::getInstance()->texture("trapZone"));
}

CActivationZone::CActivationZone(CMagicTrap* pTrap, const CActivationZone &zone):
    CObjectBase(zone)
  ,m_radius(zone.m_radius)
  ,m_pParent(pTrap)
{
    updateFigure(CObjectList::getInstance()->getFigure("trapZone"));
    setTexture(CTextureList::getInstance()->texture("trapZone"));
    update();
}

CActivationZone::~CActivationZone()
{
    m_vertexBuf.destroy();
    m_indexBuf.destroy();
}

void CActivationZone::collectlogicParams(QList<QSharedPointer<IPropertyBase>>& aProp, ENodeType paramType)
{
    auto comm = paramType & eTrapActZone;
    if (comm != eTrapActZone)
        return;

    propFloat posX(eObjParam_POSITION_X, m_position.x());
    util::addParam(aProp, &posX);
    propFloat posY(eObjParam_POSITION_Y, m_position.y());
    util::addParam(aProp, &posY);
    propFloat posZ(eObjParam_POSITION_Z, m_position.z());
    util::addParam(aProp, &posZ);
    propFloat rad(eObjParam_TRAP_AREA_RADIUS, m_radius);
    util::addParam(aProp, &rad);;
}

void CActivationZone::getLogicParam(QSharedPointer<IPropertyBase>& prop, EObjParam propType)
{
    switch (propType) {
    case eObjParam_POSITION:
    {
        prop.reset(new prop3D(propType, m_position));
        break;
    }
    case eObjParam_POSITION_X:
    {
        prop.reset(new propFloat(propType, m_position.x()));
        break;
    }
    case eObjParam_POSITION_Y:
    {
        prop.reset(new propFloat(propType, m_position.y()));
        break;
    }
    case eObjParam_POSITION_Z:
    {
        prop.reset(new propFloat(propType, m_position.z()));
        break;
    }
    case eObjParam_TRAP_AREA_RADIUS:
    {
        prop.reset(new propFloat(propType, m_radius));
        break;
    }
    default:
    {
        Q_ASSERT(false);
        break;
    }
    }
    return;
}

void CActivationZone::applyLogicParam(const QSharedPointer<IPropertyBase>& prop)
{
    switch (prop->type()){
    case eObjParam_POSITION:
    {
        QVector3D pos = dynamic_cast<const prop3D*>(prop.get())->value();
        updatePos(pos);
        emit changeActZone();
        break;
    }
    case eObjParam_POSITION_X:
    {
        float val = dynamic_cast<const propFloat*>(prop.get())->value();
        QVector3D pos(position());
        pos.setX(val);
        updatePos(pos);
        emit changeActZone();
        break;
    }
    case eObjParam_POSITION_Y:
    {
        float val = dynamic_cast<const propFloat*>(prop.get())->value();
        QVector3D pos(position());
        pos.setY(val);
        updatePos(pos);
        emit changeActZone();
        break;
    }
    case eObjParam_POSITION_Z:
    {
        float val = dynamic_cast<const propFloat*>(prop.get())->value();
        QVector3D pos(position());
        pos.setZ(val);
        updatePos(pos);
        emit changeActZone();
        break;
    }
    case eObjParam_TRAP_AREA_RADIUS:
    {
        m_radius = dynamic_cast<const propFloat*>(prop.get())->value();
        update();
        break;
    }
    default:
        Q_ASSERT(false);
    }
}

uint CActivationZone::deserialize(util::CMobParser &parser)
{
    uint readByte(0);
    QVector2D plot;
    readByte += parser.readPlot2D(plot);
    QVector3D pos(plot.x(), plot.y(), 0.0f);
    updatePos(pos);
    readByte += parser.readFloat(m_radius);

    return readByte;
}

uint CActivationZone::serialize(util::CMobParser &parser)
{
    uint writeByte(0);
    QVector2D plot(m_position.x(), m_position.y());
    writeByte += parser.writePlot2D(plot);
    writeByte += parser.writeFloat(m_radius);
    return writeByte;
}

void CActivationZone::deSerializeJson(QJsonObject data)
{
    m_radius = data["Radius"].toVariant().toUInt();

    QVector3D pos;
    QJsonArray arrPos = data["Point to"].toArray();
    if (arrPos.size() == 2)
        pos = QVector3D(arrPos[0].toVariant().toFloat(), arrPos[1].toVariant().toFloat(), 0.0f);

    updatePos(pos);
}

void CActivationZone::serializeJsonArray(QJsonArray& array)
{
    QJsonObject areaObj;
    QJsonArray pos;
    pos.append(QJsonValue::fromVariant(m_position.x()));
    pos.append(QJsonValue::fromVariant(m_position.y()));
    areaObj.insert("Point to", pos);
    areaObj.insert("Radius", QJsonValue::fromVariant(m_radius));
    array.append(areaObj);
}

//SArea CActivationZone::getArea()
//{
//    SArea area;
//    area.m_pointTo = QVector2D(m_position.x(), m_position.y());
//    area.m_radius = m_radius;
//    return area;
//}

void CActivationZone::draw(bool isActive, QOpenGLShaderProgram *program)
{
    //draw act point here
    if(m_aDrawPoint.empty())
        return;

    if(isMarkDeleted())
        return;

    //todo: for selected objects ALWAYS draw logic
    COptBool* pOpt = dynamic_cast<COptBool*>(CObjectList::getInstance()->settings()->opt("drawLogic"));
    if (nullptr == pOpt)
        return;

    if (!pOpt->value() && m_pParent->nodeState() != ENodeState::eSelect && CScene::getInstance()->getMode() != eEditModeLogic)
        return;

    CObjectBase::draw(isActive, program);
    glDisable(GL_DEPTH_TEST);
    //todo: draw help radius
    QMatrix4x4 matrix;
    matrix.setToIdentity();
    program->setUniformValue("u_modelMmatrix", matrix);
    program->setUniformValue("customColor", QVector4D(0.2f, 1.0f, 0.2f, 1.0f));

    int offset(0);
    // Tell OpenGL which VBOs to use
    m_vertexBuf.bind();
    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, int(sizeof(QVector3D)));

    // Draw cube geometry using indices from VBO 1
    m_indexBuf.bind();
    glLineWidth(2);

    glDrawElements(GL_LINE_STRIP, m_aDrawPoint.count(), GL_UNSIGNED_SHORT, nullptr);

    program->setUniformValue("customColor", QVector4D(0.0, 0.0, 0.0, 0.0));

    glEnable(GL_DEPTH_TEST);
}

void CActivationZone::update()
{
    m_aDrawPoint.clear();
    QVector<QVector3D> aCircle;
    QVector3D centr(m_position);
    centr.setZ(0.0f);
    util::getCirclePoint(aCircle, centr, double(m_radius), 40);
    m_aDrawPoint.append(aCircle);

    if(!m_aDrawPoint.isEmpty())
    {
        CLandscape::getInstance()->projectPt(m_aDrawPoint);

        // Generate VBOs and transfer data
        m_vertexBuf.create();
        m_vertexBuf.bind();
        m_vertexBuf.allocate(m_aDrawPoint.data(), m_aDrawPoint.count() * int(sizeof(QVector3D)));
        m_vertexBuf.release();

        QVector<ushort> aInd;
        for (ushort i(0); i<m_aDrawPoint.size(); ++i)
            aInd.append(i);

        m_indexBuf.create();
        m_indexBuf.bind();
        m_indexBuf.allocate(aInd.data(), aInd.count() * int(sizeof(ushort)));
        m_indexBuf.release();
    }
}

bool CActivationZone::updatePos(QVector3D &pos)
{
    bool res = CObjectBase::updatePos(pos);
    update();
    emit changeActZone();
    return res;
}

void CActivationZone::markAsDeleted(bool bDeleted)
{
    CObjectBase::markAsDeleted(bDeleted);
    setState(ENodeState::eDraw); // clear select for undo-redo
    emit changeActZone();
}

bool CActivationZone::isOperationAxisAllow(EOperationAxisType type)
{
    return type == EOperationAxisType::eMove;
}

CTrapCastPoint::CTrapCastPoint(CMagicTrap *pTrap):
    m_pParent(pTrap)
{
    updateFigure(CObjectList::getInstance()->getFigure("trapCast"));
    setTexture(CTextureList::getInstance()->texture("trapCast"));
}

CTrapCastPoint::CTrapCastPoint(CMagicTrap *pTrap, const CTrapCastPoint &pCast):
    CObjectBase(pCast)
  ,m_pParent(pTrap)
{
    updateFigure(CObjectList::getInstance()->getFigure("trapCast"));
    setTexture(CTextureList::getInstance()->texture("trapCast"));
}

CTrapCastPoint::~CTrapCastPoint()
{
}

void CTrapCastPoint::collectlogicParams(QList<QSharedPointer<IPropertyBase>>& aProp, ENodeType paramType)
{
    auto comm = paramType & eTrapCastPoint;
    if (comm != eTrapCastPoint)
        return;

    propFloat posX(eObjParam_POSITION_X, m_position.x());
    util::addParam(aProp, &posX);
    propFloat posY(eObjParam_POSITION_Y, m_position.y());
    util::addParam(aProp, &posY);
    propFloat posZ(eObjParam_POSITION_Z, m_position.z());
    util::addParam(aProp, &posZ);
}

void CTrapCastPoint::applyLogicParam(const QSharedPointer<IPropertyBase>& prop)
{
    switch (prop->type()){
    case eObjParam_POSITION:
    {
        QVector3D pos = dynamic_cast<const prop3D*>(prop.get())->value();
        updatePos(pos);
        emit changeCastPoint();
        break;
    }
    case eObjParam_POSITION_X:
    {
        float val = dynamic_cast<const propFloat*>(prop.get())->value();
        QVector3D pos(position());
        pos.setX(val);
        updatePos(pos);
        emit changeCastPoint();
        break;
    }
    case eObjParam_POSITION_Y:
    {
        float val = dynamic_cast<const propFloat*>(prop.get())->value();
        QVector3D pos(position());
        pos.setY(val);
        updatePos(pos);
        emit changeCastPoint();
        break;
    }
    case eObjParam_POSITION_Z:
    {
        float val = dynamic_cast<const propFloat*>(prop.get())->value();
        QVector3D pos(position());
        pos.setZ(val);
        updatePos(pos);
        emit changeCastPoint();
        break;
    }
    default:
        Q_ASSERT(false);
    }
}

void CTrapCastPoint::getLogicParam(QSharedPointer<IPropertyBase>& prop, EObjParam propType)
{
    switch (propType) {
    case eObjParam_POSITION:
    {
        prop.reset(new prop3D(propType, m_position));
        break;
    }
    case eObjParam_POSITION_X:
    {
        prop.reset(new propFloat(propType, m_position.x()));
        break;
    }
    case eObjParam_POSITION_Y:
    {
        prop.reset(new propFloat(propType, m_position.y()));
        break;
    }
    case eObjParam_POSITION_Z:
    {
        prop.reset(new propFloat(propType, m_position.z()));
        break;
    }
    default:
    {
        Q_ASSERT(false);
        break;
    }
    }
    return;
}

uint CTrapCastPoint::deserialize(util::CMobParser &parser)
{
    uint writeBytes(0);
    QVector2D plot;
    writeBytes += parser.readPlot2D(plot);
    QVector3D pos(plot.x(), plot.y(), 0.0f);
    updatePos(pos);
    return writeBytes;
}

uint CTrapCastPoint::serialize(util::CMobParser &parser)
{
    uint writeBytes(0);
    QVector2D plot(m_position.x(), m_position.y());
    writeBytes += parser.writePlot2D(plot);
    return writeBytes;
}

void CTrapCastPoint::deSerializeJsonArray(QJsonArray data)
{
    Q_ASSERT(data.size() == 2);
    QVector3D pos(data[0].toVariant().toFloat(), data[1].toVariant().toFloat(), 0.0f);
    updatePos(pos);
}

void CTrapCastPoint::serializeJsonArray(QJsonArray &obj)
{
    obj.append(QJsonValue::fromVariant(m_position.x()));
    obj.append(QJsonValue::fromVariant(m_position.y()));
}

void CTrapCastPoint::draw(bool isActive, QOpenGLShaderProgram *program)
{
    COptBool* pOpt = dynamic_cast<COptBool*>(CObjectList::getInstance()->settings()->opt("drawLogic"));
    if (nullptr == pOpt)
        return;

    if (!pOpt->value() && m_pParent->nodeState() != ENodeState::eSelect && CScene::getInstance()->getMode() != eEditModeLogic)
        return;

    CObjectBase::draw(isActive, program);
}

bool CTrapCastPoint::updatePos(QVector3D &pos)
{
    bool bRes = CObjectBase::updatePos(pos);
    emit changeCastPoint();
    return bRes;
}

void CTrapCastPoint::markAsDeleted(bool bDeleted)
{
    CObjectBase::markAsDeleted(bDeleted);
    setState(ENodeState::eDraw); // clear select for undo-redo
    emit changeCastPoint();
}

bool CTrapCastPoint::isOperationAxisAllow(EOperationAxisType type)
{
    return type==EOperationAxisType::eMove;
}
