#ifndef SUPPORTMPRTYPES_H
#define SUPPORTMPRTYPES_H
#include <QString>

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
    float R, G, B, A;     // Diffuse color of object    Try to use array instead 4 variables.
    float SelfIllum;      // Self illumination of object
    float WaveMultiplier;
    float WarpSpeed;
    float Reserved1;
    float Reserved2;
    float Reserved3;
};

struct MpFileHeader
{
    const uint MpFileSignature = 0xce4af672;

    uint   Signature;
    float  MaxZ;
    uint   SectorsXCount;
    uint   SectorsYCount;
    uint   TexturesCount;
    uint   TextureSize;
    uint   TilesCount;
    uint   TileSize;
    ushort MaterialsCount;
    uint   AnimTilesCount;
};

struct MpAnimTile
{
    ushort TileIndex;
    ushort PhasesCount;
};


struct MpFile
{
    MpFileHeader Header;
    MpMaterial Materials[]; //+=+= replace to vector, not array, QVector<MpMaterial> Materials. May be storage pointer instead? (<MpMaterial*>)
    eTileType  TileTypes[]; //+=+= replace to vector, not array
    MpAnimTile AnimTiles[]; //+=+= replace to vector, not array
};

struct SecFileHeader
{
   const uint SecFileSignature = 0xcf4bf774;

    uint Signature;
    char Type;
};

struct SecVertex
{
    signed char  OffsetX;
    signed char  OffsetY;
    ushort Z;
    uint   PackedNormal;
};


#endif // SUPPORTMPRTYPES_H
