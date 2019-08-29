#include "torch.h"

CTorch::CTorch()
{

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
    return readByte;
}
