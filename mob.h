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
};

class CMob
{
public:
    CMob();
    ~CMob();
    void attachView(CView* view) {m_view = view;}
    void readMob(QFileInfo& path);
    void serializeJson(QFileInfo& file);
    void addNode(QList<CNode*>& aNode) {m_aNode.append(aNode); }
    void addNode(CNode* aNode) {m_aNode.append(aNode); }
    QList<CNode*>& nodes() {return m_aNode; }
    QList<CNode*>& nodesSelected() {return m_aNodeSelected; }
    void delNodes();
    CView* view() {Q_ASSERT(m_view); return m_view;}
    void log(const char* msg);

private:
    void init();
    QString getAuxDirName();
    bool deserialize(QByteArray data);
    void updateObjects();
    void writeData(QJsonObject& mob, const QFileInfo& file, const QString key, const QString value);
    void writeData(QJsonObject& mob, const QFileInfo& file, const QString key, QByteArray& value);

private:
    //todo: global text data, script
    CView* m_view;
    QString m_filePath;
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
    QList<CNode*> m_aNodeSelected;
};

#endif // MOB_H
