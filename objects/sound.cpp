#include <QJsonArray>
#include "sound.h"

CSound::CSound()
{

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
    m_modelName = "sound";
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

void CSound::collectParams(QMap<EObjParam, QString> &aParam, ENodeType paramType)
{
    CObjectBase::collectParams(aParam, paramType);
    auto comm = paramType & eSound;
    if (comm != eSound)
        return;

    addParam(aParam, eObjParam_RANGE, QString::number(m_range));
    addParam(aParam, eObjParam_SOUND_MIN, QString::number(m_min));
    addParam(aParam, eObjParam_SOUND_MAX, QString::number(m_max));
    addParam(aParam, eObjParam_SOUND_RESNAME, util::makeString(m_aResName));
    addParam(aParam, eObjParam_SOUND_RANGE, QString::number(m_range2));
    addParam(aParam, eObjParam_SOUND_AMBIENT, util::makeString(m_bAmbient));
    addParam(aParam, eObjParam_SOUND_IS_MUSIC, util::makeString(m_bMusic));
}

void CSound::applyParam(EObjParam param, const QString &value)
{
    switch (param)
    {
    case eObjParam_RANGE:
    {
        m_range = value.toInt();
        break;
    }
    case eObjParam_SOUND_MIN:
    {
        m_min = value.toInt();
        break;
    }
    case eObjParam_SOUND_MAX:
    {
        m_max = value.toInt();
        break;
    }
    case eObjParam_SOUND_RESNAME:
    {
        m_aResName = util::strListFromString(value);
        break;
    }
    case eObjParam_SOUND_RANGE:
    {
        m_range2 = value.toInt();
        break;
    }
    case eObjParam_SOUND_AMBIENT:
    {
        m_bAmbient = util::boolFromString(value);
        break;
    }
    case eObjParam_SOUND_IS_MUSIC:
    {
        m_bMusic = util::boolFromString(value);
        break;
    }
    default:
        CObjectBase::applyParam(param, value);
    }
}

QString CSound::getParam(EObjParam param)
{
    QString value;
    switch (param)
    {
    case eObjParam_RANGE:
    {
        value = QString::number(m_range);
        break;
    }
    case eObjParam_SOUND_MIN:
    {
        value = QString::number(m_min);
        break;
    }
    case eObjParam_SOUND_MAX:
    {
        value = QString::number(m_max);
        break;
    }
    case eObjParam_SOUND_RESNAME:
    {
        value = util::makeString(m_aResName);
        break;
    }
    case eObjParam_SOUND_RANGE:
    {
        value = QString::number(m_range2);
        break;
    }
    case eObjParam_SOUND_AMBIENT:
    {
        value = util::makeString(m_bAmbient);
        break;
    }
    case eObjParam_SOUND_IS_MUSIC:
    {
        value = util::makeString(m_bMusic);
        break;
    }
    default:
        value = CObjectBase::getParam(param);
    }
    return value;
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
