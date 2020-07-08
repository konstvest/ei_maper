#ifndef UTILS_H
#define UTILS_H
#include <QDataStream>
#include <QMap>
#include <QVector>
#include <QString>
#include <QSharedPointer>
#include "types.h"

#define EPS 0.0000001

namespace util
{

void formatStream(QDataStream& stream);
void qNormalizeAngle(int& angle);
void normalizeAngle(float& angle);
void getCirclePoint(QVector<QVector3D>& aPoint, const QVector3D centr, const double radius, const int nPoint);
void splitByLen(QVector<QVector3D>& aPoint, float len);
bool isEqual(const double& a, const double& b, double Eps = EPS);


enum EType
{
    eUnknown = 0
    ,eAiGraph //граф проходимости
    ,eAreaArray
    ,eByte //(1 byte) 1б беззнаковое целое
    ,eDiplomacy //(4096 bytes) 32x32 матрица из 2б целых
    ,eDword // (4 bytes) 4б беззнаковое целое
    ,eFloat // (4 bytes) 4б вещественное
    ,eLeverStats // (12 bytes) параметры рычага //TODO: remove?! use plot instead
    ,eNull // (0 bytes) пустая нода
    ,ePlot //(12 bytes) 3 floats (vec3)
    ,ePlot2DArray
    ,eQuaternion // (16 bytes) 4 floats (vec4)
    ,eRecord // контейнер нод
    ,eRectangle
    ,eString // строка
    ,eStringArray // массив строк
    ,eStringEncrypted // зашифрованный скрипт уровня
    ,eUnitStats // (180 bytes) параметры существа
    ,eCount
};

struct STypeTable
{
    QString name;
    EType type;
};

struct SNode
{
    uint m_type;
    uint m_len;
    friend QDataStream& operator>> (QDataStream &st, SNode& pair)
    {
        return st >> pair.m_type >> pair.m_len;
    }
};

class CMobParser
{
public:
    //CMobParser() {}
    CMobParser(QByteArray& data);

    uint readAiGraph(QByteArray& data, uint len);
    uint readByteArray(QByteArray& data, uint len);
    uint readByte(char& data);
    uint readBool(bool& data);
    uint readDiplomacy(QVector<QVector<uint>>& data);
    uint readDword(uint& data);
    uint readDword(int& data);
    uint readFloat(float& data);
    uint readPlot(QVector3D& data);
    uint readPlot2DArray(QVector<QVector2D>& data);
    uint readAreaArray(QVector<SArea>& data);
    uint readQuaternion(QVector4D& data);
    uint readRectangle(SRectangle& data);
    uint readString(QString& data, uint len);
    uint readStringArray(QVector<QString>& data);
    uint readStringEncrypted(QString& data, uint len);
    uint readUnitStats(QSharedPointer<SUnitStat>& data, uint len);
    bool isNextTag(const char* tagname);
    void checkTag (const char* tag);
    uint skipTag();
    QString nextTag();
    uint skipHeader();
    uint readHeader();
    QString nodeName() {return m_aType[m_node.m_type].name; }
    uint nodeLen();


private:
    void initTypes();
    void decryptScript(QString& script, QByteArray& data, uint key);
    EType nodeType() {return m_aType[m_node.m_type].type; }
    QString nodeName(uint type) {return m_aType[type].name; }

private:
    QDataStream m_stream;
    QMap<uint, STypeTable> m_aType;
    SNode m_node;
    QVector<QString> m_stack;
};

}

#endif // UTILS_H
