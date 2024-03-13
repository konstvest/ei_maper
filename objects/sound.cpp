#include <QJsonArray>
#include "sound.h"
#include "resourcemanager.h"
#include "property.h"

CSound::CSound():
    m_range(0)
    ,m_range2(0)
    ,m_min(0)
    ,m_max(0)
    ,m_bAmbient(false)
    ,m_bMusic(false)
{
    updateFigure(CObjectList::getInstance()->getFigure("sound"));
    setTexture(CTextureList::getInstance()->texture("sound"));
}

CSound::CSound(const CSound &sound):
    CObjectBase(sound)
{
    m_range = sound.m_range;
    m_range2 = sound.m_range2;
    m_min = sound.m_min;
    m_max = sound.m_max;
    m_aResName = sound.m_aResName;
    m_bAmbient = sound.m_bAmbient;
    m_bMusic = sound.m_bMusic;
    updateFigure(CObjectList::getInstance()->getFigure("sound"));
    setTexture(CTextureList::getInstance()->texture("sound"));
}

CSound::CSound(QJsonObject data):
    CObjectBase(data["Base object"].toObject())
{
    m_range = data["Range 1(?1)"].toVariant().toUInt();
    m_range2 = data["Range 2(?!)"].toVariant().toUInt();
    m_min = data["Min distance"].toVariant().toUInt();
    m_max = data["Max distance"].toVariant().toUInt();

    QJsonArray aRes = data["Resources"].toArray();
    for(auto it=aRes.begin(); it<aRes.end();++it)
    {
        m_aResName.append(it->toString());
    }
    m_bAmbient = data["Is ambient?"].toBool();
    m_bMusic = data["Is Music?"].toBool();
    updateFigure(CObjectList::getInstance()->getFigure("sound"));
    setTexture(CTextureList::getInstance()->texture("sound"));
}

uint CSound::deserialize(util::CMobParser &parser)
{
    uint readByte(0);
    while(true)
    {
        if(parser.isNextTag("SOUND_ID"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readDword(m_mapID);
        }
        else if(parser.isNextTag("SOUND_POSITION"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readPlot(m_position);
        }
        else if(parser.isNextTag("SOUND_RANGE"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readDword(m_range);
        }
        else if(parser.isNextTag("SOUND_NAME"))
        {
            readByte += parser.readHeader();
            readByte += parser.readString(m_name, parser.nodeLen());
        }
        else if(parser.isNextTag("SOUND_MIN"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readDword(m_min);
        }
        else if(parser.isNextTag("SOUND_MAX"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readDword(m_max);
        }
        else if(parser.isNextTag("SOUND_COMMENTS"))
        {
            readByte += parser.readHeader();
            readByte += parser.readString(m_comment, parser.nodeLen());
        }
        else if(parser.isNextTag("SOUND_VOLUME"))
        {
            Q_ASSERT("SOUND_VOLUME" && false);
            readByte += parser.skipTag();
            //, eNull};
        }
        else if(parser.isNextTag("SOUND_RESNAME"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readStringArray(m_aResName);
        }
        else if(parser.isNextTag("SOUND_RANGE2"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readDword(m_range2);
        }
        else if(parser.isNextTag("SOUND_AMBIENT"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readBool(m_bAmbient);
        }
        else if(parser.isNextTag("SOUND_IS_MUSIC"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readBool(m_bMusic);
        }
        else
        {
            break;
        }
    }
    return readByte;
}

void CSound::serializeJson(QJsonObject& obj)
{
    CObjectBase::serializeJson(obj);
    obj.insert("Range 1(?1)", QJsonValue::fromVariant(m_range));
    obj.insert("Range 2(?!)", QJsonValue::fromVariant(m_range2));
    obj.insert("Min distance", QJsonValue::fromVariant(m_min));
    obj.insert("Max distance", QJsonValue::fromVariant(m_max));
    QJsonArray aRes;
    for(auto& res : m_aResName)
        aRes.append(res);
    obj.insert("Resources", aRes);
    obj.insert("Is ambient?", m_bAmbient);
    obj.insert("Is Music?", m_bMusic);
    return;
}

uint CSound::serialize(util::CMobParser& parser)
{
    uint writeByte(0);
    writeByte += parser.startSection("SOUND");

    writeByte += parser.startSection("SOUND_ID");
    writeByte += parser.writeDword(m_mapID);
    parser.endSection(); //SOUND_ID

    writeByte += parser.startSection("SOUND_POSITION");
    writeByte += parser.writePlot(m_position);
    parser.endSection(); //SOUND_POSITION

    writeByte += parser.startSection("SOUND_RANGE");
    writeByte += parser.writeDword(m_range);
    parser.endSection(); //SOUND_RANGE

    writeByte += parser.startSection("SOUND_NAME");
    writeByte += parser.writeString(m_name);
    parser.endSection(); //SOUND_NAME

    writeByte += parser.startSection("SOUND_MIN");
    writeByte += parser.writeDword(m_min);
    parser.endSection(); //SOUND_MIN

    writeByte += parser.startSection("SOUND_MAX");
    writeByte += parser.writeDword(m_max);
    parser.endSection(); //SOUND_MAX

    writeByte += parser.startSection("SOUND_COMMENTS");
    writeByte += parser.writeString(m_comment);
    parser.endSection(); //SOUND_COMMENTS

    writeByte += parser.startSection("SOUND_RESNAME");
    writeByte += parser.writeStringArray(m_aResName, "SOUND_RESNAME");
    parser.endSection(); //SOUND_RESNAME

    writeByte += parser.startSection("SOUND_RANGE2");
    writeByte += parser.writeDword(m_range2);
    parser.endSection(); //SOUND_RANGE2

    writeByte += parser.startSection("SOUND_AMBIENT");
    writeByte += parser.writeBool(m_bAmbient);
    parser.endSection(); //SOUND_AMBIENT

    writeByte += parser.startSection("SOUND_IS_MUSIC");
    writeByte += parser.writeBool(m_bMusic);
    parser.endSection(); //SOUND_IS_MUSIC

    parser.endSection(); //"SOUND"
    return writeByte;
}

void CSound::collectParams(QList<QSharedPointer<IPropertyBase>>& aProp, ENodeType paramType)
{
    CObjectBase::collectParams(aProp, paramType);
    auto comm = paramType & eSound;
    if (comm != eSound)
        return;

    propUint range(eObjParam_RANGE, m_range);
    util::addParam(aProp, &range);
    propUint min(eObjParam_SOUND_MIN, m_min);
    util::addParam(aProp, &min);
    propUint max(eObjParam_SOUND_MAX, m_max);
    util::addParam(aProp, &max);
    propStrAr aResName(eObjParam_SOUND_RESNAME, m_aResName);
    util::addParam(aProp, &aResName);
    propUint range2(eObjParam_SOUND_RANGE, m_range2);
    util::addParam(aProp, &range2);
    propBool bAmbient(eObjParam_SOUND_AMBIENT, m_bAmbient);
    util::addParam(aProp, &bAmbient);
    propBool bMusic(eObjParam_SOUND_IS_MUSIC, m_bMusic);
    util::addParam(aProp, &bMusic);
}

void CSound::getParam(QSharedPointer<IPropertyBase>& prop, EObjParam propType)
{
    switch (propType)
    {
    case eObjParam_RANGE:
    {
        prop.reset(new propUint(propType, m_range));
        break;
    }
    case eObjParam_SOUND_MIN:
    {
        prop.reset(new propUint(propType, m_min));
        break;
    }
    case eObjParam_SOUND_MAX:
    {
        prop.reset(new propUint(propType, m_max));
        break;
    }
    case eObjParam_SOUND_RESNAME:
    {
        prop.reset(new propStrAr(propType, m_aResName));
        break;
    }
    case eObjParam_SOUND_RANGE:
    {
        prop.reset(new propUint(propType, m_range2));
        break;
    }
    case eObjParam_SOUND_AMBIENT:
    {
        prop.reset(new propBool(propType, m_bAmbient));
        break;
    }
    case eObjParam_SOUND_IS_MUSIC:
    {
        prop.reset(new propBool(propType, m_bMusic));
        break;
    }
    default:
        CObjectBase::getParam(prop, propType);
    }
}

void CSound::applyParam(const QSharedPointer<IPropertyBase>& prop)
{
    switch (prop->type())
    {
    case eObjParam_RANGE:
    {
        m_range = dynamic_cast<propUint*>(prop.get())->value();
        break;
    }
    case eObjParam_SOUND_MIN:
    {
        m_min = dynamic_cast<propUint*>(prop.get())->value();
        break;
    }
    case eObjParam_SOUND_MAX:
    {
        m_max = dynamic_cast<propUint*>(prop.get())->value();
        break;
    }
    case eObjParam_SOUND_RESNAME:
    {
        m_aResName = dynamic_cast<propStrAr*>(prop.get())->value();
        break;
    }
    case eObjParam_SOUND_RANGE:
    {
        m_range2 = dynamic_cast<propUint*>(prop.get())->value();
        break;
    }
    case eObjParam_SOUND_AMBIENT:
    {
        m_bAmbient = dynamic_cast<propBool*>(prop.get())->value();
        break;
    }
    case eObjParam_SOUND_IS_MUSIC:
    {
        m_bMusic = dynamic_cast<propBool*>(prop.get())->value();
        break;
    }
    default:
        CObjectBase::applyParam(prop);
    }
}

QJsonObject CSound::toJson()
{
    QJsonObject obj;
    QJsonObject base_obj = CObjectBase::toJson();
    obj.insert("Base object", base_obj);
    obj.insert("Range 1(?1)", QJsonValue::fromVariant(m_range));
    obj.insert("Range 2(?!)", QJsonValue::fromVariant(m_range2));
    obj.insert("Min distance", QJsonValue::fromVariant(m_min));
    obj.insert("Max distance", QJsonValue::fromVariant(m_max));

    QJsonArray aRes;
    for(auto& res : m_aResName)
        aRes.append(res);

    obj.insert("Resources", aRes);
    obj.insert("Is ambient?", m_bAmbient);
    obj.insert("Is Music?", m_bMusic);
    return obj;
}
