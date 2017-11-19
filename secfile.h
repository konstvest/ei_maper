#ifndef SUPPORTMPRTYPES_H
#define SUPPORTMPRTYPES_H
#include <QString>
#include <QVector>

struct SecFileHeader
{
    uint Signature;
    quint8 Type;
};

struct SecVertex
{
    qint8  OffsetX;
    qint8  OffsetY;
    ushort Z;
    uint   PackedNormal;
};

class SecFile
{
public:
    bool ReadFromFile(QString& path);
    SecFileHeader& getHeader();
    SecFileHeader* getHeader(bool ptr);
    QVector<SecVertex>& getLandVertex();
    QVector<SecVertex>* getLandVertex(bool ptr);
    QVector<SecVertex>& getWaterVertex();
    QVector<SecVertex>* getWaterVertex(bool ptr);
    QVector<ushort>& getLandTiles();
    QVector<ushort>* getLandTiles(bool ptr);
    QVector<ushort>& getWaterTiles();
    QVector<ushort>* getWaterTiles(bool ptr);
    QVector<ushort>& getWaterAllow();
    QVector<ushort>* getWaterAllow(bool ptr);
private:
    static const int VerticesCount = 33 * 33;
    static const int VerticesSideSize = 33;
    static const int TilesCount = 16 * 16;
    static const int TilesSideSize = 16;

    bool IsRead = false;
    SecFileHeader Header;
    QVector<SecVertex> LandVertex;
    QVector<SecVertex> WaterVertex;
    QVector<ushort> LandTiles;
    QVector<ushort> WaterTiles;
    QVector<ushort> WaterAllow;
};

#endif // SECFILE_H
