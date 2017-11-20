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
