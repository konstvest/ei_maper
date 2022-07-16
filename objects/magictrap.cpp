#include <QJsonArray>
#include "magictrap.h"
#include "resourcemanager.h"

CMagicTrap::CMagicTrap():
    m_diplomacy(0)
    ,m_castInterval(0)
    ,m_bCastOnce(false)
{
    m_type = 59;
    updateFigure(CObjectList::getInstance()->getFigure("trap"));
    setTexture(CTextureList::getInstance()->texture("trap"));
}

CMagicTrap::CMagicTrap(const CMagicTrap &trap):
    CWorldObj(trap)
{
    m_diplomacy = trap.m_diplomacy;
    m_spell = trap.m_spell;
    m_aArea = trap.m_aArea;
    m_aTarget = trap.m_aTarget;
    m_castInterval = trap.m_castInterval;
    m_bCastOnce = trap.m_bCastOnce;
}

CMagicTrap::CMagicTrap(QJsonObject data):
    CWorldObj(data["World object"].toObject())
{
    m_diplomacy = data["Diplomacy group"].toVariant().toUInt();
    m_spell = data["Spell"].toString();
    QJsonArray aArea = data["Area"].toArray();
    for(auto it=aArea.begin(); it<aArea.end(); ++it)
    {
        QJsonObject obj = it->toObject();
        SArea area;
        area.m_radius = obj["Radius"].toVariant().toUInt();

        QJsonArray arrPos = obj["Point to"].toArray();
        if (arrPos.size() == 2)
            area.m_pointTo = QVector2D(arrPos[0].toVariant().toFloat(), arrPos[1].toVariant().toFloat());

        m_aArea.append(area);
    }


    QJsonArray aTarget = data["Targets(Points?!)"].toArray();
    for (auto it=aTarget.begin(); it<aTarget.end(); ++it)
    {
        QJsonArray aPos = it->toArray();
        m_aTarget.append(QVector2D(aPos[0].toVariant().toFloat(), aPos[1].toVariant().toFloat()));
    }

    m_castInterval = data["Cast interval"].toVariant().toUInt();
    m_bCastOnce = data["Is cast once?"].toBool();
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
            auto a = parser.nodeLen();
            auto b = parser.readAreaArray(m_aArea);
            if(a!=b)
            {
                //MapEd often fuck this zone
                //Q_ASSERT(a == b);
                qDebug() << "MapEd fucked MT_AREAS zone";
            }
            readByte += b;
            //todo: check len
        }
        else if(parser.isNextTag("MT_TARGETS"))
        {

            readByte += parser.readHeader();
            auto a = parser.nodeLen();
            auto b = parser.readPlot2DArray(m_aTarget);
            if(a!=b)
            {
                //MapEd often fuck this zone
                //Q_ASSERT(a == b);
                qDebug() << "MapEd fucked MT_TARGETS zone";
            }
            readByte += b;
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
    return readByte;
}

void CMagicTrap::serializeJson(QJsonObject& obj)
{
    CWorldObj::serializeJson(obj);
    obj.insert("Diplomacy group", QJsonValue::fromVariant(m_diplomacy));
    obj.insert("Spell", m_spell);
    QJsonArray aArea;
    for(auto& area : m_aArea)
    {
        QJsonObject areaObj;
        QJsonArray pos;
        pos.append(QJsonValue::fromVariant(area.m_pointTo.y()));
        pos.append(QJsonValue::fromVariant(area.m_pointTo.x()));
        areaObj.insert("Point to", pos);
        areaObj.insert("Radius", QJsonValue::fromVariant(area.m_radius));
        aArea.append(areaObj);
    }
    obj.insert("Area", aArea);

    QJsonArray aTarget;
    for (auto& target: m_aTarget)
    {
        QJsonArray pos;
        pos.append(QJsonValue::fromVariant(target.x()));
        pos.append(QJsonValue::fromVariant(target.y()));
        aTarget.append(pos);
    }
    obj.insert("Targets(Points?!)", aTarget);
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

    writeByte += parser.startSection("MT_AREAS");
    writeByte += parser.writeAreaArray(m_aArea);
    parser.endSection(); //MT_AREAS

    writeByte += parser.startSection("MT_TARGETS");
    writeByte += parser.writePlot2DArray(m_aTarget);
    parser.endSection(); //MT_TARGETS

    writeByte += CWorldObj::serialize(parser);


    parser.endSection(); //MAGIC_TRAP
    return writeByte;
}

void CMagicTrap::collectParams(QMap<EObjParam, QString> &aParam, ENodeType paramType)
{
    CWorldObj::collectParams(aParam, paramType);

    auto comm = paramType & eMagicTrap;
    if (comm != eMagicTrap)
        return;

    addParam(aParam, eObjParam_TRAP_DIPLOMACY, QString::number(m_diplomacy));
    addParam(aParam, eObjParam_TRAP_SPELL, m_spell);
    addParam(aParam, eObjParam_TRAP_AREAS, util::makeString(m_aArea));
    addParam(aParam, eObjParam_TRAP_TARGETS, util::makeString(m_aTarget));
    addParam(aParam, eObjParam_TRAP_CAST_INTERVAL, QString::number(m_castInterval));
    addParam(aParam, eObjParam_TRAP_CAST_ONCE, util::makeString(m_bCastOnce));

    aParam.remove(eObjParam_TEMPLATE); //dont allow change
    aParam.remove(eObjParam_PRIM_TXTR); //dont allow change

}

void CMagicTrap::applyParam(EObjParam param, const QString &value)
{
    switch (param) {
    case eObjParam_TRAP_DIPLOMACY:
    {
        m_diplomacy = char(value.toInt());
        break;
    }
    case eObjParam_TRAP_SPELL:
    {
        m_spell = value;
        break;
    }
    case eObjParam_TRAP_AREAS:
    {
        m_aArea = util::vecAreaFromString(value);
        break;
    }
    case eObjParam_TRAP_TARGETS:
    {
        m_aTarget = util::vecTargetFromString(value);
        break;
    }
    case eObjParam_TRAP_CAST_INTERVAL:
    {
        m_castInterval = value.toInt();
        break;
    }
    case eObjParam_TRAP_CAST_ONCE:
    {
        m_bCastOnce = util::boolFromString(value);
        break;
    }
    default:
    {
        CWorldObj::applyParam(param, value);
        break;
    }
    }
}

QString CMagicTrap::getParam(EObjParam param)
{
    QString value;
    switch (param) {
    case eObjParam_TRAP_DIPLOMACY:
    {
        value = QString::number(m_diplomacy);
        break;
    }
    case eObjParam_TRAP_SPELL:
    {
        value = m_spell;
        break;
    }
    case eObjParam_TRAP_AREAS:
    {
        value = util::makeString(m_aArea);
        break;
    }
    case eObjParam_TRAP_TARGETS:
    {
        value = util::makeString(m_aTarget);
        break;
    }
    case eObjParam_TRAP_CAST_INTERVAL:
    {
        value = QString::number(m_castInterval);
        break;
    }
    case eObjParam_TRAP_CAST_ONCE:
    {
        value = util::makeString(m_bCastOnce);
        break;
    }
    default:
    {
        value = CWorldObj::getParam(param);
        break;
    }
    }
    return value;
}

QJsonObject CMagicTrap::toJson()
{
    QJsonObject obj;
    QJsonObject world_obj = CWorldObj::toJson();
    obj.insert("World object", world_obj);
    obj.insert("Diplomacy group", QJsonValue::fromVariant(m_diplomacy));
    obj.insert("Spell", m_spell);
    QJsonArray aArea;
    for(auto& area : m_aArea)
    {
        QJsonObject areaObj;
        QJsonArray pos;
        pos.append(QJsonValue::fromVariant(area.m_pointTo.y()));
        pos.append(QJsonValue::fromVariant(area.m_pointTo.x()));
        areaObj.insert("Point to", pos);
        areaObj.insert("Radius", QJsonValue::fromVariant(area.m_radius));
        aArea.append(areaObj);
    }
    obj.insert("Area", aArea);

    QJsonArray aTarget;
    for (auto& target: m_aTarget)
    {
        QJsonArray pos;
        pos.append(QJsonValue::fromVariant(target.x()));
        pos.append(QJsonValue::fromVariant(target.y()));
        aTarget.append(pos);
    }
    obj.insert("Targets(Points?!)", aTarget);
    obj.insert("Cast interval", QJsonValue::fromVariant(m_castInterval));
    obj.insert("Is cast once?", m_bCastOnce);
    return obj;
}
