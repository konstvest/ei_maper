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
        uint32_t packedX = qBound(0, int((vert.normal.x() * 1000.0f) + 1000.0f), 2047) & 0x7FF;
        uint32_t packedY = qBound(0, int((vert.normal.y() * 1000.0f) + 1000.0f), 2047) & 0x7FF;
        uint32_t packedZ = qBound(0, int( vert.normal.z() * 1000.0f), 1023) & 0x3FF;

        // Упаковываем компоненты в одно 32-битное число
        vert.packedNormal = (packedZ << 22) | (packedX << 11) | packedY;
        return st << vert.xOffset << vert.yOffset << vert.z << vert.packedNormal;
    }
};

struct SSpecificQuad //todo: delete
{
    void addVertex(SVertexData* vertex) {m_aVertexPointer.append(vertex);}
    void rotate(int step = 0);
    int m_rotateState;
    QVector<SVertexData*> m_aVertexPointer;
};

struct STile //todo: delete
{
    STile(){}
    STile(ushort packedData);
    ushort packData(ushort m_index, ushort m_texture, ushort m_rotation);
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
class CLandTile //todo: rename to CTile
{
public:
    CLandTile();
    CLandTile(ushort packedData, ushort x, ushort y, float maxZ, int atlasNumber);
    virtual ~CLandTile();
    void resetVertices(QVector<SSecVertex>& arrVertex);
    void generateDrawVertexData(QVector<SVertexData>& outData, int& curIndex);
    bool pick(const QVector3D& point);
    virtual int tileIndex() const;
    ushort tileRotation() const {return m_rotNum;}
    void setTile(int index, int rotNum);
    const QVector<QVector<SSecVertex>>& arrVertex(){return m_arrVertex;}
    ushort packData();
protected:
    void reset();
    QVector3D pos(int row, int col);
protected:
    QVector<QVector<SSecVertex>> m_arrVertex; // matrix 3x3 of x,y offsets, z-altitude and normal
    ushort m_x; // start X (left pos to right). max X tile is 2.0f + m_x for third vertex (m_x + 0.0f, m_x + 1.0f, m_x + 2.0f1)
    ushort m_y; // start Y (bottom to top)
    ushort m_index; // index in atlas, 0-63
    ushort m_atlasTexIndex; // index of texture atlas
    ushort m_rotNum; //number of rotation
    float m_maxZ; // update for each tile when changing maximum altitude
    int m_texAtlasNumber; // update for each tile when changing atlas number
};

class CWaterTile : public CLandTile
{
public:
    CWaterTile();
    CWaterTile(ushort packedData, ushort x, ushort y, float maxZ, int atlasNumber);
    ~CWaterTile();
    int tileIndex() const override;
    void setMaterialIndex(short matIndex) {m_materialIndex = matIndex;}
    short materialIndex() const {return m_materialIndex;}
private:
    short m_materialIndex;
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
    QByteArray serializeSector();
    void draw(QOpenGLShaderProgram* program);
    void drawWater(QOpenGLShaderProgram* program);
    void setIndex(UI2& index) {m_index = index; updatePosition(); }
    const UI2& index() {return m_index;}
    bool projectPt(QVector3D& origin);
    bool pickTile(int& row, int& col, QVector3D& point, bool bLand = true);
    void setTile(QVector3D& point, int index, int rotNum, bool bLand = true, int matIndex = 0);
    const QVector<QVector<CLandTile>>& arrTile() {return m_arrLand;};
    const QVector<QVector<CWaterTile>>& arrWater() {return m_arrWater;};
    bool existsTileIndices(const QVector<int>& arrInd); // function for find incorrect\coorrupt tile indices

private:
    void updatePosition();
    void makeVertexData(QVector<QVector<SSecVertex>>& aLandVertex, QVector<STile>& aLandTile, QVector<QVector<SSecVertex>>& aWaterVertex, QVector<STile>& aWaterTile, float maxZ, int texCount);
    void generateVertexDataFromTile();


private:
    UI2 m_index;
    QMatrix4x4 m_modelMatrix;

    QVector<QVector<CLandTile>> m_arrLand;
    QVector<SVertexData> m_arrLandVrtData;
    QOpenGLBuffer m_vertexBuf;
    QOpenGLBuffer m_indexBuf;

    QVector<QVector<CWaterTile>> m_arrWater;
    QVector<SVertexData> m_arrWaterVrtData;
    QOpenGLBuffer m_waterVertexBuf;
    QOpenGLBuffer m_waterIndexBuf;
};

#endif // MAP_SEC_H
