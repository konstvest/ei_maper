#include "particle.h"

CParticle::CParticle()
{

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
    m_modelName = "particle.mod";
    return readByte;
}
