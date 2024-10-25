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

struct SSecHeader
{
    uint signature;
    quint8 type; // have liquids?

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
    uint32_t packedNormal; // temp var to unpack normal
    QVector3D normal;

    friend QDataStream& operator>> (QDataStream& st, SSecVertex& vert)
    {
        st >> vert.xOffset >> vert.yOffset >> vert.z >> vert.packedNormal;
        vert.normal.setX((((vert.packedNormal >> 11) & 0x7FF) - 1000.0f) / 1000.0f);
        vert.normal.setY(((vert.packedNormal & 0x7FF) - 1000.0f) / 1000.0f);
        vert.normal.setZ((vert.packedNormal >> 22) / 1000.0f);
        return st;
    }

    friend QDataStream& operator<< (QDataStream& st, SSecVertex& vert)
    {
        vert.packedNormal = 0;
        vert.packedNormal = (uint32_t)(vert.normal.z())*1000 << 22;
        vert.packedNormal |= (uint32_t)(vert.normal.x() * 1000 + 1000) << 11;
        vert.packedNormal |= (uint32_t)(vert.normal.y() * 1000 + 1000);
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


// nine vertices of tile
/*
6 _7 _8
|\ |\ |
|_\|_\|
3 _4 _5
|\ |\ |
|_\|_\|
0  1  2
*/
class CLandTile
{
public:
    CLandTile();
    CLandTile(ushort packedData, ushort x, ushort y, float maxZ, int atlasNumber);
    ~CLandTile();
    void resetVertices(QVector<SSecVertex>& arrVertex);
    void generateDrawVertexData(QVector<SVertexData>& outData, int& curIndex);
    bool pick(const QVector3D& point);
    int tileIndex();
    void setTile(int index, int rotNum);
private:
    void reset();
    QVector3D pos(int row, int col);
private:
    QVector<QVector<SSecVertex>> m_arrVertex; // matrix 3x3 of x,y offsets, z-altitude and normal
    ushort m_x; // start X (left pos to right). max X tile is 2.0f + m_x for third vertex (m_x + 0.0f, m_x + 1.0f, m_x + 2.0f1)
    ushort m_y; // start Y (bottom to top)
    ushort m_index; // index in atlas, 0-63
    ushort m_atlasTexIndex; // index of texture atlas
    ushort m_rotNum; //number of rotation
    float m_maxZ; // update for each tile when changing maximum altitude
    int m_texAtlasNumber; // update for each tile when changing atlas number
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
    bool projectPt(QVector3D& origin);
    bool pickTileIndex(int& index, QVector3D& point);
    void setTile(QVector3D& point, int index, int rotNum);

private:
    void updatePosition();
    bool pickTile(int& row, int& col, QVector3D& point);
    void makeVertexData(QVector<QVector<SSecVertex>>& aLandVertex, QVector<STile>& aLandTile, QVector<QVector<SSecVertex>>& aWaterVertex, QVector<STile>& aWaterTile, float maxZ, int texCount);
    void generateVertexDataFromTile();


private:
    QVector<short> m_aWaterAllow;
    UI2 m_index;
    QVector<SVertexData> m_aVertexData;
    QOpenGLBuffer m_vertexBuf;
    QOpenGLBuffer m_indexBuf;
    QMatrix4x4 m_modelMatrix;
    QVector<QVector<CLandTile>> m_arrTile;

    QVector<SVertexData> m_aWaterData;
    QOpenGLBuffer m_waterVertexBuf;
    QOpenGLBuffer m_waterIndexBuf;
};

#endif // MAP_SEC_H
