#include <QJsonArray>
#include "lever.h"

CLever::CLever():
    m_curState(0)
    ,m_totalState(2)
    ,m_bCycled(false)
    ,m_bCastOnce(false)
    ,m_typeOpen(0)
    ,m_keyID(0)
    ,m_handsSleight(0)
    ,m_bDoor(false)
    ,m_bRecalcGraph(false)
{
    m_type = 60;
}

CLever::CLever(const CLever &lever):
    CWorldObj(lever)
{
    m_curState = lever.m_curState;
    m_totalState = lever.m_totalState;
    m_bCycled = lever.m_bCycled;
    m_bCastOnce = lever.m_bCastOnce;
    m_typeOpen = lever.m_typeOpen;
    m_keyID = lever.m_keyID;
    m_handsSleight = lever.m_handsSleight;
    m_bDoor = lever.m_bDoor;
    m_bRecalcGraph = lever.m_bRecalcGraph;
}

CLever::CLever(QJsonObject data):
    CWorldObj(data["World object"].toObject())
{

    m_curState = (char)data["State"].toInt();
    m_totalState = (char)data["State numbers"].toInt();
    m_bCycled = data["Is cycled?"].toBool();
    m_bCastOnce = data["Is cast once?"].toBool();
    QJsonArray aStat = data["Science stats"].toArray();
    if (aStat.size() == 3)
    {
        m_typeOpen = aStat[0].toVariant().toUInt();
        m_keyID = aStat[1].toVariant().toUInt();
        m_handsSleight = aStat[2].toVariant().toUInt();
    }

    m_bDoor = data["Is door?"].toBool();
    m_bRecalcGraph = data["Is recalculate graph?"].toBool();
}

uint CLever::deserialize(util::CMobParser& parser)
{
    uint readByte(0);
    while(true)
    {
        if(parser.isNextTag("LEVER_SCIENCE_STATS"))
        {
            Q_ASSERT("unknow_tag" && false);
            readByte += parser.skipTag();//, eNull};
        }
        else if(parser.isNextTag("LEVER_CUR_STATE"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readByte(m_curState);
        }
        else if(parser.isNextTag("LEVER_TOTAL_STATE"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readByte(m_totalState);
        }
        else if(parser.isNextTag("LEVER_IS_CYCLED"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readBool(m_bCycled);
        }
        else if(parser.isNextTag("LEVER_CAST_ONCE"))
        { //TODO: not found this field in zone8.mob, delete/skip this?!
            Q_ASSERT("LEVER_CAST_ONCE" && false);
            readByte += parser.skipHeader();
            readByte += parser.readBool(m_bCastOnce);
        }
        else if(parser.isNextTag("LEVER_SCIENCE_STATS_NEW"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readDword(m_typeOpen); // type open (0 - lever disabled, 1 - enabled, 5 - need hands sleight, 8 - need key)
            readByte += parser.readDword(m_keyID); // key ID for open
            readByte += parser.readDword(m_handsSleight); // sleight
        }
        else if(parser.isNextTag("LEVER_IS_DOOR"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readBool(m_bDoor);
        }
        else if(parser.isNextTag("LEVER_RECALC_GRAPH"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readBool(m_bRecalcGraph);
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
    Q_ASSERT(m_type==60);
    return readByte;
}

void CLever::serializeJson(QJsonObject& obj)
{
    CWorldObj::serializeJson(obj);
    obj.insert("State", QJsonValue::fromVariant(int(m_curState)));
    obj.insert("State numbers", QJsonValue::fromVariant(int(m_totalState)));
    obj.insert("Is cycled?", m_bCycled);
    obj.insert("Is cast once?", m_bCastOnce);
    QJsonArray aStat;
    aStat.append(QJsonValue::fromVariant(m_typeOpen));
    aStat.append(QJsonValue::fromVariant(m_keyID));
    aStat.append(QJsonValue::fromVariant(m_handsSleight));
    obj.insert("Science stats", aStat);
    obj.insert("Is door?", m_bDoor);
    obj.insert("Is recalculate graph?", m_bRecalcGraph);
}

uint CLever::serialize(util::CMobParser &parser)
{
    uint writeByte(0);
    writeByte += parser.startSection("LEVER");

    writeByte += parser.startSection("LEVER_SCIENCE_STATS_NEW");
    writeByte += parser.writeDword(m_typeOpen);
    writeByte += parser.writeDword(m_keyID);
    writeByte += parser.writeDword(m_handsSleight);
    parser.endSection(); //LEVER_SCIENCE_STATS_NEW

    writeByte += parser.startSection("LEVER_CUR_STATE");
    writeByte += parser.writeByte(m_curState);
    parser.endSection(); //LEVER_CUR_STATE

    writeByte += parser.startSection("LEVER_TOTAL_STATE");
    writeByte += parser.writeByte(m_totalState);
    parser.endSection(); //LEVER_TOTAL_STATE

    writeByte += parser.startSection("LEVER_IS_CYCLED");
    writeByte += parser.writeBool(m_bCycled);
    parser.endSection(); //LEVER_IS_CYCLED

    writeByte += parser.startSection("LEVER_IS_DOOR");
    writeByte += parser.writeBool(m_bDoor);
    parser.endSection(); //LEVER_IS_DOOR

    writeByte += parser.startSection("LEVER_RECALC_GRAPH");
    writeByte += parser.writeBool(m_bRecalcGraph);
    parser.endSection(); //LEVER_RECALC_GRAPH

    writeByte += CWorldObj::serialize(parser);

    parser.endSection(); //LEVER
    return writeByte;
}

void CLever::collectParams(QMap<EObjParam, QString> &aParam, ENodeType paramType)
{
    CWorldObj::collectParams(aParam, paramType);

    auto comm = paramType & eLever;
    if (comm != eLever)
        return;

    //addParam(aParam, eObjParam_LEVER_SCIENCE_STATS_NEW, util::makeString(m_stat));
    util::addParam(aParam, eObjParam_LEVER_SCIENCE_STATS_Type_Open, QString::number(m_typeOpen));
    util::addParam(aParam, eObjParam_LEVER_SCIENCE_STATS_Key_ID, QString::number(m_keyID));
    util::addParam(aParam, eObjParam_LEVER_SCIENCE_STATS_Hands_Sleight, QString::number(m_handsSleight));
    util::addParam(aParam, eObjParam_LEVER_CUR_STATE, QString::number(m_curState));
    util::addParam(aParam, eObjParam_LEVER_TOTAL_STATE, QString::number(m_totalState));
    util::addParam(aParam, eObjParam_LEVER_IS_CYCLED, util::makeString(m_bCycled));
    util::addParam(aParam, eObjParam_LEVER_IS_DOOR, util::makeString(m_bDoor));
    util::addParam(aParam, eObjParam_LEVER_RECALC_GRAPH, util::makeString(m_bRecalcGraph));
}

void CLever::applyParam(EObjParam param, const QString &value)
{
    switch (param) {
    case eObjParam_LEVER_SCIENCE_STATS_Type_Open:
    {
        m_typeOpen = value.toUInt();
        break;
    }
    case eObjParam_LEVER_SCIENCE_STATS_Key_ID:
    {
        m_keyID = value.toUInt();
        break;
    }
    case eObjParam_LEVER_SCIENCE_STATS_Hands_Sleight:
    {
        m_handsSleight = value.toUInt();
        break;
    }
    case eObjParam_LEVER_CUR_STATE:
    {
        m_curState = char(value.toInt());
        break;
    }
    case eObjParam_LEVER_TOTAL_STATE:
    {
        m_totalState = char(value.toInt());
        break;
    }
    case eObjParam_LEVER_IS_CYCLED:
    {
        m_bCycled = util::boolFromString(value);
        break;
    }
    case eObjParam_LEVER_IS_DOOR:
    {
        m_bDoor = util::boolFromString(value);
        break;
    }
    case eObjParam_LEVER_RECALC_GRAPH:
    {
        m_bRecalcGraph = util::boolFromString(value);
        break;
    }
    default:
        CWorldObj::applyParam(param, value);
        break;
    }
}

QString CLever::getParam(EObjParam param)
{
    QString value;
    switch (param) {
    case eObjParam_LEVER_SCIENCE_STATS_Type_Open:
    {
        value = QString::number(m_typeOpen);
        break;
    }
    case eObjParam_LEVER_SCIENCE_STATS_Key_ID:
    {
        value = QString::number(m_keyID);
        break;
    }
    case eObjParam_LEVER_SCIENCE_STATS_Hands_Sleight:
    {
        value = QString::number(m_handsSleight);
        break;
    }
    case eObjParam_LEVER_CUR_STATE:
    {
        value = QString::number(m_curState);
        break;
    }
    case eObjParam_LEVER_TOTAL_STATE:
    {
        value = QString::number(m_totalState);
        break;
    }
    case eObjParam_LEVER_IS_CYCLED:
    {
        value = util::makeString(m_bCycled);
        break;
    }
    case eObjParam_LEVER_IS_DOOR:
    {
        value = util::makeString(m_bDoor);
        break;
    }
    case eObjParam_LEVER_RECALC_GRAPH:
    {
        value = util::makeString(m_bRecalcGraph);
        break;
    }
    default:
        value = CWorldObj::getParam(param);
        break;
    }
    return value;
}

QJsonObject CLever::toJson()
{
    QJsonObject obj;
    QJsonObject world_obj = CWorldObj::toJson();
    obj.insert("World object", world_obj);
    obj.insert("State", QJsonValue::fromVariant(int(m_curState)));
    obj.insert("State numbers", QJsonValue::fromVariant(int(m_totalState)));
    obj.insert("Is cycled?", m_bCycled);
    obj.insert("Is cast once?", m_bCastOnce);
    QJsonArray aStat;
    aStat.append(QJsonValue::fromVariant(m_typeOpen));
    aStat.append(QJsonValue::fromVariant(m_keyID));
    aStat.append(QJsonValue::fromVariant(m_handsSleight));
    obj.insert("Science stats", aStat);
    obj.insert("Is door?", m_bDoor);
    obj.insert("Is recalculate graph?", m_bRecalcGraph);
    return obj;
}
