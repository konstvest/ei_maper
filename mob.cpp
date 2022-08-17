#include <QDebug>
#include <QSet>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTextCodec>
#include <QTextDecoder>
#include <QDir>
#include <QMessageBox>

#include "mob.h"
#include "utils.h"
#include "objects\lever.h"
#include "objects\light.h"
#include "objects\magictrap.h"
#include "objects\particle.h"
#include "objects\sound.h"
#include "objects\torch.h"
#include "objects\worldobj.h"
#include "objects\unit.h"
#include "view.h"
#include "resourcemanager.h"
#include "landscape.h"
#include "settings.h"
#include "progressview.h"
#include "log.h"
#include "scene.h"

CMob::CMob():
    m_view(nullptr)
    ,m_pProgress(nullptr)
  ,m_bDurty(false)
{
    m_aNode.clear();
    m_worldSet.bInit = false;
    m_order = eEMobOrderSecondary;
}

CMob::~CMob()
{
    for (auto& node: m_aNode)
        delete node;
}

void CMob::attach(CView *view, CProgressView *pProgress)
{
    m_view = view;
    m_pProgress = pProgress;
}

void CMob::init()
{
    m_diplomacyFoF.resize(32);
    for(int i(0); i<m_diplomacyFoF.size(); ++i)
        m_diplomacyFoF[i].resize(32);
}

bool CMob::deserialize(QByteArray data)
{
    ei::log(eLogInfo, "Start read mob");
    //todo: global check len of nodes
    uint readByte(0);
    util::CMobParser parser(data);
    double step = double(50)/3;
    if (parser.isNextTag("OBJECT_DB_FILE"))
        readByte += parser.skipHeader();
    else
        return false;

    if(parser.isNextTag("SC_OBJECT_DB_FILE"))
    {
        readByte += parser.skipHeader();
        //readByte += parser.readHeader();
        m_order = eEMobOrderSecondary;
    }
    else if (parser.isNextTag("PR_OBJECT_DB_FILE"))
    {
        readByte += parser.skipHeader();
        m_order = eEMobOrderPrimary;
    }

    while(true)
    {
        if(parser.isNextTag("SS_TEXT"))
        {
            readByte += parser.readHeader();
            readByte += parser.readStringEncrypted(m_script, m_scriptKey, parser.nodeLen());
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
            m_worldSet.bInit = true;
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
            m_pProgress->update(step);
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
            m_pProgress->update(step);
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
            Q_ASSERT(parser.nextTag() == "ROOT"); // zone3obr.mob has current code
            break;
        }
    }
    m_pProgress->update(step);
    ei::log(eLogInfo, "End read mob");
    return true;
}

void CMob::updateObjects()
{
    ei::log(eLogInfo, "Start update objects");
    for(auto& node: m_aNode)
    {
        node->loadFigure();
        node->loadTexture();
    }
    CLandscape::getInstance()->projectPositions(m_aNode);
    m_pProgress->update(50);
    ei::log(eLogInfo, "End update objects");
}

void CMob::delNodes()
{
    QVector<int> aInd;
    for (int i(0); i < m_aNode.size(); ++i)
    {
        if(m_aNode[i]->nodeState() & ENodeState::eSelect)
            aInd.append(i);
    }
    //aInd.
    for (auto i = aInd.crbegin(); i != aInd.crend(); ++i)
    {
        delete m_aNode[*i];
        m_aNode.removeAt(*i);
    }
}

CNode *CMob::nodeByMapId(uint id)
{
    CNode* pNode = nullptr;
    foreach(pNode, m_aNode)
    {
        if(pNode->mapId()==id)
            break;
    }
    return pNode;
}

QString CMob::mobName()
{
    return m_filePath.fileName();
}

void CMob::getPatrolHash(int &unitMapIdOut, int &pointIdOut, CPatrolPoint *pPoint)
{
    CNode* pNode = nullptr;
    foreach(pNode, m_aLogicNode)
    {
        if(pNode->nodeType() == eUnit)
        {
            CUnit* pUnit = dynamic_cast<CUnit*>(pNode);
            int index = pUnit->getPatrolId(pPoint);
            if(index >=0)
            {
                unitMapIdOut = pUnit->mapId();
                pointIdOut = index;
                return;
            }
        }
    }
    return;
}

void CMob::getViewHash(int &unitMapIdOut, int &pointIdOut, int &viewIdOut, CLookPoint *pPoint)
{
    CNode* pNode = nullptr;
    foreach(pNode, m_aLogicNode)
    {
        if(pNode->nodeType() != eUnit)
            continue;

        int parentPatrol;
        int parentView;
        CUnit* pUnit = dynamic_cast<CUnit*>(pNode);
        pUnit->getViewId(parentPatrol, parentView, pPoint);
        if(parentPatrol >=0)
        {
            unitMapIdOut = pUnit->mapId();
            pointIdOut = parentPatrol;
            viewIdOut = parentView;
            return;
        }
    }
    return;
}

int CMob::getPatrolId(uint unitMapId, CPatrolPoint* pPoint)
{
    CNode* pNode = nullptr;
    foreach(pNode, m_aLogicNode)
    {
        if(pNode->mapId() == unitMapId && pNode->nodeType() == eUnit)
        {
            CUnit* pUnit = dynamic_cast<CUnit*>(pNode);
            return pUnit->getPatrolId(pPoint);
        }
    }
    return -1;
}

void CMob::createPatrolByHash(QString hash)
{
    QStringList list = hash.split(".");
    if(list.size()<2)
    {
        ei::log(eLogWarning, "Creating logic point failed. hash incorrect" + hash);
    }

    CNode* pNode = nullptr;
    foreach(pNode, m_aLogicNode)
    {
        if(pNode->nodeType() == eUnit && pNode->mapId() == list[0].toUInt())
        {
            CUnit* pUnit = dynamic_cast<CUnit*>(pNode);
            if(list.size()==2)
                pUnit->createPatrolByIndex(list[1].toInt());
            else if(list.size()==3)
                pUnit->createViewByIndex(list[1].toInt(), list[2].toInt());

            break;
        }
    }

    logicNodesUpdate();
}

void CMob::undo_createPatrolByHash(QString hash)
{
    QStringList list = hash.split(".");
    if(list.size()==2) // patrol point
    {
        CNode* pNode = nullptr;
        foreach(pNode, m_aLogicNode)
        {
            if(pNode->nodeType() == eUnit && pNode->mapId() == list[0].toUInt())
            {
                CUnit* pUnit = dynamic_cast<CUnit*>(pNode);
                pUnit->undo_createPatrolByIndex(list[1].toInt());
                break;
            }
        }
    }
    else if(list.size()==3)
    {
        CNode* pNode = nullptr;
        foreach(pNode, m_aLogicNode)
        {
            if(pNode->nodeType() == eUnit && pNode->mapId() == list[0].toUInt())
            {
                CUnit* pUnit = dynamic_cast<CUnit*>(pNode);
                pUnit->undo_createViewByIndex(list[1].toInt(), list[2].toInt());
                break;
            }
        }
    }
    logicNodesUpdate();
}

CPatrolPoint *CMob::patrolPointById(int unitId, int patrolId)
{
    CNode* pNode = nullptr;
    foreach(pNode, m_aLogicNode)
    {
        if(pNode->nodeType() == eUnit && pNode->mapId() == (uint)unitId)
        {
            CUnit* pUnit = dynamic_cast<CUnit*>(pNode);
            return pUnit->patrolByIndex(patrolId);
        }
    }
    Q_ASSERT(false && "cant find patrol point by unit map id");
    return nullptr;
}

CLookPoint *CMob::viewPointById(int unitId, int patrolId, int viewId)
{
    CPatrolPoint* pPoint = patrolPointById(unitId, patrolId);
    return pPoint->viewByIndex(viewId);
}

void CMob::getTrapZoneHash(int &unitMapIdOut, int &zoneIdOut, CActivationZone *pZone)
{
    CNode* pNode = nullptr;
    foreach(pNode, m_aLogicNode)
    {
        if(pNode->nodeType() == eMagicTrap)
        {
            CMagicTrap* pTrap = dynamic_cast<CMagicTrap*>(pNode);
            int index = pTrap->getZoneId(pZone);
            if(index >=0)
            {
                unitMapIdOut = pTrap->mapId();
                zoneIdOut = index;
                return;
            }
        }
    }
    return;
}

CActivationZone *CMob::actZoneById(int trapId, int zoneId)
{
    CNode* pNode = nullptr;
    foreach(pNode, m_aLogicNode)
    {
        if(pNode->nodeType() == eMagicTrap && pNode->mapId() == (uint)trapId)
        {
            CMagicTrap* pTrap = dynamic_cast<CMagicTrap*>(pNode);
            return pTrap->actZoneById(zoneId);
        }
    }
    Q_ASSERT(false && "cant find act zone by trap map id");
    return nullptr;
}

void CMob::getTrapCastHash(int &unitMapIdOut, int &pointIdOut, CTrapCastPoint* pCast)
{
    CNode* pNode = nullptr;
    foreach(pNode, m_aLogicNode)
    {
        if(pNode->nodeType() == eMagicTrap)
        {
            CMagicTrap* pTrap = dynamic_cast<CMagicTrap*>(pNode);
            int index = pTrap->getCastPointId(pCast);
            if(index >=0)
            {
                unitMapIdOut = pTrap->mapId();
                pointIdOut = index;
                return;
            }
        }
    }
    return;
}

CTrapCastPoint *CMob::trapCastById(int trapId, int pointId)
{
    CNode* pNode = nullptr;
    foreach(pNode, m_aLogicNode)
    {
        if(pNode->nodeType() == eMagicTrap && pNode->mapId() == (uint)trapId)
        {
            CMagicTrap* pTrap = dynamic_cast<CMagicTrap*>(pNode);
            return pTrap->castPointById(pointId);
        }
    }
    Q_ASSERT(false && "cant find cast point by trap map id");
    return nullptr;
}

uint CMob::trapIdByPoint(CActivationZone *pZone)
{
    CNode* pNode = nullptr;
    foreach(pNode, m_aLogicNode)
    {
        if(pNode->nodeType() == eMagicTrap)
        {
            CMagicTrap* pTrap = dynamic_cast<CMagicTrap*>(pNode);
            int index = pTrap->actZones().indexOf(pZone);
            if(index >=0)
                return pTrap->mapId();
        }
    }
    Q_ASSERT(false);
    return 0;
}

uint CMob::trapIdByPoint(CTrapCastPoint *pPoint)
{
    CNode* pNode = nullptr;
    foreach(pNode, m_aLogicNode)
    {
        if(pNode->nodeType() == eMagicTrap)
        {
            CMagicTrap* pTrap = dynamic_cast<CMagicTrap*>(pNode);
            int index = pTrap->castPoints().indexOf(pPoint);
            if(index >=0)
                return pTrap->mapId();
        }
    }
    Q_ASSERT(false);
    return 0;
}

CNode* CMob::createNode(QJsonObject data)
{
    auto wo = data;
    if (data.find("World object") != data.end())
        wo = wo["World object"].toObject();

    auto base = wo["Base object"].toObject();

    CNode* pNode = nullptr;
    ENodeType type = (ENodeType)base["Node type"].toInt(0);
    if (type == ENodeType::eUnknown)
    {
        qDebug() << "cant recognize type of obj";
        return pNode;
    }

    switch (type)
    {
    case ENodeType::eUnit:
    {
        pNode = new CUnit(data);
        pNode->loadFigure();
        pNode->loadTexture();
        break;
    }
    case ENodeType::eTorch:
    {
        pNode = new CTorch(data);
        pNode->loadFigure();
        pNode->loadTexture();
        break;
    }
    case ENodeType::eMagicTrap:
    {
        pNode = new CMagicTrap(data);
        break;
    }
    case ENodeType::eLever:
    {
        pNode = new CLever(data);
        pNode->loadFigure();
        pNode->loadTexture();
        break;
    }
    case ENodeType::eLight:
    {
        pNode = new CLight(data);
        break;
    }
    case ENodeType::eSound:
    {
        pNode = new CSound(data);
        break;
    }
    case ENodeType::eParticle:
    {
        pNode = new CParticle(data);
        break;
    }
    case ENodeType::eWorldObject:
    {
        pNode = new CWorldObj(data);
        pNode->loadFigure();
        pNode->loadTexture();
        break;
    }
    default:
    {
        Q_ASSERT("unknown node type" && false);
        break;
    }
    }

    if(nullptr == pNode)
    {
        Q_ASSERT("Failed to create new node" && false);
    }

    addNode(pNode);
    logicNodesUpdate();

    if(nullptr == pNode)
        return nullptr;

    //set map ID
    bool bFound = false;
    QVector<SRange> arrRange;
    for(auto& range : m_aMainRange)
        arrRange.append(SRange(range));
    for(auto& range : m_aSecRange)
        arrRange.append(SRange(range));

    for(auto& range : arrRange)
    {
        if(bFound)
            break;

        for(uint i(range.minRange); i<range.maxRange; ++i)
        {
            if(isFreeMapId(i))
            {
                pNode->setMapId(i);
                qDebug() << "id found:" << i;
                bFound = true;
                break;
            }
        }
    }

    if(!bFound)
    { // try to set any free number
        uint freeId(1000);
        QVector<uint> arrId;
        arrId.resize(m_aNode.size());

        for (int i(0); i<m_aNode.size(); ++i)
            arrId[i] = m_aNode[i]->mapId();

        for (; freeId<100000; ++freeId)
        {
            if(!arrId.contains(freeId))
            {
                pNode->setMapId(freeId);
                break;
            }
        }
    }
    pNode->setState(ENodeState::eSelect);


    return pNode;
}

void CMob::undo_createNode(uint mapId)
{
    CNode* pNode = nullptr;
    foreach(pNode, m_aNode)
    {
        if(pNode->mapId() == mapId)
        {
            //m_aDeletedNode.append(pNode);
            m_aNode.removeAt(m_aNode.indexOf(pNode));
            ei::log(eLogDebug, QString("undo create node with %1 map ID").arg(pNode->mapId()));
            return;
        }
    }
}

QList<CNode*>& CMob::nodes()
{
    return m_aNode;
}

QList<CNode *> &CMob::logicNodes()
{
    //collect logic nodes
    if(m_aLogicNode.isEmpty()) //can be optimized but need to control add\delete nodes/patrol points\view
        logicNodesUpdate();


    return m_aLogicNode;
}

void CMob::logicNodesUpdate()
{
    m_aLogicNode.clear();

    CNode* pNode = nullptr;
    foreach(pNode, m_aNode)
    {
        ENodeType type = pNode->nodeType();
        switch(type)
        {
        case ENodeType::eUnit:
        {
            m_aLogicNode.append(pNode);
            CUnit* pUnit = dynamic_cast<CUnit*>(pNode);
            Q_ASSERT(pUnit);
            pUnit->collectLogicNodes(m_aLogicNode);
            break;
        }
        case ENodeType::eMagicTrap:
        {
            m_aLogicNode.append(pNode);
            CMagicTrap* pTrap = dynamic_cast<CMagicTrap*>(pNode);
            pTrap->collectLogicNodes(m_aLogicNode);
            break;
        }
        default:
            break;
        }
    }
}

void CMob::deleteNode(uint mapId)
{
    CNode* pNode = nullptr;
    foreach(pNode, m_aNode)
    {
        if(pNode->mapId() == mapId)
        {
            pNode->setState(ENodeState::eDraw); // for restoring draw state
            m_aDeletedNode.append(pNode);
            m_aNode.removeOne(pNode);
            ei::log(eLogDebug, QString("delete node with %1").arg(pNode->mapId()));
        }
    }
}

void CMob::undo_deleteNode(uint mapId)
{
    CNode* pNode = nullptr;
    foreach(pNode, m_aDeletedNode)
    {
        if(pNode->mapId() == mapId)
        {
            m_aNode.append(pNode);
            m_aDeletedNode.removeOne(pNode);
            ei::log(eLogDebug, QString("node with %1 restored").arg(pNode->mapId()));
            break;
        }
    }
}

void CMob::readMob(QFileInfo &path)
{
    if (!path.exists())
        return;

    m_filePath = path;
    QFile file(m_filePath.filePath());
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
    logicNodesUpdate();
}

void CMob::checkUniqueId(QSet<uint> &aId)
{
    for(auto& node : m_aNode)
    {
        if(aId.contains(node->mapId()))
            QMessageBox::warning(nullptr, "Map checker", "Object with ID " + QString::number(node->mapId()) + " has duplicate\nThis can occurs crash in Evil Islands.\nPlease set unique ID");
        else
            aId.insert(node->mapId());
    }
}

QString CMob::getAuxDirName()
{
    return "aux_files";
}

/// mob - inout. json object of Mob file
/// file - in. map fileName
/// key - type of data
void CMob::writeData(QJsonObject& mob, const QFileInfo& file, const QString key, const QString value)
{
    const QString fileRelPath = getAuxDirName() + QDir::separator() + file.baseName() + '.' + key + ".txt";
    const QFileInfo auxFolder(file.dir().path() + QDir::separator() + getAuxDirName());
    if (!auxFolder.exists())
    {
        QDir().mkdir(auxFolder.filePath());
    }

    QFile temp(file.dir().path() + QDir::separator() + fileRelPath);

    try
    {
        temp.open(QIODevice::WriteOnly);
        if (temp.error() != QFile::NoError)
        {
            qDebug() << file.fileName() << " Error writing script";
            return;
        }

        //convert string to unicode(win-1251 to utf-8)
        QTextCodec* defaultTextCodec = QTextCodec::codecForName("Windows-1251");
        QTextDecoder *decoder = new QTextDecoder(defaultTextCodec);
        QString str = decoder->toUnicode(value.toLatin1());

        QTextStream stream(&temp);
        stream << str;
        temp.close();
    }
    catch (std::exception ex)
    {
        qDebug() << ex.what();
        temp.close();
    }
    mob.insert(key, fileRelPath);
}

/// mob - inout. json object of Mob file
/// file - in. map fileName
/// key - type of data
void CMob::writeData(QJsonObject& mob, const QFileInfo& file, const QString key, QByteArray& value)
{
    const QString fileRelPath = getAuxDirName() + QDir::separator() + file.baseName() + '.' + key;
    const QFileInfo auxFolder(file.dir().path() + QDir::separator() + getAuxDirName());
    if (!auxFolder.exists())
    {
        QDir().mkdir(auxFolder.filePath());
    }

    QFile temp(file.dir().path() + QDir::separator() + fileRelPath);
    try
    {
        temp.open(QIODevice::WriteOnly);
        if (temp.error() != QFile::NoError)
        {
            qDebug() << file.fileName() << " Error writing aux file";
            return;
        }
        temp.write(value);
        temp.close();
    }
    catch (std::exception ex)
    {
        qDebug() << ex.what();
        temp.close();
    }
    mob.insert(key, fileRelPath);
}

bool CMob::isFreeMapId(uint id)
{
    CNode* pNode = nullptr;
    foreach(pNode, m_aNode)
    {
        if(pNode->mapId() == id)
            return false;
    }
    return true;
}

/// file - inout. mob file
void CMob::serializeJson(const QFileInfo& file)
{
    //ranges
    QJsonArray aRange;
    auto prepareRange = [&aRange](QVector<SRange>& rangeList)
    {
        aRange = QJsonArray();
        for(auto& mainRange : rangeList)
        {
            QJsonObject range;
            range.insert("min", QJsonValue::fromVariant(mainRange.minRange));
            range.insert("max", QJsonValue::fromVariant(mainRange.maxRange));
            aRange.append(range);
        }
    };

    QJsonObject rangeObj;
    prepareRange(m_aMainRange);
    rangeObj.insert("Main ranges", aRange);
    prepareRange(m_aSecRange);
    rangeObj.insert("Sec ranges", aRange);

    QJsonArray diplomacyTable;
    for(auto& line: m_diplomacyFoF)
    {
        QString rowStr;
        for(auto& row: line)
          rowStr += QString::number(row);
        diplomacyTable.append(rowStr);

    }

    QJsonArray aDiplomacyName;
    for(auto& name : m_aDiplomacyFieldName)
    {
        aDiplomacyName.append(name);
    }

    QJsonObject worldSetObj;
    QJsonArray windDir;
    windDir.append(QJsonValue::fromVariant(m_worldSet.m_windDirection.x()));
    windDir.append(QJsonValue::fromVariant(m_worldSet.m_windDirection.y()));
    windDir.append(QJsonValue::fromVariant(m_worldSet.m_windDirection.z()));
    worldSetObj.insert("Wind direction", windDir);
    worldSetObj.insert("Wind strength", QJsonValue::fromVariant(m_worldSet.m_windStrength));
    worldSetObj.insert("Time", QJsonValue::fromVariant(m_worldSet.m_time));
    worldSetObj.insert("Ambient", QJsonValue::fromVariant(m_worldSet.m_ambient));
    worldSetObj.insert("Sun Light", QJsonValue::fromVariant(m_worldSet.m_sunLight));

    QJsonObject mob;
    mob.insert("Ranges", rangeObj);

    //scripts
    writeData(mob, file, "Script", m_script);
    writeData(mob, file, "Old_script", m_textOld);

    mob.insert("Diplomacy Names", aDiplomacyName);
    mob.insert("Diplomacy table", diplomacyTable);
    mob.insert("World set", worldSetObj);

    //binary aux data
    writeData(mob, file, "vss", m_vss_section);
    writeData(mob, file, "dir", m_directory);
    writeData(mob, file, "dirElem", m_directoryElements);
    writeData(mob, file, "graph", m_aiGraph);

    //units
    QJsonArray unitArr;
    for (auto& node: m_aNode)
    {
        QJsonObject unitObj;
        node->serializeJson(unitObj);
        unitArr.append(unitObj);
    }

    mob.insert("Objects", unitArr);

    QJsonDocument doc(mob);
    QFile f(file.absoluteFilePath());
    if (!f.open(QIODevice::WriteOnly)) {
        Q_ASSERT("Couldn't open option file." && false);
        return;
    }
    f.write(doc.toJson(QJsonDocument::JsonFormat::Indented));
    f.close();
}

void CMob::serializeMob(QByteArray& data)
{
    ei::log(eLogInfo, "Start write mob");
    uint writeByte(0);
    util::CMobParser parser(data, true);
    //header "OBJECT_DB_FILE"
    writeByte += parser.startSection("OBJECT_DB_FILE");

    switch (m_order) {
    case eEMobOrderPrimary:
        writeByte += parser.startSection("PR_OBJECT_DB_FILE");
        break;
    case eEMobOrderSecondary:
        writeByte += parser.startSection("SC_OBJECT_DB_FILE");
        break;
    }
    parser.endSection(); //"PR/SC_OBJECT_DB_FILE"

    if (!m_script.isEmpty())
    {
        writeByte += parser.startSection("SS_TEXT");
        writeByte += parser.writeStringEncrypted(m_script, m_scriptKey);
        parser.endSection(); //SS_TEXT
    }


    if (!m_textOld.isEmpty())
    {
        writeByte += parser.startSection("SS_TEXT_OLD");
        writeByte += parser.writeString(m_textOld);
        parser.endSection(); //SS_TEXT_OLD
    }


    if (!m_aMainRange.isEmpty())
    {//main range
        writeByte += parser.startSection("MAIN_RANGE");
        for (const auto& elem: m_aMainRange)
        {
            writeByte += parser.startSection("RANGE");
            {
                writeByte += parser.startSection("MIN_ID");
                writeByte += parser.writeDword(elem.minRange);
                parser.endSection();

                writeByte += parser.startSection("MAX_ID");
                writeByte += parser.writeDword(elem.maxRange);
                parser.endSection();
            }
            parser.endSection();
        }
        parser.endSection();
    }

    if (!m_aSecRange.isEmpty())
    {
        writeByte += parser.startSection("SEC_RANGE");
        for (const auto& elem: m_aSecRange)
        {
            writeByte += parser.startSection("RANGE");
            {
                writeByte += parser.startSection("MIN_ID");
                writeByte += parser.writeDword(elem.minRange);
                parser.endSection();

                writeByte += parser.startSection("MAX_ID");
                writeByte += parser.writeDword(elem.maxRange);
                parser.endSection();
            }
            parser.endSection(); // RANGE
        }
        parser.endSection();//sec range
    }

    if (!m_diplomacyFoF.empty() || !m_aDiplomacyFieldName.isEmpty())
    {
        writeByte += parser.startSection("DIPLOMATION");
        //Diplomacy table
        if (!m_diplomacyFoF.empty())
        {
            writeByte += parser.startSection("DIPLOMATION_FOF");
            writeByte += parser.writeDiplomacy(m_diplomacyFoF);
            parser.endSection();
        }

        //Diplomacy Names
        if (!m_aDiplomacyFieldName.isEmpty())
        {
            writeByte += parser.startSection("DIPLOMATION_PL_NAMES");
            writeByte += parser.writeStringArray(m_aDiplomacyFieldName, "DIPLOMATION_PL_NAMES");
            parser.endSection();
        }
        parser.endSection();
    }

    if(m_worldSet.bInit)
    {
        writeByte += parser.startSection("WORLD_SET");
        writeByte += parser.startSection("WS_WIND_DIR");
        writeByte += parser.writePlot(m_worldSet.m_windDirection);
        parser.endSection(); //"WS_WIND_DIR"

        writeByte += parser.startSection("WS_WIND_STR");
        writeByte += parser.writeFloat(m_worldSet.m_windStrength);
        parser.endSection(); //"WS_WIND_STR"

        writeByte += parser.startSection("WS_TIME");
        writeByte += parser.writeFloat(m_worldSet.m_time);
        parser.endSection(); //"WS_TIME"

        writeByte += parser.startSection("WS_AMBIENT");
        writeByte += parser.writeFloat(m_worldSet.m_ambient);
        parser.endSection(); //"WS_AMBIENT"

        writeByte += parser.startSection("WS_SUN_LIGHT");
        writeByte += parser.writeFloat(m_worldSet.m_sunLight);
        parser.endSection(); //Sun Light
        parser.endSection();//"WORLD_SET"
    }

    //if (!m_vss_section.isEmpty())
    {
        writeByte += parser.startSection("VSS_SECTION");
        writeByte += parser.writeByteArray(m_vss_section, uint(m_vss_section.size()));
        parser.endSection(); //"VSS_SECTION"
    }

    if (!m_directory.isEmpty())
    {
        writeByte += parser.startSection("DIRICTORY");
        writeByte += parser.writeByteArray(m_directory, uint(m_directory.size()));
        parser.endSection();//"DIRICTORY"
    }

    if (!m_directoryElements.isEmpty())
    {
        writeByte += parser.startSection("DIRICTORY_ELEMENTS");
        writeByte += parser.writeByteArray(m_directoryElements, uint(m_directoryElements.size()));
        parser.endSection(); //"DIRICTORY_ELEMENTS"
    }

    if(!m_aNode.empty())
    {
        COptBool* pOpt = dynamic_cast<COptBool*>(m_view->settings()->opt("freeCamera"));
        bool bSaveSelect = false;
        if(pOpt)
            bSaveSelect = pOpt->value();

        writeByte += parser.startSection("OBJECT_SECTION");
        for (const auto& node : m_aNode)
        {

            if (bSaveSelect && (node->nodeState() != ENodeState::eSelect)) //save selected objects only
                continue;

            writeByte += node->serialize(parser);
        }
        parser.endSection(); // OBJECT_SECTION
    }
    //selected nodes?

    parser.endSection(); //OBJECT_DB_FILE

    //graph
    if (!m_aiGraph.isEmpty())
    {
        writeByte += parser.startSection("AI_GRAPH");
        writeByte += parser.writeAiGraph(m_aiGraph, m_aiGraph.length());
        parser.endSection();
    }
    ei::log(eLogInfo, "End write mob");
    return;
}

void CMob::createNode(CNode *pNode)
{
    CNode* pNewNode = nullptr;
    ENodeType type = pNode->nodeType();
    if (type == ENodeType::eUnknown)
    {
        qDebug() << "cant recognize type of obj";
        return;
    }

    switch (type)
    {
    case ENodeType::eUnit:
    {
        pNewNode = new CUnit(*dynamic_cast<CUnit*>(pNode));
        break;
    }
    case ENodeType::eTorch:
    {
        pNewNode = new CTorch(*dynamic_cast<CTorch*>(pNode));
        break;
    }
    case ENodeType::eMagicTrap:
    {
        pNewNode = new CMagicTrap(*dynamic_cast<CMagicTrap*>(pNode));
        break;
    }
    case ENodeType::eLever:
    {
        pNewNode = new CLever(*dynamic_cast<CLever*>(pNode));
        break;
    }
    case ENodeType::eLight:
    {
        pNewNode = new CLight(*dynamic_cast<CLight*>(pNode));
        break;
    }
    case ENodeType::eSound:
    {
        pNewNode = new CSound(*dynamic_cast<CSound*>(pNode));
        break;
    }
    case ENodeType::eParticle:
    {
        pNewNode = new CParticle(*dynamic_cast<CParticle*>(pNode));
        break;
    }
    case ENodeType::eWorldObject:
    {
        pNewNode = new CWorldObj(*dynamic_cast<CWorldObj*>(pNode));
        break;
    }
    default:
    {
        Q_ASSERT("unknown node type" && false);
        break;
    }
    }

    if(nullptr == pNode)
    {
        Q_ASSERT("Failed to create new node" && false);
    }
    addNode(pNewNode);

    if(nullptr == pNewNode)
        return;

    //set map ID
    bool bFound = false;
    QVector<SRange> arrRange;
    for(auto& range : m_aMainRange)
        arrRange.append(SRange(range));
    for(auto& range : m_aSecRange)
        arrRange.append(SRange(range));

    for(auto& range : arrRange)
    {
        if(bFound)
            break;

        for(uint i(range.minRange); i<range.maxRange; ++i)
        {
            if(isFreeMapId(i))
            {
                pNewNode->setMapId(i);
                qDebug() << "id found:" << i;
                bFound = true;
                break;
            }
        }
    }

    if(!bFound)
    { // try to set any free number
        uint freeId(1000);
        QVector<uint> arrId;
        arrId.resize(m_aNode.size());

        for (int i(0); i<m_aNode.size(); ++i)
            arrId[i] = m_aNode[i]->mapId();

        for (; freeId<100000; ++freeId)
        {
            if(!arrId.contains(freeId))
            {
                pNewNode->setMapId(freeId);
                break;
            }
        }
    }
    pNewNode->setState(ENodeState::eSelect);


    return;
}

void CMob::deleteNode(CNode *pNode)
{
    const int ind = m_aNode.indexOf(pNode);
    if(ind >=0)
    {
        m_aDeletedNode.append(m_aNode.at(ind));
        //m_aNode.at(ind)->setState(ENodeState::eDraw);
        m_aNode.removeAt(ind);
        ei::log(eLogDebug, QString("delete node with %1").arg(pNode->mapId()));
    }
}

void CMob::clearSelect(bool bClearLogic)
{
    CNode* pNode;
    foreach (pNode, m_aNode)
    {
        if(pNode->nodeState() == ENodeState::eSelect)
            pNode->setState(ENodeState::eDraw);
        if(bClearLogic && pNode->nodeType() == ENodeType::eUnit)
        {
            CUnit* pUnit = dynamic_cast<CUnit*>(pNode);
            pUnit->clearLogicSelect();
        }
        else if(bClearLogic && pNode->nodeType() == ENodeType::eMagicTrap)
        {
            CMagicTrap* pTrap = dynamic_cast<CMagicTrap*>(pNode);
            pTrap->clearLogicSelect();
        }
    }
}

void CMob::saveAs(const QFileInfo& path)
{
    QString filePath = path.filePath();
    QFile file(filePath);
    try
    {
        file.open(QIODevice::WriteOnly);
        if (file.error() != QFile::NoError)
        {
            ei::log(eLogError, QString("Have no access to MOB file:%1").arg(file.fileName()));
            return;
        }
        QByteArray data;
        serializeMob(data);
        file.write(data);
        file.close();
    }
    catch (std::exception ex)
    {
        ei::log(eLogFatal, ex.what());
        file.close();
    }

}

void CMob::save()
{
    saveAs(m_filePath);
}
