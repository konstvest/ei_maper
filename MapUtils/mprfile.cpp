#include "mprfile.h"
#include "mpfile.h"
#include "secfile.h"
#include "resfile.h"
#include <math.h>

MprVertex::MprVertex(SecVertex sec)
{
    Z = sec.Z;
    OffsetX = sec.OffsetX;
    OffsetY = sec.OffsetY;

    uint normal = sec.PackedNormal;
    NormalX = (((normal >> 11) & 0x7FF) - 1000.0f) / 1000.0f;
    NormalY = ((normal & 0x7FF) - 1000.0f) / 1000.0f;
    NormalZ = (normal >> 22) / 1000.0f;
}

SecVertex MprVertex::ToSecVertex()
{
    SecVertex secVertex;
    if (Z < 0 || Z > 65535)
        throw "Invalid Z value";

    secVertex.Z = (ushort)Z;
    secVertex.OffsetX = OffsetX;
    secVertex.OffsetY = OffsetY;

    if (NormalX < -1 || NormalX > 1 || NormalY < -1 || NormalY > 1 || NormalZ < 0 || NormalZ > 1)
        throw "Invalid Normals";

    uint normal = 0;
    normal |= (uint)floor(NormalX * 1000.0f + 1000.0f) << 11;
    normal |= (uint)floor(NormalY * 1000.0f + 1000.0f);
    normal |= (uint)floor(NormalZ * 1000.0f) << 22;
    secVertex.PackedNormal = normal;
}

MprTile::MprTile(ushort secTile)
{
    Index = secTile & 0x3FFF;
    Angle = secTile >> 14;
}

ushort MprTile::ToSecTile()
{
    if (Index < 0 || Index > 0x3FFF || Angle < 0 || Angle > 3)
        throw "Invalid index or angle";

    ushort secTile = 0;
    secTile = (ushort)(Index & 0x3FFFu);
    secTile |= (ushort)(Angle << 14);
    return secTile;
}

MprAnimTile::MprAnimTile(MpAnimTile mpAnimTile)
{
    TileIndex = mpAnimTile.TileIndex;
    PhasesCount = mpAnimTile.PhasesCount;
}

MpAnimTile MprAnimTile::ToMpAnimTile()
{
    MpAnimTile mpAnimTile;
    mpAnimTile.TileIndex = TileIndex;
    mpAnimTile.PhasesCount = PhasesCount;
    return mpAnimTile;
}

MprMaterial::MprMaterial(MpMaterial mat)
{
    Type = mat.Type;
    R = mat.R;
    G = mat.G;
    B = mat.B;
    A = mat.A;
    SelfIllum = mat.SelfIllum;
    WaveMultiplier = mat.WaveMultiplier;
    WarpSpeed = mat.WarpSpeed;
}

MpMaterial MprMaterial::ToMpMaterial()
{
    MpMaterial mat;
    mat.Type = Type;
    mat.R = R;
    mat.G = G;
    mat.B = B;
    mat.A = A;
    mat.SelfIllum = SelfIllum;
    mat.WaveMultiplier = WaveMultiplier;
    mat.WarpSpeed = WarpSpeed;
    mat.Reserved1 = mat.Reserved2 = mat.Reserved3 = 0;
    return mat;
}

void getFormatNumber(int number, QString& strNum)
{
    //
    if(number < 100)
    {
        strNum = "0" + strNum;
        if(number < 10)
            strNum = "0" + strNum;
    }
}

void MprFile::LoadFile(QString &path)
{
    ResFile resArchive(path);

    auto buffers = resArchive.bufferOfFiles();
    auto entries = resArchive.entries();

    QString fileName = path.mid(path.lastIndexOf("/"), path.lastIndexOf(".") - path.lastIndexOf("/"));
    auto mpEntry = buffers[fileName + ".mp"];

    MpFile mpFile;
    mpFile.Read(mpEntry);
    MpFileHeader mpHeader = mpFile.header();

    MaxZ = mpHeader.MaxZ;
    SectorsXCount = (int)mpHeader.SectorsXCount;
    SectorsYCount = (int)mpHeader.SectorsYCount;
    TexturesCount = (int)mpHeader.TexturesCount;
    TextureSize   = (int)mpHeader.TextureSize;
    TileSize      = (int)mpHeader.TileSize;

    MpMaterial mpMat;
    auto materials = mpFile.materals();
    for(int i(0); i < materials.length(); i++)
    {
        mpMat = materials[i];
        MprMaterial mprMat(mpMat);
        Materials.append(mprMat);
    }

    TileTypes = mpFile.titleTypes();

    MpAnimTile mpAnimTile;
    for(int i(0); i < mpFile.animTiles().length(); i++)
    {
        mpAnimTile = mpFile.animTiles()[i];
        MprAnimTile mprAnimTile(mpAnimTile);
        AnimTiles.append(mprAnimTile);
    }

    int secXcount = (int)mpHeader.SectorsXCount;
    int secYcount = (int)mpHeader.SectorsYCount;
    int verXSize = secXcount * (SecFile::VerticesSideSize - 1) + 1;
    int verYSize = secYcount * (SecFile::VerticesSideSize - 1) + 1;
    int tilesXSize = secXcount * SecFile::TilesSideSize;
    int tilesYSize = secYcount * SecFile::TilesSideSize;

    for(int y(0); y < mpHeader.SectorsYCount; y++)
    {
        for(int x(0); x < mpHeader.SectorsXCount; x++)
        {
            QString xCoord = QString::number(x);
            QString yCoord = QString::number(y);
            getFormatNumber(x, xCoord);
            getFormatNumber(y, yCoord);
            auto secFileEntry = buffers[fileName + xCoord + yCoord + ".sec"];

        }
    }
}
