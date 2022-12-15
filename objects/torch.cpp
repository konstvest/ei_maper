#include <QJsonArray>
#include "torch.h"
#include "log.h"

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
        ei::log(eLogError, "Incorrect object type("+QString::number(m_type)+"), should be 58");
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

void CTorch::collectParams(QMap<EObjParam, QString> &aParam, ENodeType paramType)
{
    CWorldObj::collectParams(aParam, paramType);
    auto comm = paramType & eTorch;
    if (comm != eTorch)
        return;

    util::addParam(aParam, eObjParam_TORCH_PTLINK, util::makeString(m_pointLink));
    util::addParam(aParam, eObjParam_TORCH_STRENGHT, QString::number(m_power));
    util::addParam(aParam, eObjParam_TORCH_SOUND, m_sound);
}

void CTorch::applyParam(EObjParam param, const QString &value)
{
    switch (param) {
    case eObjParam_TORCH_PTLINK:
    {
        m_pointLink = util::vec3FromString(value);
        break;
    }
    case eObjParam_TORCH_STRENGHT:
    {
        m_power = value.toFloat();
        break;
    }
    case eObjParam_TORCH_SOUND:
    {
        m_sound = value;
        break;
    }
    default:
        CWorldObj::applyParam(param, value);
    }
}

QString CTorch::getParam(EObjParam param)
{
    QString value;
    switch (param) {
    case eObjParam_TORCH_PTLINK:
    {
        value = util::makeString(m_pointLink);
        break;
    }
    case eObjParam_TORCH_STRENGHT:
    {
        value = QString::number(m_power);
        break;
    }
    case eObjParam_TORCH_SOUND:
    {
        value = m_sound;
        break;
    }
    default:
        value = CWorldObj::getParam(param);
    }
    return value;
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
