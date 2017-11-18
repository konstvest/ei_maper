#ifndef SUPPORTMPRTYPES_H
#define SUPPORTMPRTYPES_H
#include <QString>
#include <QVector>
#include <QDataStream>

struct SecFileHeader
{
   const uint SecFileSignature = 0xcf4bf774;

    uint Signature;
    quint8 Type;

    friend QDataStream& operator>> (QDataStream &st, SecFileHeader &head)
    {
        return st >> head.Signature >> head.Type;
    }

    friend QDataStream& operator<< (QDataStream &st, SecFileHeader const &head)
    {
        return st << head.Signature << head.Type;
    }
};

struct SecVertex
{
    qint8  OffsetX;
    qint8  OffsetY;
    ushort Z;
    uint   PackedNormal;

    friend QDataStream& operator>> (QDataStream &st, SecVertex &vert)
    {
        return st >> vert.OffsetX >> vert.OffsetY >> vert.Z >> vert.PackedNormal;
    }

    friend QDataStream& operator<< (QDataStream &st, SecVertex const &vert)
    {
        return st << vert.OffsetX << vert.OffsetY << vert.Z << vert.PackedNormal;
    }

};

class SecFile
{
public:
    const int VerticesCount = 33 * 33;
    const int VerticesSideSize = 33;
    const int TilesCount = 16 * 16;
    const int TilesSideSize = 16;

    bool ReadSecFile(QString& path);
    SecFileHeader& getHeader();
    QVector<SecVertex>& getLandVertex();
    QVector<SecVertex>& getWaterVertex();
    QVector<ushort>& getLandTiles();
    QVector<ushort>& getWaterTiles();
private:
    SecFileHeader Header;
    QVector<SecVertex> LandVertex;
    QVector<SecVertex> WaterVertex;
    QVector<ushort> LandTiles;
    QVector<ushort> WaterTiles;
    QVector<ushort> WaterAllow;
};

#endif // SECFILE_H
