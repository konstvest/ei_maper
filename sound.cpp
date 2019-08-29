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
    m_modelName = "sound.mod";
    return readByte;
}
