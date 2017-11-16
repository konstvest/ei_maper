#ifndef SUPPORTMPRTYPES_H
#define SUPPORTMPRTYPES_H
#include <QString>
#include <QDataStream>

using namespace std;


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
   Snow = 9,
   Ice = 10,
   Drygrass = 11,
   Snowballs = 12,
   Lava = 13,
   Swamp = 14,
   Undefined = 8,   //+=+=+=+=rly?! 14, 8, 15?+=+=+=+=
   Highrock = 15,
   Last = 16
};


struct MpMaterial
{
    eMaterialType Type;
    float R, G, B, A;     // Diffuse color of object    Try to use array instead 4 variables. //Можно создать структурку Palette, но возиться с массивами, я не хочу.
    float SelfIllum;      // Self illumination of object
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
    void ReadFromFile(QString& path);
    MpFileHeader& getHeader();
    QVector<MpMaterial>& getMaterals();
    QVector<eTileType>& getTitleType();
    QVector<MpAnimTile>& getAnimTiles();
private:
    MpFileHeader Header;
    QVector<MpMaterial> Materials;
    QVector<eTileType>  TileTypes;
    QVector<MpAnimTile> AnimTiles;

};


#endif // SUPPORTMPRTYPES_H
