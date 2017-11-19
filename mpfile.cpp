#include <QDebug>
#include <QString>
#include "ei_utils.h"
#include <mpfile.h>

bool MpFile::ReadFromFile(QString& path)
{
    std::ifstream st;
    st.open(path.toStdString(), std::ios::binary);

    if(!st)
    {
        qDebug() << EI_Utils::messages.CantLoadFile << path;
        return false;
    }

    if(!EI_Utils::checkSignature(st, EI_Utils::eSignatures::mp))
        return false;

    st.read((char*)&Header, sizeof(Header));

    for(uint i(0); i<Header.MaterialsCount; i++)
    {
        MpMaterial mat;
        st.read((char*)&mat, sizeof(mat));
        Materials.append(mat);
    }

    for(uint i(0); i<Header.TilesCount; i++)
    {
        eTileType tileType;
        st.read((char*)&tileType, sizeof(tileType));
        TileTypes.append(tileType);
    }

    for(uint i(0); i<Header.AnimTilesCount; i++)
    {
        MpAnimTile animTitle;
        st.read((char*)&animTitle, sizeof(animTitle));
        AnimTiles.append(animTitle);
    }
    st.close();

    IsRead = true;
    return IsRead;
}

MpFileHeader& MpFile::getHeader()
{
    if(!IsRead)
        throw EI_Utils::messages.MpOpenError;
    return Header;
}

MpFileHeader* MpFile::getHeader(bool ptr)
{
    ptr = IsRead;
    if(!ptr)
        throw EI_Utils::messages.MpOpenError;
    return &Header;
}

QVector<MpMaterial>& MpFile::getMaterals()
{
    if(!IsRead)
        throw EI_Utils::messages.MpOpenError;
    return Materials;
}

QVector<MpMaterial>* MpFile::getMaterals(bool ptr)
{
    ptr = IsRead;
    if(!ptr)
        throw EI_Utils::messages.MpOpenError;
    return &Materials;
}

QVector<eTileType>& MpFile::getTitleType()
{
    if(!IsRead)
        throw EI_Utils::messages.MpOpenError;
    return TileTypes;
}

QVector<eTileType>* MpFile::getTitleType(bool ptr)
{
    ptr = IsRead;
    if(!ptr)
        throw EI_Utils::messages.MpOpenError;
    return &TileTypes;
}

QVector<MpAnimTile>& MpFile::getAnimTiles()
{
    if(!IsRead)
        throw EI_Utils::messages.MpOpenError;
    return AnimTiles;
}

QVector<MpAnimTile>* MpFile::getAnimTiles(bool ptr)
{
    ptr = IsRead;
    if(!ptr)
        throw EI_Utils::messages.MpOpenError;
    return &AnimTiles;
}
