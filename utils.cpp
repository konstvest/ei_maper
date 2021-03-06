#include <QDebug>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QtMath>
#include <QTextCodec>

#include "utils.h"

void util::formatStream(QDataStream& stream)
{
    stream.setByteOrder(QDataStream::ByteOrder::LittleEndian);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
}

void util::qNormalizeAngle(int& angle)
{
    if (angle > 360)
        angle -= 360;
    else if (angle < 0)
        angle = 360 - angle;
}

void util::normalizeAngle(float& angle)
{
    if (angle > 360)
        angle -= 360;
    else if (angle < -360)
        angle += 360;
}

void util::getCirclePoint(QVector<QVector3D>& aPoint, const QVector3D centr, const double radius, const int nPoint)
{
    Q_ASSERT(nPoint > 0);
    aPoint.resize(nPoint + 1);

    //double step = 2.0 * M_PI * radius / (nPoint + 1);
    const double rad = M_PI/180;
    const double step = 360/nPoint * rad;
    double angle = 0.0;
    QVector3D point(.0, .0, .0);
    for (int i(0); i<nPoint; ++i)
    {
        point.setX(float(double(centr.x()) + radius * qCos(angle))); // x:=xc+r*cos(u);
        point.setY(float(double(centr.y()) + radius * qSin(angle))); // y:=yc+r*sin(u);
        point.setZ(centr.z());  // writeln(i,' x=',x:7:2,'  y=',y:7:2);
        angle += step; // u:=u+a;
        aPoint[i] = point;
    }
    aPoint.back() = aPoint.first();
}

void util::splitByLen(QVector<QVector3D>& aPoint, float len)
{
    QVector<QVector3D> aSource = aPoint;
    aPoint.clear();
    QVector3D dir(.0, .0, .0);
    int count(0);
    for (int i(0), n(aSource.size()); i< n; ++i)
    {
        aPoint.append(aSource[i]);
        if(i < n-1)
        {
            dir = aSource[i+1] - aSource[i];
            count = int(dir.length()/len) - 1;
            dir.normalize();
            for(int j(0); j<count; ++j)
                aPoint.append(aSource[i] + dir*len*j);
        }
    }
}

bool isEqual(const double& a, const double& b, double Eps)
{
    return (qAbs(a - b) < Eps);
}

void util::CMobParser::initTypes()
{
    m_aType[0]=          {"ROOT", eRecord};

    m_aType[7680]=       {"VSS_SECTION", eRecord};
    m_aType[7681]=       {"VSS_TRIGER", eRecord};
    m_aType[7682]=       {"VSS_CHECK", eRecord};
    m_aType[7683]=       {"VSS_PATH", eRecord};
    m_aType[7684]=       {"VSS_ID", eDword};
    m_aType[7685]=       {"VSS_RECT", eRectangle};
    m_aType[7686]=       {"VSS_SRC_ID", eDword};
    m_aType[7687]=       {"VSS_DST_ID", eDword};
    m_aType[7688]=       {"VSS_TITLE", eString};
    m_aType[7689]=       {"VSS_COMMANDS", eString};
    m_aType[7690]=       {"VSS_ISSTART", eByte};
    m_aType[7691]=       {"VSS_LINK", eRecord};
    m_aType[7692]=       {"VSS_GROUP", eString};
    m_aType[7693]=       {"VSS_IS_USE_GROUP", eByte};
    m_aType[7694]=       {"VSS_VARIABLE", eRecord};
    m_aType[7695]=       {"VSS_BS_CHECK", eStringArray};
    m_aType[7696]=       {"VSS_BS_COMMANDS", eStringArray};
    m_aType[7697]=       {"VSS_CUSTOM_SCRIPT", eString};

    m_aType[40960]=      {"OBJECT_DB_FILE", eRecord};

    m_aType[43984]=      {"WORLD_SET", eRecord};
    m_aType[43985]=      {"WS_WIND_DIR", ePlot};
    m_aType[43986]=      {"WS_WIND_STR", eFloat};
    m_aType[43987]=      {"WS_TIME", eFloat};
    m_aType[43988]=      {"WS_AMBIENT", eFloat};
    m_aType[43989]=      {"WS_SUN_LIGHT", eFloat};

    m_aType[43520]=      {"LIGHT_SECTION", eNull};
    m_aType[43521]=      {"LIGHT", eRecord};
    m_aType[43522]=      {"LIGHT_RANGE", eFloat};
    m_aType[43523]=      {"LIGHT_NAME", eString};
    m_aType[43524]=      {"LIGHT_POSITION", ePlot};
    m_aType[43525]=      {"LIGHT_ID", eDword};
    m_aType[43526]=      {"LIGHT_SHADOW", eByte};
    m_aType[43527]=      {"LIGHT_COLOR", ePlot};
    m_aType[43528]=      {"LIGHT_COMMENTS", eString};

    m_aType[45056]=      {"OBJECT_SECTION", eRecord};
    m_aType[45057]=      {"OBJECT", eRecord};
    m_aType[45058]=      {"NID", eDword};
    m_aType[45059]=      {"OBJ_TYPE", eDword};
    m_aType[45060]=      {"OBJ_NAME", eString};
    m_aType[45061]=      {"OBJ_INDEX", eNull};
    m_aType[45062]=      {"OBJ_TEMPLATE", eString};
    m_aType[45063]=      {"OBJ_PRIM_TXTR", eString};
    m_aType[45064]=      {"OBJ_SEC_TXTR", eString};
    m_aType[45065]=      {"OBJ_POSITION", ePlot};
    m_aType[45066]=      {"OBJ_ROTATION", eQuaternion};
    m_aType[45067]=      {"OBJ_TEXTURE", eNull};
    m_aType[45068]=      {"OBJ_COMPLECTION", ePlot};
    m_aType[45069]=      {"OBJ_BODYPARTS", eStringArray};
    m_aType[45070]=      {"PARENT_TEMPLATE", eString};
    m_aType[45071]=      {"OBJ_COMMENTS", eString};
    m_aType[45072]=      {"OBJ_DEF_LOGIC", eNull};
    m_aType[45073]=      {"OBJ_PLAYER", eByte};
    m_aType[45074]=      {"OBJ_PARENT_ID", eDword};
    m_aType[45075]=      {"OBJ_USE_IN_SCRIPT", eByte};
    m_aType[45076]=      {"OBJ_IS_SHADOW", eByte};
    m_aType[45077]=      {"OBJ_R", eNull};
    m_aType[45078]=      {"OBJ_QUEST_INFO", eString};

    m_aType[49152]=      {"SC_OBJECT_DB_FILE", eNull};

    m_aType[52224]=      {"SOUND_SECTION", eNull};
    m_aType[52225]=      {"SOUND", eRecord};
    m_aType[52226]=      {"SOUND_ID", eDword};
    m_aType[52227]=      {"SOUND_POSITION", ePlot};
    m_aType[52228]=      {"SOUND_RANGE", eDword};
    m_aType[52229]=      {"SOUND_NAME", eString};
    m_aType[52230]=      {"SOUND_MIN", eDword};
    m_aType[52231]=      {"SOUND_MAX", eDword};
    m_aType[52232]=      {"SOUND_COMMENTS", eString};
    m_aType[52233]=      {"SOUND_VOLUME", eNull};
    m_aType[52234]=      {"SOUND_RESNAME", eStringArray};
    m_aType[52235]=      {"SOUND_RANGE2", eDword};
    m_aType[52237]=      {"SOUND_AMBIENT", eByte};
    m_aType[52238]=      {"SOUND_IS_MUSIC", eByte};

    m_aType[53248]=      {"PR_OBJECT_DB_FILE", eNull};

    m_aType[56576]=      {"PARTICL_SECTION", eNull};
    m_aType[56577]=      {"PARTICL", eRecord};
    m_aType[56578]=      {"PARTICL_ID", eDword};
    m_aType[56579]=      {"PARTICL_POSITION", ePlot};
    m_aType[56580]=      {"PARTICL_COMMENTS", eString};
    m_aType[56581]=      {"PARTICL_NAME", eString};
    m_aType[56582]=      {"PARTICL_TYPE", eDword};
    m_aType[56583]=      {"PARTICL_SCALE", eFloat};

    m_aType[57344]=      {"DIRICTORY", eRecord};
    m_aType[57345]=      {"FOLDER", eRecord};
    m_aType[57346]=      {"DIR_NAME", eString};
    m_aType[57347]=      {"DIR_NINST", eDword};
    m_aType[57348]=      {"DIR_PARENT_FOLDER", eDword};
    m_aType[57349]=      {"DIR_TYPE", eByte};

    m_aType[61440]=      {"DIRICTORY_ELEMENTS", eRecord};

    m_aType[65280]=      {"SEC_RANGE", eRecord};
    m_aType[65281]=      {"MAIN_RANGE", eRecord};
    m_aType[65282]=      {"RANGE", eRecord};
    m_aType[65285]=      {"MIN_ID", eDword};
    m_aType[65286]=      {"MAX_ID", eDword};

    m_aType[826366246]=  {"AI_GRAPH", eAiGraph};

    m_aType[2899242186]= {"SS_TEXT_OLD", eString};
    m_aType[2899242187]= {"SS_TEXT", eStringEncrypted};

    m_aType[3148611584]= {"LEVER", eRecord};
    m_aType[3148611585]= {"LEVER_SCIENCE_STATS", eNull};
    m_aType[3148611586]= {"LEVER_CUR_STATE", eByte};
    m_aType[3148611587]= {"LEVER_TOTAL_STATE", eByte};
    m_aType[3148611588]= {"LEVER_IS_CYCLED", eByte};
    m_aType[3148611589]= {"LEVER_CAST_ONCE", eByte};
    m_aType[3148611590]= {"LEVER_SCIENCE_STATS_NEW", eLeverStats};
    m_aType[3148611591]= {"LEVER_IS_DOOR", eByte};
    m_aType[3148611592]= {"LEVER_RECALC_GRAPH", eByte};

    m_aType[3149594624]= {"UNIT", eRecord};
    m_aType[3149594625]= {"UNIT_R", eNull};
    m_aType[3149594626]= {"UNIT_PROTOTYPE", eString};
    m_aType[3149594627]= {"UNIT_ITEMS", eNull};
    m_aType[3149594628]= {"UNIT_STATS", eUnitStats};
    m_aType[3149594629]= {"UNIT_QUEST_ITEMS", eStringArray};
    m_aType[3149594630]= {"UNIT_QUICK_ITEMS", eStringArray};
    m_aType[3149594631]= {"UNIT_SPELLS", eStringArray};
    m_aType[3149594632]= {"UNIT_WEAPONS", eStringArray};
    m_aType[3149594633]= {"UNIT_ARMORS", eStringArray};
    m_aType[3149594634]= {"UNIT_NEED_IMPORT", eByte};

    m_aType[3149660160]= {"UNIT_LOGIC", eRecord};
    m_aType[3149660161]= {"UNIT_LOGIC_AGRESSIV", eNull};
    m_aType[3149660162]= {"UNIT_LOGIC_CYCLIC", eByte};
    m_aType[3149660163]= {"UNIT_LOGIC_MODEL", eDword};
    m_aType[3149660164]= {"UNIT_LOGIC_GUARD_R", eFloat};
    m_aType[3149660165]= {"UNIT_LOGIC_GUARD_PT", ePlot};
    m_aType[3149660166]= {"UNIT_LOGIC_NALARM", eByte};
    m_aType[3149660167]= {"UNIT_LOGIC_USE", eByte};
    m_aType[3149660168]= {"UNIT_LOGIC_REVENGE", eNull};
    m_aType[3149660169]= {"UNIT_LOGIC_FEAR", eNull};
    m_aType[3149660170]= {"UNIT_LOGIC_WAIT", eFloat};
    m_aType[3149660171]= {"UNIT_LOGIC_ALARM_CONDITION", eByte};
    m_aType[3149660172]= {"UNIT_LOGIC_HELP", eFloat};
    m_aType[3149660173]= {"UNIT_LOGIC_ALWAYS_ACTIVE", eByte};
    m_aType[3149660174]= {"UNIT_LOGIC_AGRESSION_MODE", eByte};

    m_aType[3149725696]= {"GUARD_PT", eRecord};
    m_aType[3149725697]= {"GUARD_PT_POSITION", ePlot};
    m_aType[3149725698]= {"GUARD_PT_ACTION", eNull};

    m_aType[3149791232]= {"ACTION_PT", eRecord};
    m_aType[3149791233]= {"ACTION_PT_LOOK_PT", ePlot};
    m_aType[3149791234]= {"ACTION_PT_WAIT_SEG", eDword};
    m_aType[3149791235]= {"ACTION_PT_TURN_SPEED", eDword};
    m_aType[3149791236]= {"ACTION_PT_FLAGS", eByte};

    m_aType[3149856768]= {"TORCH", eRecord};
    m_aType[3149856769]= {"TORCH_STRENGHT", eFloat};
    m_aType[3149856770]= {"TORCH_PTLINK", ePlot};
    m_aType[3149856771]= {"TORCH_SOUND", eString};

    m_aType[3148546048]= {"MAGIC_TRAP", eRecord};
    m_aType[3148546049]= {"MT_DIPLOMACY", eDword};
    m_aType[3148546050]= {"MT_SPELL", eString};
    m_aType[3148546051]= {"MT_AREAS", eAreaArray};
    m_aType[3148546052]= {"MT_TARGETS", ePlot2DArray};
    m_aType[3148546053]= {"MT_CAST_INTERVAL", eDword};

    m_aType[3722304977]= {"DIPLOMATION", eRecord};
    m_aType[3722304978]= {"DIPLOMATION_FOF", eDiplomacy};
    m_aType[3722304979]= {"DIPLOMATION_PL_NAMES", eStringArray};

    m_aType[4294967295]= {"UNKNOWN", eUnknown};
}


util::CMobParser::CMobParser(QByteArray& data):
    m_stream(data)
{
    util::formatStream(m_stream);
    initTypes();
}

void util::CMobParser::decryptScript(QString& script, QByteArray& data, uint key)
{
    uint tmpKey;
    for (uint i(0); i < uint(data.length()); i++)
    {
        tmpKey = ((((key * 13) << 4) + key) << 8) - key;
        key += (tmpKey << 2) + 2531011;
        tmpKey = key >> 16;
        const char sym = char(data[i] ^ char(tmpKey));
        //todo: convert to qstringlist and utf-8? (mob.cpp writing has same code)
        script += sym;
    }
}

bool util::CMobParser::isNextTag(const char* tagname)
{
    m_stream.startTransaction();
    m_stream >> m_node;
    bool res = false;
    auto a = nodeName();
    if(nodeName() == tagname)
        res = true;

    m_stream.rollbackTransaction();
    return res;
}

uint util::CMobParser::nodeLen()
{
    return m_node.m_len-8;
}

uint util::CMobParser::skipTag()
{
    m_stream >> m_node;
    m_stream.device()->skip(m_node.m_len-8);
    return m_node.m_len;
}

QString util::CMobParser::nextTag()
{
    m_stream.startTransaction();
    m_stream >> m_node;
    m_stream.rollbackTransaction();
    return nodeName();
}

void util::CMobParser::checkTag(const char* tag)
{
    if(isNextTag(tag))
        skipHeader();
    else
        Q_ASSERT("Unexpected tag" && false);
}

uint util::CMobParser::skipHeader()
{
    m_stream.device()->skip(8);
    return 8;
}

uint util::CMobParser::readHeader()
{
    m_stream >> m_node;
    return 8;
}

uint util::CMobParser::readAiGraph(QByteArray& data, uint len)
{
    data = m_stream.device()->read(len);
    return len;
}

uint util::CMobParser::readByteArray(QByteArray& data, uint len)
{
    data = m_stream.device()->read(len);
    return len;
}

uint util::CMobParser::readByte(char& data)
{
    m_stream.device()->getChar(&data);
    return 1;
}

uint util::CMobParser::readBool(bool& data)
{
    m_stream >> data;
    return 1;
}

uint util::CMobParser::readDiplomacy(QVector<QVector<uint>>& data)
{
    uint dipl[32*32];
    m_stream.readRawData(reinterpret_cast<char*>(&dipl), sizeof(dipl));
    data.resize(32);
    for(int i(0); i < 32; ++i)
        for(int j(0); j < 32;++j)
            data[i].append(dipl[j+i*32]);

    return 32*32*4;
}

uint util::CMobParser::readDword(uint& data)
{
    m_stream >> data;
    return 4;
}
uint util::CMobParser::readDword(int& data)
{
    m_stream >> data;
    return 4;
}

uint util::CMobParser::readFloat(float& data)
{
    m_stream >> data;
    return 4;
}

uint util::CMobParser::readPlot(QVector3D& data)
{
    float val;
    m_stream >> val;
    data.setX(val);
    m_stream >> val;
    data.setY(val);
    m_stream >> val;
    data.setZ(val);
    return 3*sizeof(float);
}

uint util::CMobParser::readAreaArray(QVector<SArea>& data)
{
    uint size;
    m_stream >> size;
    Q_ASSERT(size <=1);
    QVector2D pointTo;
    uint radius;
    float val;
    for(uint i(0); i<size; ++i)
    {
        m_stream >> val;
        pointTo.setX(val);
        m_stream >> val;
        pointTo.setY(val);
        m_stream >> radius;
        SArea area{pointTo, radius};
        data.append(area);
    }
    return 4+12*size;
}

uint util::CMobParser::readPlot2DArray(QVector<QVector2D>& data)
{
    uint size;
    m_stream >> size;
    Q_ASSERT(size <=1);
    QVector2D pointTo;
    float val;
    for(uint i(0); i<size; ++i)
    {
        m_stream >> val;
        pointTo.setX(val);
        m_stream >> val;
        pointTo.setY(val);
        data.append(pointTo);
    }
    //todo: check for multiPoints
//    for (uint i(0); i<size; ++i)
//    {
//        m_stream >> pointTo[0] >> pointTo[1];
//    }
    //field size does not match the number of bytes read. Apparently, the error inside the EI read\writer
    //maped contains rightly field len, but incorrect len of object data
    //readLen +=4; //wtf? mobreversingtool show 4 bytes over
    return 4+8*size; // TODO return bytes that read
}

uint util::CMobParser::readQuaternion(QVector4D& data)
{
    float val;
    m_stream >> val;
    data.setW(val);
    m_stream >> val;
    data.setX(val);
    m_stream >> val;
    data.setY(val);
    m_stream >> val;
    data.setZ(val);
    return 4*sizeof(float);
}
uint util::CMobParser::readRectangle(SRectangle& data)
{
    m_stream >> data.m_minX >> data.m_maxX >> data.m_minY >> data.m_maxY;
    return 16;
}

uint util::CMobParser::readString(QString& data, uint len)
{
    QByteArray str = m_stream.device()->read(len);
    data = QString(str);
    return len;
}

uint util::CMobParser::readStringArray(QVector<QString>& data)
{
    uint blockLen(0);
    uint records;
    m_stream >> records;
    blockLen += 4;
    for(uint i(0); i<records; ++i)
    {
        SNode line;
        m_stream >> line;
        blockLen += line.m_len;
        data.append(QString(m_stream.device()->read(line.m_len-8)));
    }
    return blockLen;
}

uint util::CMobParser::readStringEncrypted(QString& data, uint len)
{
    uint key;
    m_stream >> key;
    QByteArray str = m_stream.device()->read(len-4);
    decryptScript(data, str, key);
    return len + 4;
}

uint util::CMobParser::readUnitStats(QSharedPointer<SUnitStat>& data, uint len)
{
    data = QSharedPointer<SUnitStat>(new SUnitStat);
    m_stream.readRawData(reinterpret_cast<char*>(data.get()), sizeof(SUnitStat));
    return len;
}

