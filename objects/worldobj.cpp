#include <QJsonArray>

#include "objects\worldobj.h"
#include "mob.h"
#include "view.h"
#include "resourcemanager.h"
#include "landscape.h"
#include "log.h"

CWorldObj::CWorldObj():
    m_type(54)
    ,m_secondaryTexture("default0")
    ,m_parentTemplate("")
    ,m_player(0)
    ,m_parentID(-1)
    ,m_bUseInScript(false)
    ,m_bShadow(true)
    ,m_questInfo()
{
    CObjectBase::updateFigure(CObjectList::getInstance()->getFigure("cannotDisplay"));
    CObjectBase::setTexture(CTextureList::getInstance()->texture("cannotDisplay"));
}

CWorldObj::CWorldObj(const CWorldObj &wo):
    CObjectBase(wo)
{
    m_type = wo.m_type;
    m_modelName = wo.m_modelName;
    m_primaryTexture = wo.m_primaryTexture;
    m_secondaryTexture = wo.m_secondaryTexture;
    m_parentTemplate = wo.m_parentTemplate;
    m_player = wo.m_player;
    m_parentID = wo.m_parentID;
    m_bUseInScript = wo.m_bUseInScript;
    m_bShadow = wo.m_bShadow;
    m_questInfo = wo.m_questInfo;
}

CWorldObj::CWorldObj(QJsonObject data):
    CObjectBase(data["Base object"].toObject())
{
    m_type = (uint)data["Type"].toVariant().toUInt();
    m_modelName = data["Model name"].toString();
    m_primaryTexture = data["Primary texture"].toString();
    m_secondaryTexture = data["Secondary texture"].toString();

    QJsonArray aBodyParts = data["Object parts"].toArray();
    for(auto it = aBodyParts.begin(); it < aBodyParts.end(); ++it)
    {
        m_bodyParts.append(it->toString());
    }
    m_parentTemplate = data["Object template"].toString();
    m_player = (char)data["Player(dimpomacy group)"].toInt();
    m_parentID = data["Parent Id"].toVariant().toUInt();
    m_bUseInScript = data["Is used in script?"].toBool();
    m_bShadow = data["Is shadow?"].toBool();
    m_questInfo = data["Quest info"].toString();
}

uint CWorldObj::deserialize(util::CMobParser& parser)
{
    uint readByte(0);
    while(true)
    {
        if(parser.isNextTag("NID"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readDword(m_mapID);
        }
        else if(parser.isNextTag("OBJ_TYPE"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readDword(m_type);
        }
        else if(parser.isNextTag("OBJ_NAME"))
        {
            readByte += parser.readHeader();
            readByte += parser.readString(m_name, parser.nodeLen());
        }
        else if(parser.isNextTag("OBJ_INDEX"))
        {
            Q_ASSERT("OBJ_INDEX" && false);
            readByte += parser.skipTag();//, eNull};
        }

        else if(parser.isNextTag("OBJ_TEMPLATE"))
        {
            readByte += parser.readHeader();
            readByte += parser.readString(m_modelName, parser.nodeLen());
        }
        else if(parser.isNextTag("OBJ_PRIM_TXTR"))
        {
            readByte += parser.readHeader();
            readByte += parser.readString(m_primaryTexture, parser.nodeLen());
        }
        else if(parser.isNextTag("OBJ_SEC_TXTR")){
            readByte += parser.readHeader();
            readByte += parser.readString(m_secondaryTexture, parser.nodeLen());
        }
        else if(parser.isNextTag("OBJ_POSITION"))
        {
            readByte += parser.skipHeader();
            QVector3D pos;
            readByte += parser.readPlot(pos);
            setPos(pos);
        }
        else if(parser.isNextTag("OBJ_ROTATION"))
        {
            readByte += parser.skipHeader();
            QVector4D rot;
            readByte += parser.readQuaternion(rot);
            CNode::setRot(rot);
        }
        else if(parser.isNextTag("OBJ_TEXTURE"))
        {
            Q_ASSERT("OBJ_TEDXTURE" && false);
            readByte += parser.skipTag();//, eNull};
        }
        else if(parser.isNextTag("OBJ_COMPLECTION"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readPlot(m_complection);
        }
        else if(parser.isNextTag("OBJ_BODYPARTS"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readStringArray(m_bodyParts);
        }
        else if(parser.isNextTag("PARENT_TEMPLATE"))
        {
            readByte += parser.readHeader();
            readByte += parser.readString(m_parentTemplate, parser.nodeLen());
        }
        else if(parser.isNextTag("OBJ_COMMENTS"))
        {
            readByte += parser.readHeader();
            readByte += parser.readString(m_comment, parser.nodeLen());
        }
        else if(parser.isNextTag("OBJ_DEF_LOGIC"))
        {
            Q_ASSERT("OBJ_DEF_LOGIC" && false);
            readByte += parser.skipTag();//, eNull};
        }
        else if(parser.isNextTag("OBJ_PLAYER"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readByte(m_player);
        }
        else if(parser.isNextTag("OBJ_PARENT_ID"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readDword(m_parentID);
        }
        else if(parser.isNextTag("OBJ_USE_IN_SCRIPT"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readBool(m_bUseInScript);
        }
        else if(parser.isNextTag("OBJ_IS_SHADOW"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readBool(m_bShadow);
        }
        else if(parser.isNextTag("OBJ_R"))
        {
            Q_ASSERT("OBJ_R" && false);
            readByte += parser.skipTag();//, eNull};
        }
        else if(parser.isNextTag("OBJ_QUEST_INFO"))
        {
            readByte += parser.readHeader();
            readByte += parser.readString(m_questInfo, parser.nodeLen());
        }
        else
        {
            break;
        }
    }
    return readByte;
}

void CWorldObj::loadTexture()
{
    QString texName(m_primaryTexture.toLower());
    setTexture(CTextureList::getInstance()->texture(texName));
    //todo: load texture for logic
}

void CWorldObj::serializeJson(QJsonObject& obj)
{
    CObjectBase::serializeJson(obj);
    obj.insert("Type", QJsonValue::fromVariant(m_type));
    //m_mapID
    //m_name
    //m_modelName
    obj.insert("Primary texture", m_primaryTexture);
    obj.insert("Secondary texture", m_secondaryTexture);

    //todo: use info in CObjectBase
    QJsonArray aBodyParts;
    for(auto& part : m_bodyParts)
    {
        aBodyParts.append(part);
    }
    obj.insert("Object parts", aBodyParts);
    obj.insert("Object template", m_parentTemplate);
    obj.insert("Player(dimpomacy group)", QJsonValue::fromVariant(int(m_player)));
    obj.insert("Parent Id", QJsonValue::fromVariant(m_parentID));
    obj.insert("Is used in script?", m_bUseInScript);
    obj.insert("Is shadow?", m_bShadow);
    obj.insert("Quest info", m_questInfo);
    return;
}

uint CWorldObj::serialize(util::CMobParser &parser)
{
    uint writeByte(0);
    if (nodeType() == eWorldObject)
        writeByte += parser.startSection("OBJECT");

    writeByte += parser.startSection("OBJ_BODYPARTS");
    writeByte += parser.writeStringArray(m_bodyParts, "OBJ_BODYPARTS");
    parser.endSection(); //OBJ_BODYPARTS

    writeByte += parser.startSection("OBJ_PLAYER");
    writeByte += parser.writeByte(m_player);
    parser.endSection(); //OBJ_PLAYER

    writeByte += parser.startSection("NID");
    writeByte += parser.writeDword(m_mapID);
    parser.endSection(); //"NID"

    Q_ASSERT(m_type);
    writeByte += parser.startSection("OBJ_TYPE");
    writeByte += parser.writeDword(m_type);
    parser.endSection(); //"OBJ_TYPE"

    writeByte += parser.startSection("OBJ_NAME");
    writeByte += parser.writeString(m_name);
    parser.endSection(); //OBJ_NAME

    writeByte += parser.startSection("OBJ_TEMPLATE");
    writeByte += parser.writeString(m_modelName);
    parser.endSection(); //OBJ_TEMPLATE

    writeByte += parser.startSection("PARENT_TEMPLATE");
    writeByte += parser.writeString(m_parentTemplate);
    parser.endSection(); //PARENT_TEMPLATE

    writeByte += parser.startSection("OBJ_PRIM_TXTR");
    writeByte += parser.writeString(m_primaryTexture);
    parser.endSection(); //OBJ_PRIM_TXTR

    writeByte += parser.startSection("OBJ_SEC_TXTR");
    writeByte += parser.writeString(m_secondaryTexture);
    parser.endSection(); //OBJ_SEC_TXTR

    writeByte += parser.startSection("OBJ_COMMENTS");
    writeByte += parser.writeString(m_comment);
    parser.endSection(); //OBJ_COMMENTS

    writeByte += parser.startSection("OBJ_POSITION");
    writeByte += parser.writePlot(m_position);
    parser.endSection(); //OBJ_POSITION

    writeByte += parser.startSection("OBJ_ROTATION");
    QMatrix3x3 mtrx3 = m_rotateMatrix.normalMatrix();
    QQuaternion quat;
    quat = QQuaternion::fromRotationMatrix(mtrx3);
    QVector4D quatObj = quat.toVector4D();
    writeByte += parser.writeQuaternion(quatObj);
    parser.endSection(); //OBJ_ROTATION

    writeByte += parser.startSection("OBJ_USE_IN_SCRIPT");
    writeByte += parser.writeBool(m_bUseInScript);
    parser.endSection(); //OBJ_USE_IN_SCRIPT

    writeByte += parser.startSection("OBJ_IS_SHADOW");
    writeByte += parser.writeBool(m_bShadow);
    parser.endSection(); //OBJ_IS_SHADOW

    writeByte += parser.startSection("OBJ_PARENT_ID");
    writeByte += parser.writeDword(m_parentID);
    parser.endSection(); //OBJ_PARENT_ID

    writeByte += parser.startSection("OBJ_QUEST_INFO");
    writeByte += parser.writeString(m_questInfo);
    parser.endSection(); //OBJ_QUEST_INFO

    writeByte += parser.startSection("OBJ_COMPLECTION");
    writeByte += parser.writePlot(m_complection);
    parser.endSection();//"OBJ_COMPLECTION"

    if (nodeType() == eWorldObject)
        parser.endSection(); //OBJECT

    return writeByte;
}

void CWorldObj::collectParams(QMap<EObjParam, QString> &aParam, ENodeType paramType)
{
    CObjectBase::collectParams(aParam, paramType); //don't want to change type from parameters. should use delete and add new object with type
    auto comm = paramType & eWorldObject;
    if (comm != eWorldObject)
        return;

    addParam(aParam, eObjParam_BODYPARTS, util::makeString(m_bodyParts));
    addParam(aParam, eObjParam_PLAYER, QString::number(m_player));

    //addParam(aParam, eObjParam_TYPE, QString::number(m_type));
    addParam(aParam, eObjParam_NAME, m_name);
    addParam(aParam, eObjParam_TEMPLATE, m_modelName);
    addParam(aParam, eObjParam_PARENT_TEMPLATE, m_parentTemplate);
    addParam(aParam, eObjParam_PRIM_TXTR, m_primaryTexture);
    //addParam(aParam, eObjParam_SEC_TXTR, m_secondaryTexture);


    QMatrix3x3 mtrx3 = m_rotateMatrix.normalMatrix();
    QQuaternion quat;
    quat = QQuaternion::fromRotationMatrix(mtrx3);
    QVector3D eulerRot = quat.toEulerAngles();
    addParam(aParam, eObjParam_ROTATION, util::makeString(eulerRot));  //todo: show rotation as X,Y,Z rotation, don't use quaternion
    addParam(aParam, eObjParam_USE_IN_SCRIPT, util::makeString(m_bUseInScript));
    addParam(aParam, eObjParam_IS_SHADOW, util::makeString(m_bShadow));
    addParam(aParam, eObjParam_PARENT_ID, QString::number(m_parentID));
    addParam(aParam, eObjParam_QUEST_INFO, m_questInfo);
    addParam(aParam, eObjParam_COMPLECTION, util::makeString(m_complection));

}

void CWorldObj::applyParam(EObjParam param, const QString &value)
{
    switch (param)
    {
    case eObjParam_BODYPARTS:
    {
        m_bodyParts = util::strListFromString(value);
        recalcFigure();
        break;
    }
    case eObjParam_PLAYER:
    {
        m_player = char(value.toInt());
        break;
    }

    case eObjParam_TYPE:
    {
        m_type = value.toUInt();
        break;
    }
    case eObjParam_TEMPLATE:
    {
        m_modelName = value;
        m_bodyParts.clear();
        auto pFig = CObjectList::getInstance()->getFigure(m_modelName);
        CObjectBase::updateFigure(pFig);
        break;
    }
    case eObjParam_PARENT_TEMPLATE:
    {
        //todo: delete obj and create new from template if exists
        m_parentTemplate = value;
        break;
    }
    case eObjParam_PRIM_TXTR:
    {
        m_primaryTexture = value;
        setTexture(CTextureList::getInstance()->texture(m_primaryTexture));
        break;
    }
    case eObjParam_SEC_TXTR:
    {
        m_secondaryTexture = value;
        break;
    }
    case eObjParam_ROTATION:
    {
        //QVector3D eulerRot = util::vec3FromString(value);
        const float gimbalAvoidStep = 0.001f;
        QQuaternion quat;//this rotation as quat be applied to object
        QVector3D eulerRot; // temp variable
        const auto setNotNan = [&quat, &eulerRot, &gimbalAvoidStep](QVector3D& ch_rot)
        {
            for(int i(0); i< 100; ++i)
            {
                quat = QQuaternion::fromEulerAngles(ch_rot);
                eulerRot = quat.toEulerAngles();
                if(eulerRot.x() != eulerRot.x())
                {
                    ch_rot.setX(ch_rot.x() + gimbalAvoidStep);
                }
                else if(eulerRot.y() != eulerRot.y())
                {
                    ch_rot.setY(ch_rot.y() + gimbalAvoidStep);
                }
                else if(eulerRot.z() != eulerRot.z())
                {
                    ch_rot.setZ(ch_rot.z() + gimbalAvoidStep);
                }
                else
                    return;
            }
            return;
        };
        QVector3D rotation = util::vec3FromString(value); //this rotation will be tested
        setNotNan(rotation);
        setRot(quat);
        rotation = getEulerRotation();
        recalcMinPos();
        break;
    }
    case eObjParam_USE_IN_SCRIPT:
    {
        m_bUseInScript = util::boolFromString(value);
        break;
    }
    case eObjParam_IS_SHADOW:
    {
        m_bShadow = util::boolFromString(value);
        break;
    }
    case eObjParam_PARENT_ID:
    {
        m_parentID = value.toInt();
        break;
    }
    case eObjParam_QUEST_INFO:
    {
        m_questInfo = value;
        break;
    }
    case eObjParam_COMPLECTION:
    {
        m_complection = util::vec3FromString(value);
        recalcFigure();
        break;
    }
    default:
        CObjectBase::applyParam(param, value);
        break;
    }
}

QString CWorldObj::getParam(EObjParam param)
{
    QString value;
    switch (param)
    {
    case eObjParam_BODYPARTS:
    {
        value = util::makeString(m_bodyParts);
        break;
    }
    case eObjParam_PLAYER:
    {
        value = QString::number(m_player);
        break;
    }
//    case eObjParam_TYPE:
//    {
//        value = QString::number(m_type);
//        break;
//    }
    case eObjParam_TEMPLATE:
    {
        value = m_modelName;
        break;
    }
    case eObjParam_PARENT_TEMPLATE:
    {
        value = m_parentTemplate;
        break;
    }
    case eObjParam_PRIM_TXTR:
    {
        value = m_primaryTexture;
        break;
    }
    case eObjParam_SEC_TXTR:
    {
        value = m_secondaryTexture;
        break;
    }
    case eObjParam_ROTATION:
    {
        value = util::makeString(getEulerRotation());
        break;
    }
    case eObjParam_USE_IN_SCRIPT:
    {
        value = util::makeString(m_bUseInScript);
        break;
    }
    case eObjParam_IS_SHADOW:
    {
        value = util::makeString(m_bShadow);
        break;
    }
    case eObjParam_PARENT_ID:
    {
        value = util::makeString(m_parentID);
        break;
    }
    case eObjParam_QUEST_INFO:
    {
        value = m_questInfo;
        break;
    }
    case eObjParam_COMPLECTION:
    {
        value = util::makeString(m_complection);
        break;
    }
    default:
        value = CObjectBase::getParam(param);
        break;
    }

    return value;
}

QJsonObject CWorldObj::toJson()
{
    QJsonObject obj;
    QJsonObject base_obj = CObjectBase::toJson();
    obj.insert("Base object", base_obj);
    obj.insert("Type", QJsonValue::fromVariant(m_type));
    obj.insert("Model name", m_modelName);
    obj.insert("Primary texture", m_primaryTexture);
    obj.insert("Secondary texture", m_secondaryTexture);

    //todo: use info in CObjectBase
    QJsonArray aBodyParts;
    for(auto& part : m_bodyParts)
    {
        aBodyParts.append(part);
    }
    obj.insert("Object parts", aBodyParts);
    obj.insert("Object template", m_parentTemplate);
    obj.insert("Player(dimpomacy group)", QJsonValue::fromVariant(int(m_player)));
    obj.insert("Parent Id", QJsonValue::fromVariant(m_parentID));
    obj.insert("Is used in script?", m_bUseInScript);
    obj.insert("Is shadow?", m_bShadow);
    obj.insert("Quest info", m_questInfo);
    return obj;
}
