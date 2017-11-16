//#ifndef MPRFILE_H
//#define MPRFILE_H
//#include <QString>
//#include <mpfile.h>

//struct MprMaterial  //=+=+=+=+do you realy need MpMaterial and MprMaterial?!
//{
//public: //=+=+=+public on default
//    eMaterialType Type;
//    float R, G, B, A;
//    float SelfIllum;
//    float WaveMultiplier;
//    float WarpSpeed;

//    MprMaterial(MpMaterial mat);
//    MpMaterial* ToMpMaterial();
//};

//struct MprAnimTile
//{
//public:
//    ushort TileIndex;
//    ushort PhasesCount;

//    MprAnimTile(MpAnimTile mpAnimTile);
//    MpAnimTile* ToMpAnimTile();
//};

//struct MprTile
//{
//public:
//    MprTile(ushort secTile);

//    ushort ToSecTile();
//    int Index;
//    int Angle;
//};

////struct MprVertex
////{
////public:
////    int Z;
////    signed char OffsetX;
////    signed char OffsetY;
////    float NormalX;
////    float NormalY;
////    float NormalZ;

////    MprVertex(SecVertex *secVertex);
////    SecVertex* ToSecVertex();
////};

//class MprFile
//{
//public:
//    QString path;

//    void Load(QString* path);
//    void Save(QString* path);

//    float GetMaxZ();
//    int GetSectorsXCount();
//    int GetSectorsYCount();
//    int GetTexturesCount();
//    int GetTextureSize();
//    int GetTileSize();

//    QVector<MprMaterial> Materials;
//    eTileType TileTypes;
//    QVector<MprAnimTile> AnimTiles;
//    QVector<QVector<MprVertex>> LandVertices;
//    QVector<QVector<MprVertex>> WaterVertices;
//    QVector<QVector<MprTile>> LandTiles;
//    QVector<QVector<MprTile>> WaterTiles;
//    int WaterMaterials[];
//private:
//    float MaxZ;
//    int SectorsXCount;
//    int SectorsYCount;
//    int TexturesCount;
//    int TextureSize;
//    int TileSize;
//};

//#endif // MPRFILE_H
