#include <QDebug>
#include <QString>
#include <fstream>
#include "qmath.h"
#include <mprfile.h>
#include <mpfile.h>
//#include <supp


void MprFile::Load(QString *path)
{
    std::ifstream stream;
    stream.open(path->toLatin1(), std::ios::binary);

}
void MprFile::Save(QString* path)
{
    //QDebug(path);
}

float MprFile::GetMaxZ()
{
    return MaxZ;
}

int MprFile::GetSectorsXCount()
{
    return SectorsYCount;
}

int MprFile::GetSectorsYCount()
{
    return SectorsYCount;
}

int MprFile::GetTexturesCount()
{
    return TexturesCount;
}

int MprFile::GetTextureSize()
{
    return TextureSize;
}

int MprFile::GetTileSize()
{
    return TileSize;
}

MprAnimTile::MprAnimTile(MpAnimTile mpAnimTile)
{
    TileIndex = mpAnimTile.TileIndex;
    PhasesCount = mpAnimTile.PhasesCount;
}

MpAnimTile* MprAnimTile::ToMpAnimTile()
{
    MpAnimTile mpAnimTile;
    mpAnimTile.TileIndex = TileIndex;
    mpAnimTile.PhasesCount = PhasesCount;

    return &mpAnimTile; //!!!! ti vozvraschaew' to, 4to ydal9etsya posle vihoda iz fynkcii
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

MpMaterial *MprMaterial::ToMpMaterial()
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
    return &mat;    //!!!! ti vozvraschaew' to, 4to ydal9etsya posle vihoda iz fynkcii
}

MprVertex::MprVertex(SecVertex *secVertex)
{
    Z = secVertex->PackedNormal;
    OffsetX = secVertex->OffsetX;
    OffsetY = secVertex->OffsetY;

    uint normal = secVertex->PackedNormal;
    NormalX = (((normal >> 11) & 0x7FF) - 1000.0f) / 1000.0f;
    NormalY = ((normal & 0x7FF) - 1000.0f) / 1000.0f;
    NormalZ = (normal >> 22) / 1000.0f;
}

SecVertex* MprVertex::ToSecVertex()
{
    SecVertex secVertex;
    if (Z < 0 || Z > 65535)
        throw "Invalid argument \"Z\" while convert MprVertex to SecVertex";
    secVertex.Z = (uint)Z;
    secVertex.OffsetX = OffsetX;
    secVertex.OffsetY = OffsetY;

    if (NormalX < -1 || NormalX > 1 || NormalY < -1 || NormalY > 1 || NormalZ < 0 || NormalZ > 1)
        throw "Invalid normal or normals while convert MprVertex to SecVertex";
    uint normal = 0;
    normal |= (uint)floor(NormalX * 1000.0f + 1000.0f) << 11;
    normal |= (uint)floor(NormalY * 1000.0f + 1000.0f);
    normal |= (uint)floor(NormalZ * 1000.0f) << 22;
    secVertex.PackedNormal = normal;

    return &secVertex;  //!!!! ti vozvraschaew' to, 4to ydal9etsya posle vihoda iz fynkcii
}

MprTile::MprTile(ushort secTile)
{
   this->Index = secTile & 0x3FFF;
   this->Angle = secTile >> 14;
}

ushort MprTile::ToSecTile()
{
    if (Index < 0 || Index > 0x3FFF || Angle < 0 || Angle > 3)
    {
        //throw new ArgumentException();
    }

    ushort secTile = 0;
    secTile = (ushort)(Index & 0x3FFFu);
    secTile |= (ushort)(Angle << 14);
    return secTile;
}
