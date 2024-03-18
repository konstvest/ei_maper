#ifndef MOB_H
#define MOB_H

#include <QFileInfo>
#include <QMap>
#include <QDataStream>
#include <QVector>
#include <QVector3D>
#include <QOpenGLShader>

#include "types.h"
#include "node.h"

class CView;
class CProgressView;
class CPatrolPoint;
class CLookPoint;
class CActivationZone;
class CTrapCastPoint;

///
/// \brief The CMob class provides reading, saving and editing of *.mob file.
///
class CMob
{
public:
    CMob();
    ~CMob();
    void attach(CView* view, CProgressView* pProgress);
    void readMob(QFileInfo& path);
    void checkUniqueId(QSet<uint>& aId);
    void saveAs(const QFileInfo& path);
    void save();
    void serializeJson(const QFileInfo& file);
    void serializeMob(QByteArray& data);
    void addNode(QList<CNode*>& aNode) {m_aNode.append(aNode); }
    void addNode(CNode* aNode) {m_aNode.append(aNode); }
    void createNode(CNode* pNode);
    CNode* createNode(QJsonObject data);
    void undo_createNode(uint mapId);
    QList<CNode*>& nodes();
    void deleteNode(uint mapId);
    CNode* undo_deleteNode(uint mapId);
    void deleteNode(CNode* pNode);
    void clearSelect(bool bClearLogic = true);
    void delNodes();
    CNode* nodeByMapId(uint id);
    CView* view() {Q_ASSERT(m_view); return m_view;}
    QString mobName();
    const QFileInfo& filePath() {return m_filePath;}
    void setFileName(const QFileInfo& fileInfo) {m_filePath = fileInfo;}
    const CWorldSet& worldSet() {return m_worldSet;}
    void setWorldSet(const CWorldSet& ws){m_worldSet = ws;}
    const QVector<SRange>& ranges(bool bMain);
    void setRanges(bool bMain, const QVector<SRange>& range);
    void clearRanges(bool bMain);
    const QList<QString>& diplomacyNames() {return m_aDiplomacyFieldName;}
    void setDiplomacyNames(QList<QString>& arrName) {m_aDiplomacyFieldName = arrName;}
    QVector<QVector<uint>>& diplomacyField() {return m_diplomacyFoF;}
    void setDiplomacyField(const QVector<QVector<uint>>& df) {m_diplomacyFoF = df;}
    const QString& script() {return m_script;}
    void setScript(const QString& script) {m_script = script;}
    void setQuestMob(bool bQuest = true) {m_mobType = bQuest ? eEMobTypeQuest : eEMobTypeBase;}
    bool isQuestMob() {return m_mobType == EMobType::eEMobTypeQuest;}
    void setDurty(bool bDurty = true) {m_bDurty = bDurty;}
    bool isDurty() {return m_bDurty;}
    void generateDiplomacyTable();
    void clearDiplomacyTable();
    void addRange(bool bMain, const SRange range);
    const SRange& activeRange();
    uint activeRangeId() {return m_activeRangeId;}
    void setActiveRange(uint rangeId);

    //functions for logic processing
    QList<CNode*>& logicNodes();
    void logicNodesUpdate();
    void getPatrolHash(int& unitMapIdOut, int& pointIdOut, CPatrolPoint* pPoint);
    void getViewHash(int& unitMapIdOut, int& pointIdOut, int& viewIdOut, CLookPoint* pPoint);
    int getPatrolId(uint unitMapId, CPatrolPoint* pPoint);
    void createPatrolByHash(QString hash);
    void undo_createPatrolByHash(QString hash);
    CPatrolPoint* patrolPointById(int unitId, int patrolId);
    CLookPoint* viewPointById(int unitId, int patrolId, int viewId);
    void getTrapZoneHash(int& unitMapIdOut, int& zoneIdOut, CActivationZone* pZone);
    CActivationZone* actZoneById(int trapId, int zoneId);
    void getTrapCastHash(int& unitMapIdOut, int& pointIdOut, CTrapCastPoint* pCast);
    CTrapCastPoint* trapCastById(int trapId, int pointId);
    uint trapIdByPoint(CActivationZone* pZone);
    uint trapIdByPoint(CTrapCastPoint* pPoint);
    // <== end functions of logic

private:
    QString getAuxDirName();
    bool deserialize(QByteArray data);
    void updateObjects();
    void writeData(QJsonObject& mob, const QFileInfo& file, const QString key, const QString value);
    void writeData(QJsonObject& mob, const QFileInfo& file, const QString key, QByteArray& value);
    bool isFreeMapId(uint id);
    void generateMapId(CNode* pNode);
    void collectTreeView();

private:
    //todo: global text data, script
    CView* m_view;
    CProgressView* m_pProgress;
    QFileInfo m_filePath;
    uint m_scriptKey;
    QString m_script;
    QString m_textOld;
    QVector<SRange> m_aMainRange;
    QVector<SRange> m_aSecRange;
    QVector<QVector<uint>> m_diplomacyFoF;
    QList<QString> m_aDiplomacyFieldName;
    CWorldSet m_worldSet;
    QByteArray m_vss_section;
    QByteArray m_directory;
    QByteArray m_directoryElements;
    QByteArray m_aiGraph;

    //todo: include ALL nodes into these lists, for node assemly use children\parents
    QList<CNode*> m_aNode;
    QList<CNode*> m_aDeletedNode;
    QList<CNode*> m_aLogicNode;
    EMobType m_mobType;
    bool m_bDurty;
    uint m_activeRangeId;
};

#endif // MOB_H
