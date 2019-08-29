#include "worldobj.h"

CWorldObj::CWorldObj()
{

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
            m_modelName += ".mod";
        }
        else if(parser.isNextTag("OBJ_PRIM_TXTR"))
        {
            readByte += parser.readHeader();
            readByte += parser.readString(m_primaryTexture, parser.nodeLen());
            m_primaryTexture += ".mmp";
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
            setRot(rot);
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

void CWorldObj::updateVisibleParts()
{
    updateVisibility(m_bodyParts);
}

