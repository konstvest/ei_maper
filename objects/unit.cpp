#include <QJsonObject>
#include <QJsonArray>
#include "unit.h"
#include "view.h"
#include "resourcemanager.h"
#include "landscape.h"
#include "settings.h"
#include "log.h"
#include "scene.h"

CUnit::CUnit():
    m_prototypeName("")
    ,m_bImport(false)
{
    m_type = 50; //Human by default
    m_aLogic.clear();
    for(int i(0); i<5; ++i)
    {
        m_aLogic.append(new CLogic(this, i==0));
    }
    m_stat.reset(new SUnitStat());
}

CUnit::CUnit(const CUnit &unit):
    CWorldObj(unit)
{
    m_prototypeName = unit.m_prototypeName;
    m_stat.reset(new SUnitStat(*unit.m_stat.get()));
    m_aQuestItem = unit.m_aQuestItem;
    m_aQuickItem = unit.m_aQuickItem;
    m_aSpell = unit.m_aSpell;
    m_aWeapon = unit.m_aWeapon;
    m_aArmor = unit.m_aArmor;
    m_bImport = unit.m_bImport;
    m_aLogic.clear();
    for(auto& pLogic : unit.m_aLogic)
        m_aLogic.append(new CLogic(this, *pLogic));
}

CUnit::CUnit(QJsonObject data):
    CWorldObj(data["World object"].toObject())
{
    //m_pMob = pMob;
    m_prototypeName = data["Prototype name"].toString();
    m_stat.reset(new SUnitStat(data["Unit stats"].toObject()));
    const auto deSerializeStringList = [&data](QVector<QString>& aList, QString name)
    {
        QJsonArray aItem = data[name].toArray();
        for(auto it=aItem.begin(); it<aItem.end(); ++it)
        {
            aList.append(it->toString());
        }
    };

    deSerializeStringList(m_aQuestItem, "Quest items");
    deSerializeStringList(m_aQuickItem, "Quick items");
    deSerializeStringList(m_aSpell, "Spells");
    deSerializeStringList(m_aWeapon, "Weapons");
    deSerializeStringList(m_aArmor, "Armors");
    m_bImport = data["Is use mob stats?"].toBool();
    QJsonArray arrLogic = data["Logics"].toArray();
    if(arrLogic.size() == 5)
    {
        for(auto it=arrLogic.begin(); it<arrLogic.end(); ++it)
        {
            CLogic* pLogic = new CLogic(this);
            pLogic->deSerializeJson(it->toObject());
            m_aLogic.append(pLogic);
        }
    }

}

CUnit::~CUnit()
{
    for(auto& logic: m_aLogic)
        delete logic;
}

void CUnit::draw(QOpenGLShaderProgram* program)
{
    CObjectBase::draw(program);
    if (m_aLogic.empty() || m_state == ENodeState::eHidden)
        return;

    m_aLogic.front()->draw(program);
}

void CUnit::drawSelect(QOpenGLShaderProgram* program)
{
    CObjectBase::drawSelect(program);
    for(auto& logic: m_aLogic)
    {
        logic->drawSelect(program);
    }
}

uint CUnit::deserialize(util::CMobParser& parser)
{
    uint readByte(0);
    int logicNum(0);
    while(true)
    {
        if(parser.isNextTag("UNIT_R"))
        {
            Q_ASSERT("UNIT_R" && false);
            readByte += parser.skipHeader();
            readByte += parser.readDword(m_type);
        }
        else if(parser.isNextTag("UNIT_PROTOTYPE"))
        {
            readByte += parser.readHeader();
            readByte += parser.readString(m_prototypeName, parser.nodeLen());
        }
        else if(parser.isNextTag("UNIT_ITEMS"))
        {
            Q_ASSERT("UNIT_ITEMS" && false);
        }
        else if(parser.isNextTag("UNIT_STATS"))
        {
            readByte += parser.readHeader();
            readByte += parser.readUnitStats(m_stat, parser.nodeLen());
        }
        else if(parser.isNextTag("UNIT_QUEST_ITEMS"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readStringArray(m_aQuestItem);
        }
        else if(parser.isNextTag("UNIT_QUICK_ITEMS"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readStringArray(m_aQuickItem);
        }
        else if(parser.isNextTag("UNIT_SPELLS"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readStringArray(m_aSpell);
        }
        else if(parser.isNextTag("UNIT_WEAPONS"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readStringArray(m_aWeapon);
        }
        else if(parser.isNextTag("UNIT_ARMORS"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readStringArray(m_aArmor);
        }
        else if(parser.isNextTag("UNIT_NEED_IMPORT"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readBool(m_bImport);
        }
        else if(parser.isNextTag("UNIT_LOGIC"))
        {
            readByte += parser.skipHeader();
            CLogic* pLogic = m_aLogic[logicNum];
            readByte += pLogic->deserialize(parser);
            ++logicNum;
        }
        else
        {
            uint baseByte = CWorldObj::deserialize(parser);
            if(baseByte > 0)
                readByte += baseByte;
            else
            { //debugging branch :)
                auto tag = parser.nextTag();
                Q_UNUSED(tag);
                break;
            }
        }
        //"UNIT_R", eNull};
        //"UNIT_ITEMS", eNull};
    }
    Q_ASSERT(logicNum == 5);
    Q_ASSERT((m_type==50)||(m_type==51)||(m_type==52));
    return readByte;
}

void CUnit::serializeJson(QJsonObject& obj)
{
    CWorldObj::serializeJson(obj);
    obj.insert("Prototype name", m_prototypeName);
    //todo: m_stat
    const auto serializeStringList = [&obj](QVector<QString>& aList, QString name)
    {
        QJsonArray aitem;
        for(auto& item : aList)
            aitem.append(item);
        obj.insert(name, aitem);
    };

    serializeStringList(m_aQuestItem, "Quest items");
    serializeStringList(m_aQuickItem, "Quick items");
    serializeStringList(m_aSpell, "Spells");
    serializeStringList(m_aWeapon, "Weapons");
    serializeStringList(m_aArmor, "Armors");
    obj.insert("Is import (use mob info instead database)?", m_bImport);
    QJsonArray logicArr;
    for(auto& logic: m_aLogic)
    {
        QJsonObject logicObj;
        logic->serializeJson(logicObj);
        logicArr.append(logicObj);
    }

    obj.insert("Logics", logicArr);
}

uint CUnit::serialize(util::CMobParser &parser)
{
    uint writeByte(0);
    writeByte += parser.startSection("UNIT");
//    parser.startSection("UNIT_R");
//    parser.writeDword(m_type);
//    parser.endSection(); //"UNIT_R"

    writeByte += parser.startSection("UNIT_NEED_IMPORT");
    writeByte += parser.writeBool(m_bImport);
    parser.endSection(); //"UNIT_NEED_IMPORT"

    writeByte += parser.startSection("UNIT_PROTOTYPE");
    writeByte += parser.writeString(m_prototypeName);
    parser.endSection();// "UNIT_PROTOTYPE"

    writeByte += parser.startSection("UNIT_ARMORS");
    writeByte += parser.writeStringArray(m_aArmor, "UNIT_ARMORS");
    parser.endSection(); //"UNIT_ARMORS"

    writeByte += parser.startSection("UNIT_WEAPONS");
    writeByte += parser.writeStringArray(m_aWeapon, "UNIT_WEAPONS");
    parser.endSection(); //"UNIT_WEAPONS"

    writeByte += parser.startSection("UNIT_SPELLS");
    writeByte += parser.writeStringArray(m_aSpell, "UNIT_SPELLS");
    parser.endSection(); //"UNIT_SPELLS"

//что-то тут не то. мапед сохраняет чуточку по-другому, сам мапед на мой файл ругается на ошибку.
    writeByte += parser.startSection("UNIT_QUICK_ITEMS");
    writeByte += parser.writeStringArray(m_aQuickItem, "UNIT_QUICK_ITEMS");
    parser.endSection(); //"UNIT_QUICK_ITEMS"

    writeByte += parser.startSection("UNIT_QUEST_ITEMS");
    writeByte += parser.writeStringArray(m_aQuestItem, "UNIT_QUEST_ITEMS");
    parser.endSection(); //"UNIT_QUEST_ITEMS"


    writeByte += parser.startSection("UNIT_STATS");
    writeByte += parser.writeUnitStats(m_stat);
    parser.endSection();// "UNIT_STATS"

    writeByte += CWorldObj::serialize(parser);

    for(const auto& logic : m_aLogic)
    {
        writeByte += logic->serialize(parser);
    }

    parser.endSection(); //UNIT
    return writeByte;
}

//CSettings* CUnit::settings()
//{
//    return m_pMob->view()->settings();
//}

void CUnit::collectParams(QMap<EObjParam, QString> &aParam, ENodeType paramType)
{
    CWorldObj::collectParams(aParam, paramType);

    auto comm = paramType & eUnit;
    if (comm != eUnit)
        return;

    util::addParam(aParam, eObjParam_UNIT_NEED_IMPORT, util::makeString(m_bImport));
    util::addParam(aParam, eObjParam_UNIT_PROTOTYPE, m_prototypeName);
    util::addParam(aParam, eObjParam_UNIT_ARMORS, util::makeString(m_aArmor));
    util::addParam(aParam, eObjParam_UNIT_WEAPONS, util::makeString(m_aWeapon));
    util::addParam(aParam, eObjParam_UNIT_SPELLS, util::makeString(m_aSpell));
    util::addParam(aParam, eObjParam_UNIT_QUICK_ITEMS, util::makeString(m_aQuickItem));
    util::addParam(aParam, eObjParam_UNIT_QUEST_ITEMS, util::makeString(m_aQuestItem));
    util::addParam(aParam, eObjParam_UNIT_STATS, util::makeString(*m_stat.get()));
    util::addParam(aParam, eObjParam_TYPE, QString::number(m_type));
}

void CUnit::collectlogicParams(QMap<EObjParam, QString> &aParam, ENodeType paramType)
{
    auto comm = paramType & eUnit;
    if (comm != eUnit)
        return;

    m_aLogic.front()->collectlogicParams(aParam);
}

void CUnit::applyParam(EObjParam param, const QString &value)
{
    switch (param)
    {
    case eObjParam_UNIT_NEED_IMPORT:
    {
        m_bImport = util::boolFromString(value);
        break;
    }
    case eObjParam_UNIT_PROTOTYPE:
    {
        m_prototypeName = value;
        break;
    }
    case eObjParam_UNIT_ARMORS:
    {
        m_aArmor = util::strListFromString(value);
        break;
    }
    case eObjParam_UNIT_WEAPONS:
    {
        m_aWeapon = util::strListFromString(value);
        break;
    }
    case eObjParam_UNIT_SPELLS:
    {
        m_aSpell = util::strListFromString(value);
        break;
    }
    case eObjParam_UNIT_QUICK_ITEMS:
    {
        m_aQuickItem = util::strListFromString(value);
        break;
    }
    case eObjParam_UNIT_QUEST_ITEMS:
    {
        m_aQuestItem = util::strListFromString(value);
        break;
    }
    case eObjParam_UNIT_STATS:
    {
        m_stat.reset(new SUnitStat(util::unitStatFromString(value)));
        break;
    }
    default:
        CWorldObj::applyParam(param, value);
    }
}

QString CUnit::getParam(EObjParam param)
{
    QString value;
    switch (param)
    {
    case eObjParam_UNIT_NEED_IMPORT:
    {
        value = util::makeString(m_bImport);
        break;
    }
    case eObjParam_UNIT_PROTOTYPE:
    {
        value = m_prototypeName;
        break;
    }
    case eObjParam_UNIT_ARMORS:
    {
        value = util::makeString(m_aArmor);
        break;
    }
    case eObjParam_UNIT_WEAPONS:
    {
        value = util::makeString(m_aWeapon);
        break;
    }
    case eObjParam_UNIT_SPELLS:
    {
        value = util::makeString(m_aSpell);
        break;
    }
    case eObjParam_UNIT_QUICK_ITEMS:
    {
        value = util::makeString(m_aQuickItem);
        break;
    }
    case eObjParam_UNIT_QUEST_ITEMS:
    {
        value = util::makeString(m_aQuestItem);
        break;
    }
    case eObjParam_UNIT_STATS:
    {
        value = util::makeString(*m_stat.get());
        break;
    }
    default:
        value = CWorldObj::getParam(param);
    }
    return value;
}

bool CUnit::updatePos(QVector3D &pos)
{
    QVector3D dir = pos - m_position;
    CObjectBase::updatePos(pos);
    if(m_aLogic.isEmpty())
        return true;

    if(CScene::getInstance()->getMode()==eEditModeObjects)
        m_aLogic.front()->updatePos(dir);
    else
        m_aLogic.front()->update();

    return true;
}

QJsonObject CUnit::toJson()
{
    QJsonObject obj;
    QJsonObject world_obj = CWorldObj::toJson();
    obj.insert("World object", world_obj);
    obj.insert("Prototype name", m_prototypeName);
    obj.insert("Unit stats", m_stat->toJson());
    const auto serializeStringList = [&obj](QVector<QString>& aList, QString name)
    {
        QJsonArray aitem;
        for(auto& item : aList)
            aitem.append(item);
        obj.insert(name, aitem);
    };

    serializeStringList(m_aQuestItem, "Quest items");
    serializeStringList(m_aQuickItem, "Quick items");
    serializeStringList(m_aSpell, "Spells");
    serializeStringList(m_aWeapon, "Weapons");
    serializeStringList(m_aArmor, "Armors");
    obj.insert("Is use mob stats?", m_bImport);
    QJsonArray logicArr;
    for(auto& logic: m_aLogic)
    {
        QJsonObject logicObj;
        logic->serializeJson(logicObj);
        logicArr.append(logicObj);
    }

    obj.insert("Logics", logicArr);
    return obj;
}

void CUnit::collectLogicNodes(QList<CNode *> &arrNode)
{
    m_aLogic.front()->collectPatrolNodes(arrNode);
}

void CUnit::clearLogicSelect()
{
    m_aLogic.front()->clearPatrolSelect();
}

bool CUnit::isChild(CPatrolPoint *pPointIn)
{
    return m_aLogic.front()->isChild(pPointIn);
}

CLogic::CLogic(CUnit* unit, bool bUse):
    m_indexBuf(QOpenGLBuffer::IndexBuffer)
    ,m_bCyclic(false)
    ,m_model(eRadius)
    ,m_guardRadius(10.0f)
    ,m_use(bUse)
    ,m_wait(0.0f)
    ,m_help(10.0f)
    ,m_parent(unit)
{
}

CLogic::CLogic(CUnit* unit, const CLogic &logic):
    m_parent(unit)
{
    m_bCyclic = logic.m_bCyclic;
    m_model = logic.m_model;
    m_guardRadius = logic.m_guardRadius;
    m_guardPlacement = logic.m_guardPlacement;
    m_numAlarm = logic.m_numAlarm;
    m_use = logic.m_use;
    m_wait = logic.m_wait;
    m_alarmCondition = logic.m_alarmCondition;
    m_help = logic.m_help;
    m_alwaysActive = logic.m_alwaysActive;
    m_agressionMode = logic.m_agressionMode;
    for(auto pP: logic.m_aPatrolPt)
        m_aPatrolPt.append(new CPatrolPoint(*pP));

    m_aDrawPoint = logic.m_aDrawPoint;
    update();
    Q_ASSERT(m_parent);
}

CLogic::~CLogic()
{
    m_vertexBuf.destroy();
    m_indexBuf.destroy();
    for(auto& pp: m_aPatrolPt)
        delete pp;
}

void CLogic::draw(QOpenGLShaderProgram* program)
{
    if(!m_use || m_aDrawPoint.empty())
        return;

    //todo: for selected objects ALWAYS draw logic
    COptBool* pOpt = dynamic_cast<COptBool*>(CObjectList::getInstance()->settings()->opt("drawLogic"));
    if (nullptr == pOpt)
        return;

    if (!pOpt->value() && m_parent->nodeState() != ENodeState::eSelect && CScene::getInstance()->getMode() != eEditModeLogic)
        return;

    glDisable(GL_DEPTH_TEST);
    //todo: draw help radius
    QMatrix4x4 matrix;
    matrix.setToIdentity();
    program->setUniformValue("u_modelMmatrix", matrix);

    program->setUniformValue("customColor", QVector4D(0.8f, 0.8f, 0.2f, 1.0f));
    int offset(0);
    // Tell OpenGL which VBOs to use
    m_vertexBuf.bind();
    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, int(sizeof(QVector3D)));

//    offset+=int(sizeof(QVector3D)); // size of vertex position
//    int normLocation = program->attributeLocation("a_normal");
//    program->enableAttributeArray(normLocation);
//    program->setAttributeBuffer(normLocation, GL_FLOAT, offset, 3, int(sizeof(SVertexData)));

//    offset+=int(sizeof(QVector3D)); // size of normal
//    int textureLocation = program->attributeLocation("a_texture");
//    program->enableAttributeArray(textureLocation);
//    program->setAttributeBuffer(textureLocation, GL_FLOAT, offset, 2, int(sizeof(SVertexData)));

    // Draw cube geometry using indices from VBO 1
    m_indexBuf.bind();
    glLineWidth(2);
    glDrawElements(GL_LINE_STRIP, m_aDrawPoint.count(), GL_UNSIGNED_SHORT, nullptr);
    program->setUniformValue("customColor", QVector4D(0.0, 0.0, 0.0, 0.0));

    if (m_model == EBehaviourType::ePath)
        for(auto& pp : m_aPatrolPt)
            pp->draw(program);

    glEnable(GL_DEPTH_TEST);
}

void CLogic::drawSelect(QOpenGLShaderProgram* program)
{
    if(!m_use)
        return;

    COptBool* pOpt = dynamic_cast<COptBool*>(CObjectList::getInstance()->settings()->opt("drawLogic"));
    if (pOpt && !pOpt->value())
        return;

    glDisable(GL_DEPTH_TEST);
    //todo: choose guard behavior
    for(auto& pp : m_aPatrolPt)
        pp->drawSelect(program);

    glEnable(GL_DEPTH_TEST);
}

void CLogic::update()
{
    m_aDrawPoint.clear();
    switch (m_model) {
    case EBehaviourType::eRadius:
    {
        QVector3D centr(m_guardPlacement);
        centr.setZ(.0f);
        util::getCirclePoint(m_aDrawPoint, centr, double(m_guardRadius), 40);
        CLandscape::getInstance()->projectPt(m_aDrawPoint);
        break;
    }
    case EBehaviourType::ePath:
    {
        Q_ASSERT(m_parent);
        QVector3D pos(m_parent->position());
        pos.setZ(0.0f);
        CLandscape::getInstance()->projectPt(pos);
        m_aDrawPoint.append(pos);
        for (auto& pt: m_aPatrolPt)
        {
            pos = pt->position();
            pos.setZ(0.0f);
            CLandscape::getInstance()->projectPt(pos);
            pt->setPos(pos);
            pt->setDrawPosition(pos);
            m_aDrawPoint.append(pos);
            for (auto& look : pt->lookPoint())
            {
                pos = look->position();
                pos.setZ(0.0f);
                CLandscape::getInstance()->projectPt(pos);
                look->setPos(pos);
                look->setDrawPosition(pos);
            }
            pt->update();
        }
        util::splitByLen(m_aDrawPoint, 2.0f);
        break;
    }
    case EBehaviourType::ePlace:
    {
        return; //todo
        //break;
    }
    case EBehaviourType::eBriffing:
    {
        return;// todo
        //break;
    }
    case EBehaviourType::eGuardAlaram:
    {
        //Q_ASSERT("We found it, master" && false);
        ei::log(eLogDebug, "We found it, master");
        return;
    }
    default:
        //Q_ASSERT("unknown behaviour type" && false);
        return; //break;
    }
    // todo: draw lines over the landscape

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

void CLogic::updatePos(QVector3D &dir)
{
    m_guardPlacement+=dir;
    CLandscape::getInstance()->projectPt(m_guardPlacement);
    //m_parent->mob()->view()->land()->projectPosition(this);
    for(auto& patrol : m_aPatrolPt)
    {
        for(auto& look : patrol->lookPoint())
        {
            look->position() += dir;
            //m_parent->mob()->view()->land()->projectPosition(look);
        }
        patrol->position() += dir;
        //m_parent->mob()->view()->land()->projectPosition(patrol);
    }
    update();
}

void CLogic::collectPatrolNodes(QList<CNode *> &arrNode)
{
    CPatrolPoint* pPoint = nullptr;
    foreach(pPoint, m_aPatrolPt)
    {
        arrNode.append(pPoint);
        pPoint->collectLookNodes(arrNode);
    }
}

void CLogic::clearPatrolSelect()
{
    for(auto& pPoint : m_aPatrolPt)
    {
        pPoint->setState(ENodeState::eDraw);
        pPoint->clearLookSelect();
    }
}

void CLogic::collectlogicParams(QMap<EObjParam, QString> &aParam)
{
    util::addParam(aParam, eObjParam_LOGIC_BEHAVIOUR, QString::number(m_model));
    util::addParam(aParam, eObjParam_GUARD_ALARM, QString::number(m_help));
    //if behaviour is radius
    util::addParam(aParam, eObjParam_GUARD_RADIUS, QString::number(m_guardRadius));
    util::addParam(aParam, eObjParam_GUARD_PLACE, util::makeString(m_guardPlacement));
    util::addParam(aParam, eObjParam_AGRESSION_MODE, QString::number(m_agressionMode));
}

bool CLogic::isChild(CPatrolPoint *pPointIn)
{
    CPatrolPoint* pPoint = nullptr;
    foreach(pPoint, m_aPatrolPt)
    {
        if(pPoint == pPointIn)
            return true;
    }
    return false;
}

uint CLogic::deserialize(util::CMobParser& parser)
{
    uint readByte(0);
    while(true)
    {
        if(parser.isNextTag("UNIT_LOGIC_CYCLIC"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readBool(m_bCyclic);
        }
        else if(parser.isNextTag("UNIT_LOGIC_MODEL"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readDword(m_model);
        }
        else if(parser.isNextTag("UNIT_LOGIC_GUARD_R"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readFloat(m_guardRadius);
        }
        else if(parser.isNextTag("UNIT_LOGIC_GUARD_PT"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readPlot(m_guardPlacement);
        }
        else if(parser.isNextTag("UNIT_LOGIC_NALARM"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readByte(m_numAlarm);
        }
        else if(parser.isNextTag("UNIT_LOGIC_USE"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readByte(m_use);
        }
        else if(parser.isNextTag("UNIT_LOGIC_WAIT"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readFloat(m_wait);
        }
        else if(parser.isNextTag("UNIT_LOGIC_ALARM_CONDITION"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readByte(m_alarmCondition);
        }
        else if(parser.isNextTag("UNIT_LOGIC_HELP"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readFloat(m_help);
        }
        else if(parser.isNextTag("UNIT_LOGIC_ALWAYS_ACTIVE"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readByte(m_alwaysActive);
        }
        else if(parser.isNextTag("UNIT_LOGIC_AGRESSION_MODE"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readByte(m_agressionMode);
        }
        else if(parser.isNextTag("GUARD_PT"))
        {
            readByte += parser.skipHeader();
            CPatrolPoint* place = new CPatrolPoint(); //need unit parent?
            //place->attachMob(m_parent->mob());
            QObject::connect(place, SIGNAL(patrolChanges()), this, SLOT(update()));
            readByte += place->deserialize(parser);

            m_aPatrolPt.append(place);
        }
        else
        {
            auto a = parser.nextTag();
            break;
        }
    }
    update();
    return readByte;
}

void CLogic::serializeJson(QJsonObject& obj)
{
    obj.insert("Is cyclyc?", m_bCyclic);
    obj.insert("Model", QJsonValue::fromVariant(m_model));
    obj.insert("Guard raidus", QJsonValue::fromVariant(m_guardRadius));

    QJsonArray aPlacement;
    aPlacement.append(QJsonValue::fromVariant(m_guardPlacement.x()));
    aPlacement.append(QJsonValue::fromVariant(m_guardPlacement.y()));
    aPlacement.append(QJsonValue::fromVariant(m_guardPlacement.z()));

    obj.insert("Guard placement", aPlacement);
    obj.insert("Alarm number", m_numAlarm);
    obj.insert("Is logic use?", m_use);
    obj.insert("Wait", QJsonValue::fromVariant(m_wait));
    obj.insert("Alarm condition", m_alarmCondition);
    obj.insert("Help radius", QJsonValue::fromVariant(m_help));
    obj.insert("Always active", m_alwaysActive);
    obj.insert("Aggression mode", m_agressionMode);

    QJsonArray ppArr;
    for(auto& pp: m_aPatrolPt)
    {
        QJsonObject ppObj;
        pp->serializeJson(ppObj);
        ppArr.append(ppObj);
    }
    obj.insert("Patrol Points", ppArr);
}

void CLogic::deSerializeJson(QJsonObject data)
{
    m_bCyclic = data["Is cyclyc?"].toBool();
    m_model = (EBehaviourType)data["Model"].toVariant().toUInt();
    m_guardRadius = data["Guard raidus"].toVariant().toFloat();

    QJsonArray aPlacement = data["Guard placement"].toArray();
    if(aPlacement.size()==3)
    m_guardPlacement = QVector3D(aPlacement[0].toVariant().toFloat(), aPlacement[1].toVariant().toFloat(), aPlacement[2].toVariant().toFloat());

    m_numAlarm = (char)data["Alarm number"].toInt();
    m_use = (char)data["Is logic use?"].toInt();
    m_wait = data["Wait"].toVariant().toFloat();
    m_alarmCondition = (char)data["Alarm condition"].toInt();
    m_help = data["Help radius"].toVariant().toFloat();
    m_alwaysActive = (char)data["Always active"].toInt();
    m_agressionMode = (char)data["Aggression mode"].toInt();

    QJsonArray arrPP = data["Patrol Points"].toArray();
    for(auto it=arrPP.begin(); it<arrPP.end(); ++it)
    {
        CPatrolPoint* place = new CPatrolPoint(); //need unit parent?
        place->deSerializeJson(it->toObject());
        m_aPatrolPt.append(place);
    }
    update();
}

uint CLogic::serialize(util::CMobParser& parser)
{
    uint writeByte(0);
    writeByte += parser.startSection("UNIT_LOGIC");

    writeByte += parser.startSection("UNIT_LOGIC_ALARM_CONDITION");
    writeByte += parser.writeByte(m_alarmCondition);
    parser.endSection(); //"UNIT_LOGIC_ALARM_CONDITION"

    writeByte += parser.startSection("UNIT_LOGIC_HELP");
    writeByte += parser.writeFloat(m_help);
    parser.endSection(); //"UNIT_LOGIC_HELP"

    writeByte += parser.startSection("UNIT_LOGIC_CYCLIC");
    writeByte += parser.writeBool(m_bCyclic);
    parser.endSection(); //"UNIT_LOGIC_CYCLIC"

    writeByte += parser.startSection("UNIT_LOGIC_AGRESSION_MODE");
    writeByte += parser.writeByte(m_agressionMode);
    parser.endSection();

    writeByte += parser.startSection("UNIT_LOGIC_ALWAYS_ACTIVE");
    writeByte += parser.writeByte(m_alwaysActive);
    parser.endSection(); //"UNIT_LOGIC_ALWAYS_ACTIVE"

    writeByte += parser.startSection("UNIT_LOGIC_MODEL");
    writeByte += parser.writeDword(m_model);
    parser.endSection(); //parser.skipHeader();

    writeByte += parser.startSection("UNIT_LOGIC_GUARD_R");
    writeByte += parser.writeFloat(m_guardRadius);
    parser.endSection(); //"UNIT_LOGIC_GUARD_R"

    writeByte += parser.startSection("UNIT_LOGIC_WAIT");
    writeByte += parser.writeFloat(m_wait);
    parser.endSection(); //"UNIT_LOGIC_WAIT"

    writeByte += parser.startSection("UNIT_LOGIC_GUARD_PT");
    writeByte += parser.writePlot(m_guardPlacement);
    parser.endSection(); //"UNIT_LOGIC_GUARD_PT"

    writeByte += parser.startSection("UNIT_LOGIC_USE");
    writeByte += parser.writeByte(m_use);
    parser.endSection(); //"UNIT_LOGIC_USE"

    writeByte += parser.startSection("UNIT_LOGIC_NALARM");
    writeByte += parser.writeByte(m_numAlarm);
    parser.endSection(); //"UNIT_LOGIC_NALARM"

    CPatrolPoint* pPoint = nullptr;
    foreach(pPoint, m_aPatrolPt)
    {
        writeByte += pPoint->serialize(parser);
    }
    parser.endSection(); //"UNIT_LOGIC"
    return writeByte;
}

CPatrolPoint::CPatrolPoint():
    m_indexBuf(QOpenGLBuffer::IndexBuffer)
{
    CObjectBase::updateFigure(CObjectList::getInstance()->getFigure("point"));
    CObjectBase::setTexture(CTextureList::getInstance()->texture("point"));
}

CPatrolPoint::CPatrolPoint(const CPatrolPoint &patrol):
    CObjectBase(patrol)
    ,m_indexBuf(QOpenGLBuffer::IndexBuffer)
{
    //m_vertexBuf = patrol.m_vertexBuf;
    //m_indexBuf = patrol.m_indexBuf;
    CLookPoint* pPoint = nullptr;
    foreach(pPoint, patrol.m_aLookPt)
        m_aLookPt.append(new CLookPoint(*pPoint));

    m_aDrawingLine = patrol.m_aDrawingLine;
    update();
}


CPatrolPoint::~CPatrolPoint()
{
    m_vertexBuf.destroy();
    m_indexBuf.destroy();
    for(auto& lp : m_aLookPt)
        delete lp;
}

void CPatrolPoint::draw(QOpenGLShaderProgram* program)
{
    CObjectBase::draw(program);

    if(m_aDrawingLine.size() == 0) // do not draw looking point if absent
        return;

    QMatrix4x4 matrix;
    matrix.setToIdentity();
    program->setUniformValue("u_modelMmatrix", matrix);

    program->setUniformValue("customColor", QVector4D(0.12f, 0.4f, 0.8f, 1.0));
    int offset(0);
    // Tell OpenGL which VBOs to use

    m_vertexBuf.bind();
    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, int(sizeof(QVector3D)));

//    offset+=int(sizeof(QVector3D)); // size of vertex position
//    int normLocation = program->attributeLocation("a_normal");
//    program->enableAttributeArray(normLocation);
//    program->setAttributeBuffer(normLocation, GL_FLOAT, offset, 3, int(sizeof(SVertexData)));

//    offset+=int(sizeof(QVector3D)); // size of normal
//    int textureLocation = program->attributeLocation("a_texture");
//    program->enableAttributeArray(textureLocation);
//    program->setAttributeBuffer(textureLocation, GL_FLOAT, offset, 2, int(sizeof(SVertexData)));

    // Draw cube geometry using indices from VBO 1
    m_indexBuf.bind();
    glDrawElements(GL_LINES, (m_aDrawingLine.count()-1)*2, GL_UNSIGNED_SHORT, nullptr);
    program->setUniformValue("customColor", QVector4D(0.0, 0.0, 0.0, 0.0));
    for(auto& look: m_aLookPt)
    {
        look->draw(program);
    }
}

void CPatrolPoint::drawSelect(QOpenGLShaderProgram* program)
{
    CObjectBase::drawSelect(program);
    for(auto& look: m_aLookPt)
    {
        look->drawSelect(program);
    }
}


void CPatrolPoint::update()
{
    m_aDrawingLine.clear();
    if(m_aLookPt.empty())
        return;

//    QVector3D pos(position());
//    pos.setZ(0.0f);
    m_aDrawingLine.append(position()); // self position
    for(auto& lp: m_aLookPt)
    {
        m_aDrawingLine.append(lp->position());
//        pos=lp->position();
//        pos.setZ(0.0f);
//        m_aDrawingLine.append(pos);   //drawing position?!
    }
//    m_pMob->view()->land()->projectPt(m_aDrawingLine);

    // Generate VBOs and transfer data
    m_vertexBuf.create();
    m_vertexBuf.bind();
    m_vertexBuf.allocate(m_aDrawingLine.data(), m_aDrawingLine.count() * int(sizeof(QVector3D)));
    m_vertexBuf.release();

    QVector<ushort> aInd;
    for(int i(0); i<m_aLookPt.size(); ++i)
    {
        aInd.append(0);
        aInd.append(ushort(i+1));
    }

    m_indexBuf.create();
    m_indexBuf.bind();
    m_indexBuf.allocate(aInd.data(), aInd.count() * int(sizeof(ushort)));
    m_indexBuf.release();
}

uint CPatrolPoint::deserialize(util::CMobParser& parser)
{
    uint readByte(0);
    while(true)
    {
        if(parser.isNextTag("GUARD_PT_POSITION"))
        {
            QVector3D pos;
            readByte += parser.skipHeader();
            readByte += parser.readPlot(pos);
            setPos(pos);
            setDrawPosition(pos);
        }
        else if(parser.isNextTag("GUARD_PT_ACTION"))
        {
            Q_ASSERT("GUARD_PT_ACTION" && false);
        }
        else if(parser.isNextTag("ACTION_PT"))
        {
            readByte += parser.skipHeader();
            CLookPoint* pLook = new CLookPoint();
            readByte += pLook->deserialize(parser);
            QObject::connect(pLook, SIGNAL(lookPointChanges()), this, SLOT(update()));
            m_aLookPt.append(pLook);
        }
        else
        {
            auto a = parser.nextTag();
            break;
        }
    }
    return readByte;
}

QString CPatrolPoint::getParam(EObjParam param)
{
    QString value;
    switch (param){
    case eObjParam_POSITION:
    {
        value = util::makeString(m_position);
        break;
    }
    default:
        Q_ASSERT(false);
    }
    return value;
}

void CPatrolPoint::applyParam(EObjParam param, const QString &value)
{
    switch (param){
    case eObjParam_POSITION:
    {
        m_position = util::vec3FromString(value);
        break;
    }
    default:
        Q_ASSERT(false);
    }
    emit patrolChanges();
}

void CPatrolPoint::collectlogicParams(QMap<EObjParam, QString> &aParam, ENodeType paramType)
{
    //CObjectBase::collectParams(aParam, paramType);
    auto comm = paramType & ePatrolPoint;
    if (comm != ePatrolPoint)
        return;

    util::addParam(aParam, eObjParam_POSITION, util::makeString(m_position));
}

void CPatrolPoint::serializeJson(QJsonObject &obj)
{
    QJsonArray lpArr;
    for(auto& lp: m_aLookPt)
    {
        QJsonObject lpObj;
        lp->serializeJson(lpObj);
        lpArr.append(lpObj);
    }
    QJsonArray posArr;
    posArr.append(QJsonValue::fromVariant(position().x()));
    posArr.append(QJsonValue::fromVariant(position().y()));
    posArr.append(QJsonValue::fromVariant(position().z()));
    obj.insert("Position", posArr);
    obj.insert("Looking points", lpArr);
}

void CPatrolPoint::deSerializeJson(QJsonObject data)
{
    QJsonArray arrPos = data["Position"].toArray();
    if(arrPos.size()==3)
    {
        QVector3D pos(arrPos[0].toVariant().toFloat(), arrPos[1].toVariant().toFloat(), arrPos[2].toVariant().toFloat());
        setPos(pos);
        setDrawPosition(pos);
    }
    QJsonArray arrLP = data["Looking points"].toArray();
    for(auto it=arrLP.begin(); it<arrLP.end(); ++it)
    {
        CLookPoint* pLook = new CLookPoint();
        pLook->deSerializeJson(it->toObject());
        m_aLookPt.append(pLook);
    }
}

uint CPatrolPoint::serialize(util::CMobParser &parser)
{
    uint writeByte(0);
    writeByte += parser.startSection("GUARD_PT");

    writeByte += parser.startSection("GUARD_PT_POSITION");
    writeByte += parser.writePlot(position());
    parser.endSection(); //"GUARD_PT_POSITION
    for(const auto& lookP : m_aLookPt)
    {
        writeByte += lookP->serialize(parser);
    }

    parser.endSection(); //"GUARD_PT"
    return writeByte;
}

void CPatrolPoint::collectLookNodes(QList<CNode *> &arrNode)
{
    CLookPoint* pLook = nullptr;
    foreach(pLook, m_aLookPt)
    {
        arrNode.append(pLook);
    }
}

void CPatrolPoint::clearLookSelect()
{
    for(auto pLook : m_aLookPt)
        pLook->setState(ENodeState::eDraw);
}

bool CPatrolPoint::updatePos(QVector3D &pos)
{
    bool res = CObjectBase::updatePos(pos);
    emit patrolChanges();
    return res;
}

CLookPoint::CLookPoint()
{
    CObjectBase::updateFigure(CObjectList::getInstance()->getFigure("view"));
    CObjectBase::setTexture(CTextureList::getInstance()->texture("view"));
}

CLookPoint::CLookPoint(const CLookPoint &look):
    CObjectBase(look)
{
    m_wait = look.m_wait;
    m_turnSpeed = look.m_turnSpeed;
    m_flag = look.m_flag;
}

QString CLookPoint::getParam(EObjParam param)
{
    QString value;
    switch (param){
    case eObjParam_POSITION:
    {
        value = util::makeString(m_position);
        break;
    }
    case eObjParam_VIEW_WAIT:
    {
        value = QString::number(m_wait);
        break;
    }
    case eObjParam_VIEW_TURN_SPEED:
    {
        value = QString::number(m_turnSpeed);
        break;
    }
    default:
        Q_ASSERT(false);
    }
    return value;
}

void CLookPoint::applyParam(EObjParam param, const QString &value)
{
    switch (param){
    case eObjParam_POSITION:
    {
        m_position = util::vec3FromString(value);
        break;
    }
    case eObjParam_VIEW_WAIT:
    {
        m_wait = value.toUInt();
        break;
    }
    case eObjParam_VIEW_TURN_SPEED:
    {
        m_turnSpeed = value.toUInt();
        break;
    }
    default:
        Q_ASSERT(false);
    }
    emit lookPointChanges();
}

void CLookPoint::collectlogicParams(QMap<EObjParam, QString> &aParam, ENodeType paramType)
{
    //CObjectBase::collectParams(aParam, paramType);
    auto comm = paramType & eLookPoint;
    if (comm != eLookPoint)
        return;

    util::addParam(aParam, eObjParam_POSITION, util::makeString(m_position));
    util::addParam(aParam, eObjParam_VIEW_WAIT, QString::number(m_wait)); //TODO: calc to seconds
    util::addParam(aParam, eObjParam_VIEW_TURN_SPEED, QString::number(m_turnSpeed));
}

uint CLookPoint::deserialize(util::CMobParser& parser)
{
    uint readByte(0);
    while(true)
    {
        if(parser.isNextTag("ACTION_PT_LOOK_PT"))
        {
            QVector3D pos;
            readByte += parser.skipHeader();
            readByte += parser.readPlot(pos);
            setPos(pos);
            setDrawPosition(pos);
        }
        else if(parser.isNextTag("ACTION_PT_WAIT_SEG"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readDword(m_wait);
        }
        else if(parser.isNextTag("ACTION_PT_TURN_SPEED"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readDword(m_turnSpeed);
        }
        else if(parser.isNextTag("ACTION_PT_FLAGS"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readByte(m_flag);
        }
        else
        {
            auto a = parser.nextTag();
            break;
        }
    }
    return readByte;

}

void CLookPoint::serializeJson(QJsonObject &obj)
{
    obj.insert("Wait", QJsonValue::fromVariant(m_wait));
    obj.insert("Turn speed", QJsonValue::fromVariant(m_turnSpeed));
    obj.insert("Point flag", m_flag);
    QJsonArray posArr;
    posArr.append(QJsonValue::fromVariant(position().x()));
    posArr.append(QJsonValue::fromVariant(position().y()));
    posArr.append(QJsonValue::fromVariant(position().z()));
    obj.insert("Position", posArr);
}

void CLookPoint::deSerializeJson(QJsonObject data)
{
    m_wait = data["Wait"].toVariant().toUInt();
    m_turnSpeed = data["Turn speed"].toVariant().toUInt();
    m_flag = (char)data["Point flag"].toInt();
    QJsonArray arrPos = data["Position"].toArray();
    if(arrPos.size()==3)
    {
        QVector3D pos(arrPos[0].toVariant().toFloat(), arrPos[1].toVariant().toFloat(), arrPos[2].toVariant().toFloat());
        setPos(pos);
        setDrawPosition(pos);
    }
}

uint CLookPoint::serialize(util::CMobParser &parser)
{
    uint writeByte(0);
    writeByte += parser.startSection("ACTION_PT");
    writeByte += parser.startSection("ACTION_PT_LOOK_PT");
    writeByte += parser.writePlot(position());
    parser.endSection(); //"ACTION_PT_LOOK_PT"

    writeByte += parser.startSection("ACTION_PT_WAIT_SEG");
    writeByte += parser.writeDword(m_wait);
    parser.endSection(); //"ACTION_PT_WAIT_SEG"

    writeByte += parser.startSection("ACTION_PT_TURN_SPEED");
    writeByte += parser.writeDword(m_turnSpeed);
    parser.endSection(); //"ACTION_PT_TURN_SPEED"

    writeByte += parser.startSection("ACTION_PT_FLAGS");
    writeByte += parser.writeByte(m_flag);
    parser.endSection(); //"ACTION_PT_FLAGS"

    parser.endSection(); //"ACTION_PT"
    return writeByte;
}

bool CLookPoint::updatePos(QVector3D &pos)
{
    bool res = CObjectBase::updatePos(pos);
    emit lookPointChanges();
    return res;
}
