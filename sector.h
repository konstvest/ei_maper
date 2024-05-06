#ifndef MAP_SEC_H
#define MAP_SEC_H

#include <QString>
#include <QVector>
#include <QFile>
#include <QDataStream>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include "types.h"
//#include "landscape.h"
//#include "primitives.h"

struct SSecHeader
{
    uint signature;
    quint8 type;

    friend QDataStream& operator>> (QDataStream& st, SSecHeader& head)
    {
        return st >> head.signature >> head.type;
    }

    friend QDataStream& operator<< (QDataStream& st, const SSecHeader& head)
    {
        return st << head.signature << head.type;
    }
};

struct SSecVertex
{
    qint8  xOffset;
    qint8  yOffset;
    ushort z;
    uint packedNormal;
    QVector3D normal;

    friend QDataStream& operator>> (QDataStream& st, SSecVertex& vert)
    {
        st >> vert.xOffset >> vert.yOffset >> vert.z >> vert.packedNormal;
        vert.normal.setX((((vert.packedNormal >> 11) & 0x7FF) - 1000.0f) / 1000.0f);
        vert.normal.setY(((vert.packedNormal & 0x7FF) - 1000.0f) / 1000.0f);
        vert.normal.setZ((vert.packedNormal >> 22) / 1000.0f);
        return st;
    }

    friend QDataStream& operator<< (QDataStream& st, const SSecVertex& vert)
    {
        return st << vert.xOffset << vert.yOffset << vert.z << vert.packedNormal;
    }
};

struct SSpecificQuad
{
    void addVertex(SVertexData* vertex) {m_aVertexPointer.append(vertex);}
    void rotate(int step = 0);
    int m_rotateState;
    QVector<SVertexData*> m_aVertexPointer;
};

struct STile
{
    STile(){}
    STile(ushort packedData);
    //void operator=(STile& tile) {m_index = tile.m_index; m_texture = tile.m_texture; m_rotation = tile.m_rotation;}
    ushort m_index;
    ushort m_texture;
    ushort m_rotation;
};

///
/// \brief The CSector class realizes a part of the game resources, from which the landscape is assembled
///
class CSector
{
public:
    CSector();
    CSector(QDataStream& stream, float maxZ, int texCount);
    ~CSector();
    void draw(QOpenGLShaderProgram* program);
    void drawWater(QOpenGLShaderProgram* program);
    void setIndex(UI2& index) {m_index = index; updatePosition(); }
    const UI2& index() {return m_index;}
    bool projectPt(QVector3D& point);

private:
    void updatePosition();
    void makeVertexData(QVector<QVector<SSecVertex>>& aLandVertex, QVector<STile>& aLandTile, QVector<QVector<SSecVertex>>& aWaterVertex, QVector<STile>& aWaterTile, float maxZ, int texCount);


private:
    QVector<short> m_aWaterAllow;
    UI2 m_index;
    QVector<SVertexData> m_aVertexData;
    //QVector<SSpecificQuad> m_aQuad;
    QOpenGLBuffer m_vertexBuf;
    QOpenGLBuffer m_indexBuf;
    QMatrix4x4 m_modelMatrix;

    QVector<SVertexData> m_aWaterData;
    QOpenGLBuffer m_waterVertexBuf;
    QOpenGLBuffer m_waterIndexBuf;
};

#endif // MAP_SEC_H
