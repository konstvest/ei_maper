#include <QJsonArray>
#include "lever.h"

CLever::CLever()
{

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
            readByte += parser.readPlot(m_stat);//, eLeverStats};
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
    aStat.append(QJsonValue::fromVariant(m_stat.x()));
    aStat.append(QJsonValue::fromVariant(m_stat.y()));
    aStat.append(QJsonValue::fromVariant(m_stat.z()));
    obj.insert("Science stats", aStat);
    obj.insert("Is door?", m_bDoor);
    obj.insert("Is recalculate graph?", m_bRecalcGraph);
}
