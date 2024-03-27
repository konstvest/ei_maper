#include <QJsonArray>
#include "torch.h"
#include "log.h"
#include "property.h"

CTorch::CTorch():
    m_power(0.0f)
{
    m_type = 58;
}

CTorch::CTorch(const CTorch &torch):
    CWorldObj(torch)
{
    m_type = 58;
    m_power = torch.m_power;
    m_pointLink = torch.m_pointLink;
    m_sound = torch.m_sound;
}

CTorch::CTorch(QJsonObject data):
    CWorldObj(data["World object"].toObject())
{
    m_type = 58;
    m_power = data["Power"].toVariant().toFloat();
    QJsonArray arrPoint = data["Position"].toArray();
    if(arrPoint.size()==3)
        m_pointLink = QVector3D(arrPoint[0].toVariant().toFloat(), arrPoint[1].toVariant().toFloat(), arrPoint[2].toVariant().toFloat());

    m_sound = data["Sound"].toString();
}

uint CTorch::deserialize(util::CMobParser& parser)
{
    uint readByte(0);
    while(true)
    {
        if(parser.isNextTag("TORCH_STRENGHT"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readFloat(m_power);
        }
        else if(parser.isNextTag("TORCH_PTLINK"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readPlot(m_pointLink);
        }
        else if(parser.isNextTag("TORCH_SOUND"))
        {
            readByte += parser.readHeader();
            readByte += parser.readString(m_sound, parser.nodeLen());
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
    if(m_type!=58)
    {
        ei::log(eLogWarning, "Incorrect object type("+QString::number(m_type)+"), should be 58");
    }
    return readByte;
}

void CTorch::serializeJson(QJsonObject& obj)
{
    CWorldObj::serializeJson(obj);
    obj.insert("Power", QJsonValue::fromVariant(m_power));
    QJsonArray point;
    point.append(QJsonValue::fromVariant(m_pointLink.x()));
    point.append(QJsonValue::fromVariant(m_pointLink.y()));
    point.append(QJsonValue::fromVariant(m_pointLink.z()));
    obj.insert("Position", point);
    obj.insert("Sound", m_sound);
}

uint CTorch::serialize(util::CMobParser &parser)
{
    uint writeByte(0);
    writeByte += parser.startSection("TORCH");

    writeByte += parser.startSection("TORCH_PTLINK");
    writeByte += parser.writePlot(m_pointLink);
    parser.endSection(); //TORCH_PTLINK

    writeByte += parser.startSection("TORCH_STRENGHT");
    writeByte += parser.writeFloat(m_power);
    parser.endSection(); //TORCH_STRENGHT

    writeByte += parser.startSection("TORCH_SOUND");
    writeByte += parser.writeString(m_sound);
    parser.endSection(); //TORCH_SOUND

    writeByte += CWorldObj::serialize(parser);

    parser.endSection(); //TORCH
    return writeByte;
}

void CTorch::collectParams(QList<QSharedPointer<IPropertyBase>>& aProp, ENodeType paramType)
{
    CWorldObj::collectParams(aProp, paramType);
    auto comm = paramType & eTorch;
    if (comm != eTorch)
        return;

    prop3D linkPoint(eObjParam_TORCH_PTLINK, m_pointLink);
    util::addParam(aProp, &linkPoint);
    propFloat strength(eObjParam_TORCH_STRENGHT, m_power);
    util::addParam(aProp, &strength);
    propStr soundName(eObjParam_TORCH_SOUND, m_sound);
    util::addParam(aProp, &soundName);
}

void CTorch::getParam(QSharedPointer<IPropertyBase>& prop, EObjParam propType)
{
    switch (propType) {
    case eObjParam_TORCH_PTLINK:
    {
        prop.reset(new prop3D(propType, m_pointLink));
        break;
    }
    case eObjParam_TORCH_STRENGHT:
    {
        prop.reset(new propFloat(propType, m_power));
        break;
    }
    case eObjParam_TORCH_SOUND:
    {
        prop.reset(new propStr(propType, m_sound));
        break;
    }
    default:
        CWorldObj::getParam(prop, propType);
    }
}

void CTorch::applyParam(const QSharedPointer<IPropertyBase>& prop)
{
    switch (prop->type()) {
    case eObjParam_TORCH_PTLINK:
    {
        m_pointLink = dynamic_cast<prop3D*>(prop.get())->value();
        break;
    }
    case eObjParam_TORCH_STRENGHT:
    {
        m_power = dynamic_cast<propFloat*>(prop.get())->value();
        break;
    }
    case eObjParam_TORCH_SOUND:
    {
        m_sound = dynamic_cast<propStr*>(prop.get())->value();
        break;
    }
    default:
        CWorldObj::applyParam(prop);
    }
}

QJsonObject CTorch::toJson()
{
    QJsonObject obj;
    QJsonObject world_obj = CWorldObj::toJson();
    obj.insert("World object", world_obj);
    obj.insert("Power", QJsonValue::fromVariant(m_power));
    QJsonArray point;
    point.append(QJsonValue::fromVariant(m_pointLink.x()));
    point.append(QJsonValue::fromVariant(m_pointLink.y()));
    point.append(QJsonValue::fromVariant(m_pointLink.z()));
    obj.insert("Position", point);
    obj.insert("Sound", m_sound);
    return obj;
}
