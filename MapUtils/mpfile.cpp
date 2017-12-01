#include <QDebug>
#include <QString>
#include "mpfile.h"

bool MpFile::Read(QString& path)
{
    if(IsRead)
        return IsRead;
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

bool MpFile::Read(QByteArray& buffer)
{
    if(IsRead)
        return IsRead;

    QDataStream stream(&buffer, QIODevice::ReadOnly);
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
        throw new std::runtime_error("Mp-file must be initialize, use ReadFromFile function for initialize");
    return Header;
}

QVector<MpMaterial>& MpFile::materals()
{
    if(!IsRead)
        throw new std::runtime_error("Mp-file must be initialize, use ReadFromFile function for initialize");
    return Materials;
}


QVector<eTileType>& MpFile::titleTypes()
{
    if(!IsRead)
        throw new std::runtime_error("Mp-file must be initialize, use ReadFromFile function for initialize");
    return TileTypes;
}

QVector<MpAnimTile>& MpFile::animTiles()
{
    if(!IsRead)
        throw new std::runtime_error("Mp-file must be initialize, use ReadFromFile function for initialize");
    return AnimTiles;
}
