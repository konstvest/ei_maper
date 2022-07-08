#include "particle.h"

CParticle::CParticle()
{

}

CParticle::CParticle(QJsonObject data):
    CObjectBase(data["Base object"].toObject())
{
    m_kind = data["Type"].toVariant().toUInt();
    m_scale = data["Scale"].toVariant().toFloat();
}

uint CParticle::deserialize(util::CMobParser& parser)
{
    uint readByte(0);
    while(true)
    {
        if(parser.isNextTag("PARTICL_ID"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readDword(m_mapID);
        }
        else if(parser.isNextTag("PARTICL_POSITION"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readPlot(m_position);
        }
        else if(parser.isNextTag("PARTICL_COMMENTS"))
        {
            readByte += parser.readHeader();
            readByte += parser.readString(m_comment, parser.nodeLen());
        }
        else if(parser.isNextTag("PARTICL_NAME"))
        {
            readByte += parser.readHeader();
            readByte += parser.readString(m_name, parser.nodeLen());
        }
        else if(parser.isNextTag("PARTICL_TYPE"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readDword(m_kind);
        }
        else if(parser.isNextTag("PARTICL_SCALE"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readFloat(m_scale);
        }
        else
            break;
    }
    m_modelName = "particle";
    return readByte;
}

void CParticle::serializeJson(QJsonObject& obj)
{
    CObjectBase::serializeJson(obj);
    obj.insert("Type", QJsonValue::fromVariant(m_kind));
    obj.insert("Scale", QJsonValue::fromVariant(m_scale));
    return;
}

uint CParticle::serialize(util::CMobParser &parser)
{
    uint writeByte(0);
    writeByte += parser.startSection("PARTICL");

    writeByte += parser.startSection("PARTICL_ID");
    writeByte += parser.writeDword(m_mapID);
    parser.endSection(); //PARTICL_ID

    writeByte += parser.startSection("PARTICL_POSITION");
    writeByte += parser.writePlot(m_position);
    parser.endSection(); //PARTICL_POSITION

    writeByte += parser.startSection("PARTICL_NAME");
    writeByte += parser.writeString(m_name);
    parser.endSection(); //PARTICL_COMMENTS

    writeByte += parser.startSection("PARTICL_COMMENTS");
    writeByte += parser.writeString(m_comment);
    parser.endSection(); //PARTICL_COMMENTS

    writeByte += parser.startSection("PARTICL_TYPE");
    writeByte += parser.writeDword(m_kind);
    parser.endSection(); //PARTICL_TYPE

    writeByte += parser.startSection("PARTICL_SCALE");
    writeByte += parser.writeFloat(m_scale);
    parser.endSection(); //PARTICL_SCALE

    parser.endSection(); //PARTICL
    return writeByte;
}

void CParticle::collectParams(QMap<EObjParam, QString> &aParam, ENodeType paramType)
{
    CObjectBase::collectParams(aParam, paramType);
    auto comm = paramType & eParticle;
    if (comm != eParticle)
        return;


    addParam(aParam, eObjParam_PARTICL_TYPE, QString::number(m_kind));
    addParam(aParam, eObjParam_PARTICL_SCALE, QString::number(m_scale));
}

void CParticle::applyParam(EObjParam param, const QString &value)
{
    switch (param)
    {
    case eObjParam_PARTICL_TYPE:
    {
        m_kind = value.toInt();
        break;
    }
    case eObjParam_PARTICL_SCALE:
    {
        m_scale = value.toFloat();
        break;
    }
    default:
        CObjectBase::applyParam(param, value);
    }
}

QString CParticle::getParam(EObjParam param)
{
    QString value;
    switch (param)
    {
    case eObjParam_PARTICL_TYPE:
    {
        break;
    }
    case eObjParam_PARTICL_SCALE:
    {
        value = QString::number(m_scale);
        break;
    }
    default:
        value = CObjectBase::getParam(param);
    }
    return value;
}

QJsonObject CParticle::toJson()
{
    QJsonObject obj;
    QJsonObject base_obj = CObjectBase::toJson();
    obj.insert("Base object", base_obj);
    obj.insert("Type", QJsonValue::fromVariant(m_kind));
    obj.insert("Scale", QJsonValue::fromVariant(m_scale));
    return obj;
}
