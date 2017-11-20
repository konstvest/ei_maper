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
    };

    struct MpFileHeader
    {
        //uint   Signature;
        float   MaxZ;
        uint   SectorsXCount;
        uint   SectorsYCount;
        uint   TexturesCount;
        uint   TextureSize;
        uint   TilesCount;
        uint   TileSize;
        unsigned short MaterialsCount;
        uint   AnimTilesCount;
    };

    struct MpAnimTile
    {
        ushort TileIndex;
        ushort PhasesCount;
    };

    class MpFile
    {
    public:
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


#endif // SUPPORTMPRTYPES_H
