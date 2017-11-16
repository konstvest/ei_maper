#include <QDebug>
#include <QString>
#include <QDataStream>
#include "qmath.h"
#include <qfile.h>
#include <QDataStream>
#include <mpfile.h>

void MpFile::ReadFromFile(QString& path)
{
    //QDataStream &is;
    QFile mpFile(path);
    mpFile.open(QIODevice::ReadOnly);
    QDataStream is(&mpFile);
    is.setByteOrder(QDataStream::ByteOrder::LittleEndian);
    is.setFloatingPointPrecision(QDataStream::SinglePrecision);

    is >> Header;

    for(int i(0); i<Header.MaterialsCount; i++)
    {
        MpMaterial mat;
        is >> mat;
        Materials.append(mat);
    }

    for(int i(0); i<Header.TilesCount; i++)
    {
        eTileType tileType;
        is >> (int&)tileType;
        TileTypes.append(tileType);
    }

    for(int i(0); i<Header.AnimTilesCount; i++)
    {
        MpAnimTile animTitle;
        is >> animTitle;
        AnimTiles.append(animTitle);
    }
}
