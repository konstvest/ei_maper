#ifndef MPRFILE_H
#define MPRFILE_H
#include "mpfile.h"
#include "secfile.h"

struct MprVertex
{
    int Z;
    quint8 OffsetX;
    quint8 OffsetY;
    float NormalX;
    float NormalY;
    float NormalZ;

    MprVertex(SecVertex sec);

    SecVertex ToSecVertex();
};

struct MprTile
{
    int Index;
    int Angle;

    MprTile(ushort secTile);

    ushort ToSecTile();
};

struct MprAnimTile
{
    ushort TileIndex;
    ushort PhasesCount;

    MprAnimTile();
    MprAnimTile(MpAnimTile mpAnimTile);

    MpAnimTile ToMpAnimTile();
};

struct MprMaterial
{
    eMaterialType Type;
    float R, G, B, A;
    float SelfIllum;
    float WaveMultiplier;
    float WarpSpeed;

    MprMaterial();
    MprMaterial(MpMaterial mat);

    MpMaterial ToMpMaterial();
};

class MprFile
{
public:
    void LoadFile(QString& path);

    QVector<MprMaterial> Materials;
    QVector<eTileType> TileTypes;
    QVector<MprAnimTile> AnimTiles;

    float maxZ();
    int sectorsXCount();
    int sectorsYCount();
    int texturesCount();
    int textureSize();
    int tileSize();

    QVector<QVector<MprVertex>>& landVertices();
    QVector<QVector<MprVertex>>& waterVertices();
    QVector<QVector<MprTile>>& landTiles();
    QVector<QVector<MprTile>>& waterTiles();
    QVector<QVector<int>>& waterMaterials();
private:
    float MaxZ;
    int SectorsXCount;
    int SectorsYCount;
    int TexturesCount;
    int TextureSize;
    int TileSize;

    QVector<QVector<MprVertex>> LandVertices;
    QVector<QVector<MprVertex>> WaterVertices;
    QVector<QVector<MprTile>> LandTiles;
    QVector<QVector<MprTile>> WaterTiles;
    QVector<QVector<int>> WaterMaterials;
};

#endif // MPRFILE_H
