#include <QString>
#include "secfile.h"
#include "ei_types.h"

bool SecFile::Read(QString &path)
{
    if(IsRead)
        return IsRead;
    QFile secFile(path);
    secFile.open(QIODevice::ReadOnly);
    if(secFile.error() != QFile::NoError)
    {
        if(secFile.error() != QFile::OpenError)
            secFile.close();
        return false;
    }
    secFile.close();

    QDataStream stream(&secFile);
    stream.setByteOrder(QDataStream::ByteOrder::LittleEndian);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);


    stream >> Header;
    if(Header.Signature != Signature)
        return false;

    SecVertex sv;
    for(int i(0); i<VerticesCount; i++)
    {
        stream >> sv;
        LandVertex.append(sv);
    }

    if(Header.Type == 3)
    {
        for(int i(0); i<VerticesCount; i++)
        {
            //SecVertex sv;
            stream >> sv;
            WaterVertex.append(sv);
        }
    }
    else
    {
        for(int i(0); i<VerticesCount; i++)
        {
            sv = {0, 0, 0, 0};
            WaterVertex.append(sv);
        }
    }

    ushort lt;
    for(int i(0); i<TilesCount; i++)
    {
        stream >> lt;
        LandTiles.append(lt);
    }

    if(Header.Type == 3)
    {
        ushort wt;
        for(int i(0); i<TilesCount; i++)
        {
            stream >> wt;
            WaterTiles.append(wt);
        }

        ushort wa;
        for(int i(0); i<TilesCount; i++)
        {
            stream >> wa;
            WaterAllow.append(wa);
        }
    }
    else
    {
        WaterTiles.append(0);
        WaterAllow.append(65535);
    }

    IsRead = true;
    return IsRead;
}

bool SecFile::Read(QByteArray &buffer)
{
    if(IsRead)
        return IsRead;
    QDataStream stream(&buffer, QIODevice::ReadOnly);
    stream.setByteOrder(QDataStream::ByteOrder::LittleEndian);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);

    stream >> Header;
    if(Header.Signature != Signature)
        return false;

    SecVertex sv;
    for(int i(0); i<VerticesCount; i++)
    {
        stream >> sv;
        LandVertex.append(sv);
    }

    if(Header.Type == 3)
    {
        for(int i(0); i<VerticesCount; i++)
        {
            //SecVertex sv;
            stream >> sv;
            WaterVertex.append(sv);
        }
    }
    else
    {
        for(int i(0); i<VerticesCount; i++)
        {
            sv = {0, 0, 0, 0};
            WaterVertex.append(sv);
        }
    }

    ushort lt;
    for(int i(0); i<TilesCount; i++)
    {
        stream >> lt;
        LandTiles.append(lt);
    }

    if(Header.Type == 3)
    {
        ushort wt;
        for(int i(0); i<TilesCount; i++)
        {
            stream >> wt;
            WaterTiles.append(wt);
        }

        ushort wa;
        for(int i(0); i<TilesCount; i++)
        {
            stream >> wa;
            WaterAllow.append(wa);
        }
    }
    else
    {
        WaterTiles.append(0);
        WaterAllow.append(65535);
    }

    IsRead = true;
    return IsRead;
}

SecFileHeader& SecFile::header()
{
    if(!IsRead)
    {
        throw new std::runtime_error("Sec-file must be initialize, use ReadFromFile function for initialize");
    }
    return Header;
}

QVector<SecVertex>& SecFile::landVertex()
{
    if(!IsRead)
    {
        throw new std::runtime_error("Sec-file must be initialize, use ReadFromFile function for initialize");
    }
    return LandVertex;
}

QVector<SecVertex>& SecFile::waterVertex()
{
    if(!IsRead)
    {
        throw new std::runtime_error("Sec-file must be initialize, use ReadFromFile function for initialize");
    }
    return WaterVertex;
}

QVector<ushort>& SecFile::landTiles()
{
    if(!IsRead)
    {
        throw new std::runtime_error("Sec-file must be initialize, use ReadFromFile function for initialize");
    }
    return LandTiles;
}

QVector<ushort>& SecFile::waterTiles()
{

    if(!IsRead)
    {
        throw new std::runtime_error("Sec-file must be initialize, use ReadFromFile function for initialize");
    }
    return WaterTiles;
}


QVector<ushort>& SecFile::waterAllow()
{
    if(!IsRead)
    {
        throw new std::runtime_error("Sec-file must be initialize, use ReadFromFile function for initialize");
    }
    return WaterAllow;
}
