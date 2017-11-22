#include <QDebug>
#include <QString>
#include "ei_utils.h"
#include <mpfile.h>

bool MpFile::ReadFromFile(QString& path)
{
    QFile mpFile(path);
    mpFile.open(QIODevice::ReadOnly);
    if(mpFile.error() != QFile::NoError)
    {
        if(mpFile.error() != QFile::OpenError)
            mpFile.close();
        return false;
    }
    mpFile.close();
    QDataStream stream(&mpFile);
    stream.setByteOrder(QDataStream::ByteOrder::LittleEndian);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);

    stream >> Header;
    if (Header.Signature != Signature)
        return false;

    MpMaterial mat;
    for(uint i(0); i<Header.MaterialsCount; i++)
    {
        stream >> mat;
        Materials.append(mat);
    }

    eTileType tileType;
    for(uint i(0); i<Header.TilesCount; i++)
    {
        stream >> (int&)tileType;
        TileTypes.append(tileType);
    }

    MpAnimTile animTitle;
    for(uint i(0); i<Header.AnimTilesCount; i++)
    {
        stream >> animTitle;
        AnimTiles.append(animTitle);
    }

    IsRead = true;
    return IsRead;
}

MpFileHeader& MpFile::header()
{
    if(!IsRead)
        throw EI_Utils::messages.MpOpenError;
    return Header;
}

QVector<MpMaterial>& MpFile::materals()
{
    if(!IsRead)
        throw EI_Utils::messages.MpOpenError;
    return Materials;
}


QVector<eTileType>& MpFile::titleType()
{
    if(!IsRead)
        throw EI_Utils::messages.MpOpenError;
    return TileTypes;
}

QVector<MpAnimTile>& MpFile::animTiles()
{
    if(!IsRead)
        throw EI_Utils::messages.MpOpenError;
    return AnimTiles;
}
