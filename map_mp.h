#ifndef MAP_MP_H
#define MAP_MP_H

#include <QString>
#include <QFile>
#include <QDataStream>
#include <QVector>

enum ETerrainType
{
    eTerrainBase = 0
    , eTerrainWaterBase
    , eTerrainGrass
    , eTerrainWater
    , eTerrainLast
};


enum ETileType
{
    eGrass = 0
    , eGround = 1
    , eStone = 2
    , eSand = 3
    , eRock = 4
    , eField = 5
    , eWater = 6
    , eRoad = 7
    , eEmpty = 8
    , eSnow = 9
    , eIce = 10
    , eDrygrass = 11
    , eSnowballs = 12
    , eLava = 13
    , eSwamp = 14
    , eHighrock = 15
    , eLast = 16
};


struct SMaterial
{
    ETerrainType type;
    float R, G, B, A;
    float selfIllumination;
    float waveMultiplier;
    float warpSpeed;
    float reserved1;
    float reserved2;
    float reserved3;

    friend QDataStream& operator<< (QDataStream& os, const SMaterial& mat)
    {
        return os << int(mat.type) << mat.R << mat.G << mat.B << mat.A <<
                     mat.selfIllumination << mat.waveMultiplier << mat.warpSpeed <<
                     mat.reserved1 << mat.reserved2 << mat.reserved3;
    }

    friend QDataStream& operator>> (QDataStream &is, SMaterial &mat)
    {
        return is >> (qint32&)mat.type >> mat.R >> mat.G >> mat.B >> mat.A >>
                     mat.selfIllumination >> mat.waveMultiplier >> mat.warpSpeed >>
                     mat.reserved1 >> mat.reserved2 >> mat.reserved3;
    }
};

struct SMapHeader
{
    uint   signature;
    float  maxZ;
    uint   nXSector;
    uint   nYSector;
    uint   nTexture;
    uint   textureSize;
    uint   nTileType;
    uint   tileSize;
    ushort nMaterial;
    uint   nAnimTile;

    friend QDataStream& operator>> (QDataStream& is, SMapHeader& head)
    {
        return is >> head.signature >> head.maxZ >> head.nXSector >> head.nYSector  >>
                     head.nTexture >> head.textureSize >> head.nTileType >> head.tileSize >>
                     head.nMaterial >> head.nAnimTile;
    }

    friend QDataStream& operator<< (QDataStream& os, const SMapHeader& head)
    {
        return os << head.signature << head.maxZ  << head.nXSector << head.nYSector <<
                     head.nTexture << head.textureSize << head.nTileType << head.tileSize <<
                     head.nMaterial << head.nAnimTile;
    }
};

struct SAnimTile
{
    ushort tileIndex;
    ushort nPhase;

    friend QDataStream& operator>> (QDataStream& st, SAnimTile& tile)
    {
        return st >> tile.tileIndex >> tile.nPhase;
    }

    friend QDataStream& operator<< (QDataStream& st, SAnimTile& tile)
    {
        return st << tile.tileIndex << tile.nPhase;
    }
};

#endif // MAP_MP_H
