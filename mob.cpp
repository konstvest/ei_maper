#include <QDebug>
#include <QSet>
#include "mob.h"
#include "utils.h"
#include "lever.h"
#include "light.h"
#include "magictrap.h"
#include "particle.h"
#include "sound.h"
#include "torch.h"
#include "worldobj.h"
#include "unit.h"
#include "view.h"
#include "objectlist.h"
#include "texturelist.h"

CMob::CMob()
{
    m_aNode.clear();
    m_aNodeSelected.clear();
}

CMob::~CMob()
{
    for (auto& node: m_aNode)
        delete node;

    for (auto& node: m_aNodeSelected)
        delete node;
}

void CMob::init()
{
    m_diplomacyFoF.resize(32);
    for(int i(0); i<m_diplomacyFoF.size(); ++i)
        m_diplomacyFoF[i].resize(32);
}

bool CMob::deserialize(QByteArray data)
{
    //todo: global check len of nodes
    uint readByte(0);
    util::CMobParser parser(data);
    if (parser.isNextTag("OBJECT_DB_FILE"))
        readByte += parser.skipHeader();
    else
        return false;

    if(parser.isNextTag("SC_OBJECT_DB_FILE") || parser.isNextTag("PR_OBJECT_DB_FILE"))
        readByte += parser.skipHeader();

    while(true)
    {
        if(parser.isNextTag("SS_TEXT"))
        {
            readByte += parser.readHeader();
            readByte += parser.readStringEncrypted(m_script, parser.nodeLen());
        }
        else if(parser.isNextTag("SS_TEXT_OLD"))
        {
            readByte += parser.readHeader();
            readByte += parser.readString(m_textOld, parser.nodeLen());
        }
        else if(parser.isNextTag("MAIN_RANGE"))
        {
            readByte += parser.skipHeader();
            while(parser.isNextTag("RANGE"))
            {
                SRange range;
                readByte += parser.skipHeader(); // range
                if (parser.isNextTag("MIN_ID"))
                {
                    readByte += parser.skipHeader();
                    readByte += parser.readDword(range.minRange);
                }
                if(parser.isNextTag("MAX_ID"))
                {
                    readByte += parser.skipHeader();
                    readByte += parser.readDword(range.maxRange);
                }
                m_aMainRange.append(range);
            }
        }
        else if(parser.isNextTag("SEC_RANGE"))
        {
            readByte += parser.skipHeader();
            while(parser.isNextTag("RANGE"))
            {
                SRange range;
                readByte += parser.skipHeader(); // range
                if (parser.isNextTag("MIN_ID"))
                {
                    readByte += parser.skipHeader();
                    readByte += parser.readDword(range.minRange);
                }
                if(parser.isNextTag("MAX_ID"))
                {
                    readByte += parser.skipHeader();
                    readByte += parser.readDword(range.maxRange);
                }
                m_aSecRange.append(range);
            }
        }
        else if(parser.isNextTag("DIPLOMATION"))
        {
            readByte += parser.skipHeader(); //diplomation
            while(true)
            {
                if(parser.isNextTag("DIPLOMATION_FOF"))
                {
                    readByte += parser.skipHeader();
                    readByte += parser.readDiplomacy(m_diplomacyFoF);
                }
                else if(parser.isNextTag("DIPLOMATION_PL_NAMES"))
                {
                    readByte += parser.skipHeader();
                    readByte += parser.readStringArray(m_aDiplomacyFieldName);
                }
                else {
                    break;
                }
            }
        }
        else if(parser.isNextTag("WORLD_SET"))
        {
            readByte += parser.skipHeader(); // world set
            while(true)
            {
                if(parser.isNextTag("WS_WIND_DIR"))
                {
                    readByte += parser.skipHeader();
                    readByte += parser.readPlot(m_worldSet.m_windDirection);
                }
                else if(parser.isNextTag("WS_WIND_STR"))
                {
                    readByte += parser.skipHeader();
                    readByte += parser.readFloat(m_worldSet.m_windStrength);
                }
                else if(parser.isNextTag("WS_TIME"))
                {
                    readByte += parser.skipHeader();
                    readByte += parser.readFloat(m_worldSet.m_time);
                }
                else if(parser.isNextTag("WS_AMBIENT"))
                {
                    readByte += parser.skipHeader();
                    readByte += parser.readFloat(m_worldSet.m_ambient);
                }
                else if(parser.isNextTag("WS_SUN_LIGHT"))
                {
                    readByte += parser.skipHeader();
                    readByte += parser.readFloat(m_worldSet.m_sunLight);
                }
                else {
                    break;
                }
            }
        }

        //vss section
        else if(parser.isNextTag("VSS_SECTION"))
        {
            readByte += parser.readHeader();
            readByte += parser.readByteArray(m_vss_section, parser.nodeLen());
        }

        //dirictory
        else if(parser.isNextTag("DIRICTORY"))
        {
            readByte += parser.readHeader();
            readByte += parser.readByteArray(m_directory, parser.nodeLen());
        }

        //dirictory elements
        else if(parser.isNextTag("DIRICTORY_ELEMENTS"))
        {
            readByte += parser.readHeader();
            readByte += parser.readByteArray(m_directoryElements, parser.nodeLen());
        }

        //object section
        else if(parser.isNextTag("OBJECT_SECTION"))
        {
            uint objSecLen = parser.nodeLen();
            uint readSecByte(0);
            readByte += parser.skipHeader(); // object section
            while(readSecByte < objSecLen)
            {
                if(parser.isNextTag("OBJECT"))
                {
                    CWorldObj* obj = new CWorldObj();
                    readSecByte += parser.skipHeader(); // "OBJECT";
                    readSecByte += obj->deserialize(parser);
                    addNode(obj);
                }
                else if (parser.isNextTag("LEVER"))
                {
                    CLever* lever = new CLever();
                    readSecByte += parser.skipHeader(); // "LEVER";
                    readSecByte += lever->deserialize(parser);
                    addNode(lever);
                }
                else if (parser.isNextTag("UNIT"))
                {
                    CUnit* unit = new CUnit();
                    readSecByte += parser.skipHeader(); // "UNIT";
                    unit->attachMob(this);
                    readSecByte += unit->deserialize(parser);
                    addNode(unit);
                }
                else if (parser.isNextTag("TORCH"))
                {
                    CTorch* torch = new CTorch();
                    readSecByte += parser.skipHeader(); // "TORCH";
                    readSecByte += torch->deserialize(parser);
                    addNode(torch);
                }
                else if (parser.isNextTag("MAGIC_TRAP"))
                {
                    CMagicTrap* trap = new CMagicTrap();
                    readSecByte += parser.skipHeader();  // "MAGIC_TRAP";
                    readSecByte += trap->deserialize(parser);
                    addNode(trap);
                }
                else if(parser.isNextTag("LIGHT"))
                {
                    CLight* light = new CLight();
                    readByte += parser.skipHeader(); // "LIGHT";
                    readByte += light->deserialize(parser);
                    addNode(light);
                }
                else if(parser.isNextTag("SOUND"))
                {
                    CSound* sound = new CSound();
                    readByte += parser.skipHeader(); // "SOUND";
                    readByte += sound->deserialize(parser);
                    addNode(sound);
                }
                else if(parser.isNextTag("PARTICL"))
                {
                    CParticle* particle = new CParticle();
                    readByte += parser.skipHeader(); // "PARTICL";
                    readByte += particle->deserialize(parser);
                    addNode(particle);
                }
                else {
                    auto a = parser.nextTag();
                    break;
                }
            }
            Q_ASSERT(readSecByte <= objSecLen);
            readByte += readSecByte;
        }
        else if (parser.isNextTag("LIGHT_SECTION"))
        {
            Q_ASSERT("light section" && false);
            readByte += parser.skipTag();
        }
        else if (parser.isNextTag("PARTICL_SECTION"))
        {
            Q_ASSERT("particle section" && false);
            readByte += parser.skipTag();
        }
        else if (parser.isNextTag("SOUND_SECTION"))
        {
            Q_ASSERT("sound section" && false);
            readByte += parser.skipTag();
        }
        else if(parser.isNextTag("AI_GRAPH"))
        {
            readByte += parser.readHeader();
            readByte += parser.readAiGraph(m_aiGraph, parser.nodeLen());
        }
        else
        {
            auto a = parser.nextTag();
            Q_ASSERT(parser.nextTag() == "ROOT");
            break;
        }
    }
    return true;
}

void CMob::updateObjects()
{
    QSet<QString> aModelName;
    QSet<QString> aTextureName;
    for(auto& node: m_aNode)
    {
        aModelName.insert(node->modelName());
        aTextureName.insert(node->textureName());
    }
    //preload figures
    m_view->objList()->loadFigures(aModelName);
    m_view->texList()->loadTexture(aTextureName.toList());
    QString texName;
    for(auto& node: m_aNode)
    {
        node->updateFigure(m_view->objList()->getFigure(node->modelName()));
        node->updateVisibleParts();
        texName = node->textureName();
        node->setTexture(m_view->texList()->texture(texName));
    }
}

void CMob::delNodes()
{
    for (auto& node: m_aNodeSelected)
        delete node;

    m_aNodeSelected.clear();
}

void CMob::readMob(QFileInfo &path)
{
    if (!path.exists())
        return;

    QString m_filePath = path.filePath();
    QFile file(m_filePath);
    try
    {
        file.open(QIODevice::ReadOnly);
        if (file.error() != QFile::NoError)
        {
            qDebug() << file.fileName() << " Error while open mob-file";
            return;
        }

        deserialize(file.readAll());
        file.close();
    }
    catch (std::exception ex)
    {
        qDebug() << ex.what();
        file.close();
    }

    updateObjects();
}

