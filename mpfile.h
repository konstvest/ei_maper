#ifndef MPFILE_H
#define MPFILE_H
#include <QString>
#include <QFile>
#include <QDataStream>
#include <QVector>

enum eMaterialType
{
    UndefMat,
    Terrain,
    WaterWithoutTexture,
    WaterMat,
    GrassMat
};


enum eTileType
{
    Grass = 0,
    Ground = 1,
    Stone = 2,
    Sand = 3,
    Rock = 4,
    Field = 5,
    Water = 6,
    Road = 7,
    Undefined = 8,
    Snow = 9,
    Ice = 10,
    Drygrass = 11,
    Snowballs = 12,
    Lava = 13,
    Swamp = 14,
    Highrock = 15,
    Last = 16
};


struct MpMaterial
{
    eMaterialType Type;
    float R, G, B, A;
    float SelfIllum;
    float WaveMultiplier;
    float WarpSpeed;
    float Reserved1;
    float Reserved2;
    float Reserved3;

    friend QDataStream& operator<< (QDataStream &os, MpMaterial const &mat)
    {
        return os << (int)mat.Type << mat.R << mat.G << mat.B << mat.A << mat.SelfIllum <<
                     mat.WaveMultiplier << mat.WarpSpeed << mat.Reserved1 <<
                     mat.Reserved2 << mat.Reserved3;
    }

    friend QDataStream& operator>> (QDataStream &is, MpMaterial &mat)
    {
        return is >> (qint32&)mat.Type >> mat.R >> mat.G >> mat.B >> mat.A >> mat.SelfIllum >>
                     mat.WaveMultiplier >> mat.WarpSpeed >> mat.Reserved1 >>
                     mat.Reserved2 >> mat.Reserved3;
    }
};

struct MpFileHeader
{
    uint   Signature;
    float   MaxZ;
    uint   SectorsXCount;
    uint   SectorsYCount;
    uint   TexturesCount;
    uint   TextureSize;
    uint   TilesCount;
    uint   TileSize;
    unsigned short MaterialsCount;
    uint   AnimTilesCount;

    friend QDataStream& operator>> (QDataStream &is, MpFileHeader &head)
    {
        return is >> head.Signature >> head.MaxZ >> head.SectorsXCount >> head.SectorsYCount  >>
                     head.TexturesCount >> head.TextureSize >> head.TilesCount >> head.TileSize >>
                     head.MaterialsCount >> head.AnimTilesCount;
    }

    friend QDataStream& operator<< (QDataStream &os, MpFileHeader const &head)
    {
        return os << head.Signature << head.MaxZ  << head.SectorsXCount << head.SectorsYCount <<
                     head.TexturesCount << head.TextureSize << head.TilesCount << head.TileSize <<
                     head.MaterialsCount << head.AnimTilesCount;
    }
};

struct MpAnimTile
{
    ushort TileIndex;
    ushort PhasesCount;

    friend QDataStream& operator>> (QDataStream &st, MpAnimTile &tile)
    {
        return st >> tile.TileIndex >> tile.PhasesCount;
    }

    friend QDataStream& operator<< (QDataStream &st, MpAnimTile &tile)
    {
        return st << tile.TileIndex << tile.PhasesCount;
    }
};

class MpFile
{
public:
    static const uint Signature = 0xce4af672;

    bool ReadFromFile(QString& path);
    MpFileHeader& header();
    QVector<MpMaterial>& materals();
    QVector<eTileType>& titleType();
    QVector<MpAnimTile>& animTiles();
private:
    bool IsRead = false;
    MpFileHeader Header;
    QVector<MpMaterial> Materials;
    QVector<eTileType>  TileTypes;
    QVector<MpAnimTile> AnimTiles;
};


#endif // MPFILE_H
