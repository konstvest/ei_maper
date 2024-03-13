#include <QJsonArray>
#include "light.h"
#include "resourcemanager.h"
#include "property.h"

CLight::CLight():
    m_range(0.0f)
    ,m_bShadow(false)
    ,m_color(0.0f, 0.0f, 0.0f)
{
    updateFigure(CObjectList::getInstance()->getFigure("light"));
    setTexture(CTextureList::getInstance()->texture("light"));
}

CLight::CLight(const CLight &light):
    CObjectBase(light)
{
    m_range = light.m_range;
    m_bShadow = light.m_bShadow;
    m_color = light.m_color;
    updateFigure(CObjectList::getInstance()->getFigure("light"));
    setTexture(CTextureList::getInstance()->texture("light"));
}

CLight::CLight(QJsonObject data):
    CObjectBase(data["Base object"].toObject())
{
    m_range = data["Range"].toVariant().toFloat();
    m_bShadow = data["Is shadow?"].toBool();
    QJsonArray aColor = data["Color"].toArray();
    if (aColor.size()==3)
        m_color = QVector3D(aColor[0].toVariant().toFloat(), aColor[1].toVariant().toFloat(), aColor[2].toVariant().toFloat());
    updateFigure(CObjectList::getInstance()->getFigure("light"));
    setTexture(CTextureList::getInstance()->texture("light"));
}

uint CLight::deserialize(util::CMobParser& parser)
{
    uint readByte(0);
    while(true)
    {
        if(parser.isNextTag("LIGHT_RANGE"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readFloat(m_range);
        }
        else if(parser.isNextTag("LIGHT_NAME"))
        {
            readByte += parser.readHeader();
            readByte += parser.readString(m_name, parser.nodeLen());
        }
        else if(parser.isNextTag("LIGHT_POSITION"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readPlot(m_position);
        }
        else if(parser.isNextTag("LIGHT_ID"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readDword(m_mapID);
        }
        else if(parser.isNextTag("LIGHT_SHADOW"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readBool(m_bShadow);
        }
        else if(parser.isNextTag("LIGHT_COLOR"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readPlot(m_color);
        }
        else if(parser.isNextTag("LIGHT_COMMENTS"))
        {
            readByte += parser.readHeader();
            readByte += parser.readString(m_comment, parser.nodeLen());
        }
        else
        {
            break;
        }
    }
    return readByte;
}

void CLight::serializeJson(QJsonObject& obj)
{
    CObjectBase::serializeJson(obj);
    obj.insert("Range", QJsonValue::fromVariant(m_range));
    obj.insert("Is shadow?", m_bShadow);
    QJsonArray aColor;
    aColor.append(QJsonValue::fromVariant(m_color.x()));
    aColor.append(QJsonValue::fromVariant(m_color.y()));
    aColor.append(QJsonValue::fromVariant(m_color.z()));
    obj.insert("Color", aColor);
}

uint CLight::serialize(util::CMobParser &parser)
{
    uint writeByte(0);
    writeByte += parser.startSection("LIGHT");

    writeByte += parser.startSection("LIGHT_ID");
    writeByte += parser.writeDword(m_mapID);
    parser.endSection(); //LIGHT_ID

    writeByte += parser.startSection("LIGHT_SHADOW");
    writeByte += parser.writeBool(m_bShadow);
    parser.endSection(); //LIGHT_SHADOW

    writeByte += parser.startSection("LIGHT_POSITION");
    writeByte += parser.writePlot(m_position);
    parser.endSection(); //LIGHT_POSITION

    writeByte += parser.startSection("LIGHT_COLOR");
    writeByte += parser.writePlot(m_color);
    parser.endSection(); //LIGHT_COLOR

    writeByte += parser.startSection("LIGHT_RANGE");
    writeByte += parser.writeFloat(m_range);
    parser.endSection(); //LIGHT_RANGE

    writeByte += parser.startSection("LIGHT_NAME");
    writeByte +=  parser.writeString(m_name);
    parser.endSection(); //LIGHT_NAME

    writeByte += parser.startSection("LIGHT_COMMENTS");
    writeByte += parser.writeString(m_comment);
    parser.endSection(); //LIGHT_COMMENTS

    parser.endSection(); //LIGHT
    return writeByte;
}

void CLight::collectParams(QList<QSharedPointer<IPropertyBase>>& aProp, ENodeType paramType)
{
    CObjectBase::collectParams(aProp, paramType);
    auto comm = paramType & eLight;
    if (comm != eLight)
        return;

    //util::addParam(aProp, eObjParam_NID, QString::number(m_mapID));
    propBool bShadow(eObjParam_LIGHT_SHADOW, m_bShadow);
    util::addParam(aProp, &bShadow);
    //util::addParam(aProp, eObjParam_POSITION, util::makeString(m_position));
    prop3D color(eObjParam_LIGHT_COLOR, m_color);
    util::addParam(aProp, &color);
    propFloat range(eObjParam_RANGE, m_range);
    util::addParam(aProp, &range);
    propStr name(eObjParam_NAME, m_name);
    util::addParam(aProp, &name);
    //util::addParam(aProp, eObjParam_COMMENTS, m_comment);
}

void CLight::getParam(QSharedPointer<IPropertyBase>& prop, EObjParam propType)
{
    switch (propType)
    {
    case eObjParam_LIGHT_SHADOW:
    {
        prop.reset(new propBool(propType, m_bShadow));
        break;
    }
    case eObjParam_LIGHT_COLOR:
    {
        prop.reset(new prop3D(propType, m_color));
        break;
    }
    case eObjParam_RANGE:
    {
        prop.reset(new propFloat(propType, m_range));
        break;
    }
    default:
        CObjectBase::getParam(prop, propType);
    }
}

void CLight::applyParam(const QSharedPointer<IPropertyBase>& prop)
{
    switch (prop->type()){
    case eObjParam_LIGHT_SHADOW:
    {
        m_bShadow = dynamic_cast<propBool*>(prop.get())->value();
        break;
    }
    case eObjParam_LIGHT_COLOR:
    {
        m_color = dynamic_cast<prop3D*>(prop.get())->value();
        break;
    }
    case eObjParam_RANGE:
    {
        m_range = dynamic_cast<propFloat*>(prop.get())->value();
        break;
    }
    default:
        CObjectBase::applyParam(prop);
    }
}

QJsonObject CLight::toJson()
{
    QJsonObject obj;
    QJsonObject base_obj = CObjectBase::toJson();
    obj.insert("Base object", base_obj);
    obj.insert("Range", QJsonValue::fromVariant(m_range));
    obj.insert("Is shadow?", m_bShadow);
    QJsonArray aColor;
    aColor.append(QJsonValue::fromVariant(m_color.x()));
    aColor.append(QJsonValue::fromVariant(m_color.y()));
    aColor.append(QJsonValue::fromVariant(m_color.z()));
    obj.insert("Color", aColor);
    return obj;
}
