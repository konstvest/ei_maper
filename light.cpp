#include "light.h"

CLight::CLight()
{

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
    m_modelName = "light.mod";
    return readByte;
}
