#include <QJsonArray>
#include "magictrap.h"

CMagicTrap::CMagicTrap()
{

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
                //maped fucked zone
                //Q_ASSERT(a == b);
                qDebug() << "maped fucked MT_AREAS zone";
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
                //maped fucked zone
                //Q_ASSERT(a == b);
                qDebug() << "maped fucked MT_TARGETS zone";
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
    m_modelName = "magicTrap.mod";
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
