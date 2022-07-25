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

struct SWorldSet
{
    QVector3D m_windDirection;
    float m_windStrength;
    float m_time;
    float m_ambient;
    float m_sunLight; //power of sunlight ?!
    bool bInit;
};

enum EMobOrder
{
    eEMobOrderPrimary
    ,eEMobOrderSecondary
};

class CProgressView;
class CPatrolPoint;
class CLookPoint;

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
    QList<CNode*>& logicNodes();
    void deleteNode(uint mapId);
    void undo_deleteNode(uint mapId);
    void deleteNode(CNode* pNode);
    void clearSelect(bool bClearLogic = false);
    void delNodes();
    CNode* nodeByMapId(uint id);
    CView* view() {Q_ASSERT(m_view); return m_view;}
    QString mobName();
    const QFileInfo& filePath() {return m_filePath;}
    void setFileName(const QFileInfo& fileInfo) {m_filePath = fileInfo;}
    const SWorldSet& worldSet() {return m_worldSet;}
    void setWorldSet(const SWorldSet& ws){m_worldSet = ws;}
    const QVector<SRange>& mainRanges() {return m_aMainRange;}
    void setMainRanges(const QVector<SRange>& range) {m_aMainRange = range;}
    const QVector<SRange>& secRanges() {return m_aSecRange;}
    void setSecRanges(const QVector<SRange>& range) {m_aSecRange = range;}
    const QVector<QString>& diplomacyNames() {return m_aDiplomacyFieldName;}
    QVector<QVector<uint>>& diplomacyField() {return m_diplomacyFoF;}
    //void getDiplomacyField(QVector<QVector<uint>>& df) {df = m_diplomacyFoF;}
    void setDiplomacyField(const QVector<QVector<uint>>& df) {m_diplomacyFoF = df;}
    const QString& script() {return m_script;}
    void setScript(const QString& script) {m_script = script;}

    CNode* findUnitParent(CNode* pPointIn);
    void getPatrolHash(int& unitMapIdOut, int& pointIdOut, CPatrolPoint* pPoint);
    void getViewHash(int& unitMapIdOut, int& pointIdOut, int& viewIdOut, CLookPoint* pPoint);
    int getPatrolId(uint unitMapId, CPatrolPoint* pPoint);
    void createPatrolByHash(QString hash);
    void undo_createPatrolByHash(QString hash);

private:
    void init();
    QString getAuxDirName();
    bool deserialize(QByteArray data);
    void updateObjects();
    void writeData(QJsonObject& mob, const QFileInfo& file, const QString key, const QString value);
    void writeData(QJsonObject& mob, const QFileInfo& file, const QString key, QByteArray& value);
    void logicNodesUpdate();

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
    QVector<QString> m_aDiplomacyFieldName;
    SWorldSet m_worldSet;
    QByteArray m_vss_section;
    QByteArray m_directory;
    QByteArray m_directoryElements;
    QByteArray m_aiGraph;

    //todo: include ALL nodes into these lists, for node assemly use children\parents
    QList<CNode*> m_aNode;
    QList<CNode*> m_aDeletedNode;
    QList<CNode*> m_aLogicNode;
    EMobOrder m_order;
};

#endif // MOB_H
