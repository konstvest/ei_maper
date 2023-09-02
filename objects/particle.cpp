#include "particle.h"
#include "resourcemanager.h"
#include "property.h"

CParticle::CParticle():
    m_kind(0)
    ,m_scale(0.0f)
{
    updateFigure(CObjectList::getInstance()->getFigure("particle"));
    setTexture(CTextureList::getInstance()->texture("particle"));
}

CParticle::CParticle(const CParticle &particle):
    CObjectBase(particle)
{
    m_kind = particle.m_kind;
    m_scale = particle.m_scale;
    updateFigure(CObjectList::getInstance()->getFigure("particle"));
    setTexture(CTextureList::getInstance()->texture("particle"));
}

CParticle::CParticle(QJsonObject data):
    CObjectBase(data["Base object"].toObject())
{
    m_kind = data["Type"].toVariant().toUInt();
    m_scale = data["Scale"].toVariant().toFloat();
    updateFigure(CObjectList::getInstance()->getFigure("particle"));
    setTexture(CTextureList::getInstance()->texture("particle"));
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

void CParticle::collectParams(QMap<QSharedPointer<IPropertyBase>, bool>& aProp, ENodeType paramType)
{
    CObjectBase::collectParams(aProp, paramType);
    auto comm = paramType & eParticle;
    if (comm != eParticle)
        return;

    propUint kind(eObjParam_PARTICL_TYPE, m_kind);
    util::addParam(aProp, &kind);
    propFloat scale(eObjParam_PARTICL_SCALE, m_scale);
    util::addParam(aProp, &scale);
}

void CParticle::getParam(QSharedPointer<IPropertyBase>& prop, EObjParam propType)
{
    switch (propType)
    {
    case eObjParam_PARTICL_TYPE:
    {
        prop.reset(new propUint(propType, m_kind));
        break;
    }
    case eObjParam_PARTICL_SCALE:
    {
        prop.reset(new propFloat(propType, m_scale));
        break;
    }
    default:
        CObjectBase::getParam(prop, propType);
    }
}

void CParticle::applyParam(const QSharedPointer<IPropertyBase>& prop)
{
    switch (prop->type())
    {
    case eObjParam_PARTICL_TYPE:
    {
        m_kind = dynamic_cast<propUint*>(prop.get())->value();
        break;
    }
    case eObjParam_PARTICL_SCALE:
    {
        m_scale = dynamic_cast<propFloat*>(prop.get())->value();
        break;
    }
    default:
        CObjectBase::applyParam(prop);
    }
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
