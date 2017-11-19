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
    SecFileHeader& header();
    QVector<SecVertex>& landVertex();
    QVector<SecVertex>& waterVertex();
    QVector<ushort>& landTiles();
    QVector<ushort>& waterTiles();
    QVector<ushort>& waterAllow();
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
