#ifndef SECFILE_H
#define SECFILE_H
#include <QString>
#include <QVector>
#include <QFile>
#include <QDataStream>

struct SecFileHeader
{
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
    static const uint Signature = 0xcf4bf774;
    static const int VerticesCount = 33 * 33;
    static const int VerticesSideSize = 33;
    static const int TilesCount = 16 * 16;
    static const int TilesSideSize = 16;

    bool Read(QString& path);
    bool Read(QByteArray& buffer);
    SecFileHeader& header();
    QVector<SecVertex>& landVertex();
    QVector<SecVertex>& waterVertex();
    QVector<ushort>& landTiles();
    QVector<ushort>& waterTiles();
    QVector<ushort>& waterAllow();
private:
    bool IsRead = false;
    SecFileHeader Header;
    QVector<SecVertex> LandVertex;
    QVector<SecVertex> WaterVertex;
    QVector<ushort> LandTiles;
    QVector<ushort> WaterTiles;
    QVector<ushort> WaterAllow;
};

#endif // SECFILE_H
