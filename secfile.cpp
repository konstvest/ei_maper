#include <QString>
#include <secfile.h>
#include "ei_types.h"
#include "ei_utils.h"

bool SecFile::ReadFromFile(QString &path)
{
    std::ifstream st;
    st.open(path.toStdString(), std::ios::binary);

    if(!st)
    {
        qDebug() << EI_Utils::messages.CantLoadFile << path;
        return false;
    }

    if(!EI_Utils::checkSignature(st, EI_Utils::sec))
    {
        //qDebug() << "incorrect signature";
        st.close();
        return false;
    }

    st.read((char*)&Header.Type, sizeof(Header.Type));

    for(int i(0); i<VerticesCount; i++)
    {
        SecVertex sv;
        st.read((char*)&sv, sizeof(sv));
        LandVertex.append(sv);
    }

    if(Header.Type == 3)
    {
        for(int i(0); i<VerticesCount; i++)
        {
            SecVertex sv;
            st.read((char*)&sv, sizeof(sv));
            WaterVertex.append(sv);
        }
    }
    else
    {
        for(int i(0); i<VerticesCount; i++)
        {
            SecVertex sv = {0, 0, 0, 0};
            WaterVertex.append(sv);
        }
    }

    for(int i(0); i<TilesCount; i++)
    {
        ushort lt;
        st.read((char*)&lt, sizeof(lt));
        LandTiles.append(lt);
    }

    if(Header.Type == 3)
    {
        for(int i(0); i<TilesCount; i++)
        {
            ushort wt;
            st.read((char*)&wt, sizeof(wt));
            WaterTiles.append(wt);
        }

        for(int i(0); i<TilesCount; i++)
        {
            ushort wa;
            st.read((char*)&wa, sizeof(wa));
            WaterAllow.append(wa);
        }
    }
    else
    {
        WaterTiles.append(0);
        WaterAllow.append(65535);
    }

    st.close();

    IsRead = true;
    return IsRead;
}

SecFileHeader& SecFile::getHeader()
{
    if(!IsRead)
    {
        throw EI_Utils::messages.SecOpenError;
    }
    return Header;
}

SecFileHeader* SecFile::getHeader(bool ptr)
{
    ptr = IsRead; //Подавление предупреждений
    if(!ptr)
    {
        throw EI_Utils::messages.SecOpenError;
    }
    return &Header;
}

QVector<SecVertex>& SecFile::getLandVertex()
{
    if(!IsRead)
    {
        throw EI_Utils::messages.SecOpenError;
    }
    return LandVertex;
}

QVector<SecVertex>* SecFile::getLandVertex(bool ptr)
{
    ptr = IsRead;
    if(!ptr)
    {
        throw EI_Utils::messages.SecOpenError;
    }
    return &LandVertex;
}

QVector<SecVertex>& SecFile::getWaterVertex()
{
    if(!IsRead)
    {
        throw EI_Utils::messages.SecOpenError;
    }
    return WaterVertex;
}

QVector<SecVertex>* SecFile::getWaterVertex(bool ptr)
{
    ptr = IsRead;
    if(!ptr)
    {
        throw EI_Utils::messages.SecOpenError;
    }
    return &WaterVertex;
}

QVector<ushort>& SecFile::getLandTiles()
{
    if(!IsRead)
    {
        throw EI_Utils::messages.SecOpenError;
    }
    return LandTiles;
}

QVector<ushort>* SecFile::getLandTiles(bool ptr)
{
    ptr = IsRead;
    if(!ptr)
    {
        throw EI_Utils::messages.SecOpenError;
    }
    return &LandTiles;
}

QVector<ushort>& SecFile::getWaterTiles()
{

    if(!IsRead)
    {
        throw EI_Utils::messages.SecOpenError;
    }
    return WaterTiles;
}

QVector<ushort>* SecFile::getWaterTiles(bool ptr)
{
    ptr = IsRead;
    if(!ptr)
    {
        throw EI_Utils::messages.SecOpenError;
    }
    return &WaterTiles;
}

QVector<ushort>& SecFile::getWaterAllow()
{
    if(!IsRead)
    {
        throw EI_Utils::messages.SecOpenError;
    }
    return WaterAllow;
}

QVector<ushort>* SecFile::getWaterAllow(bool ptr)
{
    ptr = IsRead;
    if(!ptr)
    {
        throw EI_Utils::messages.SecOpenError;
    }
    return &WaterAllow;
}
